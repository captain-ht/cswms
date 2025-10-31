#include "frmconfigdb.h"
#include "ui_frmconfigdb.h"
#include "quihelper.h"
#include "dbconnthread.h"

frmConfigDb::frmConfigDb(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigDb)
{
    ui->setupUi(this);
    this->initForm();
    this->initButton();
    this->initConfig();
}

frmConfigDb::~frmConfigDb()
{
    delete ui;
}

void frmConfigDb::setAutoSetInfo(bool autoSetInfo)
{
    this->autoSetInfo = autoSetInfo;
}

void frmConfigDb::setBtnInRight(bool btnInRight)
{
    this->btnInRight = btnInRight;
    this->initButton();
}

void frmConfigDb::setConnName(const QString &connName)
{
    this->connName = connName;
}

void frmConfigDb::setConnFlag(const QString &connFlag)
{
    this->connFlag = connFlag;
}

void frmConfigDb::initForm()
{
    autoSetInfo = false;
    btnInRight = false;
    connName = "testdb";
    connFlag = QUIHelper::appName();
}

void frmConfigDb::initButton()
{
    //将按钮移动到右侧
    if (btnInRight) {
        ui->btnConnect->setMinimumWidth(110);
        ui->btnConnect->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        ui->btnInit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(ui->btnConnect);
        layout->addWidget(ui->btnInit);
        ui->gridLayout->addLayout(layout, 0, 4, 3, 3);
    }
}

void frmConfigDb::initConfig()
{
    ui->cboxDbType->addItems(DbHelper::getDbType());
    ui->cboxDbType->setCurrentIndex(ui->cboxDbType->findText(AppConfig::LocalDbType));
    connect(ui->cboxDbType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtDbName->setText(AppConfig::LocalDbName);
    connect(ui->txtDbName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtHostName->setText(AppConfig::LocalHostName);
    connect(ui->txtHostName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtHostPort->setText(QString::number(AppConfig::LocalHostPort));
    connect(ui->txtHostPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtUserName->setText(AppConfig::LocalUserName);
    connect(ui->txtUserName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtUserPwd->setText(AppConfig::LocalUserPwd);
    connect(ui->txtUserPwd, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
}

void frmConfigDb::saveConfig()
{
    AppConfig::LocalDbType = ui->cboxDbType->currentText();
    AppConfig::LocalDbName = ui->txtDbName->text();
    AppConfig::LocalHostName = ui->txtHostName->text();
    AppConfig::LocalHostPort = ui->txtHostPort->text().toInt();
    AppConfig::LocalUserName = ui->txtUserName->text();
    AppConfig::LocalUserPwd = ui->txtUserPwd->text();
    AppConfig::writeConfig();
}

void frmConfigDb::on_btnConnect_clicked()
{
    {
        DbInfo dbInfo;
        dbInfo.connName = connName;
        dbInfo.dbName = AppConfig::LocalDbName;
        dbInfo.hostName = AppConfig::LocalHostName;
        dbInfo.hostPort = AppConfig::LocalHostPort;
        dbInfo.userName = AppConfig::LocalUserName;
        dbInfo.userPwd = AppConfig::LocalUserPwd;

        QString dbType = AppConfig::LocalDbType.toUpper();
        if (dbType == "SQLITE") {
            dbInfo.dbName = DbHelper::getDbDefaultFile(connFlag);
            if (QFile(dbInfo.dbName).size() <= 4) {
                QUIHelper::showMessageBoxError("数据库文件不存在!", 5);
                return;
            }
        }

        QSqlDatabase database;
        if (DbHelper::initDatabase(true, dbType, database, dbInfo)) {
            if (database.open()) {
                database.close();
                QUIHelper::showMessageBoxInfo("连接数据库成功!", 3);
            } else {
                QString error = database.lastError().text();
                QUIHelper::showMessageBoxError("连接数据库失败!\n" + error, 3);
            }
        } else {
            QUIHelper::showMessageBoxError("数据库服务器未开启!", 5);
        }
    }

    QSqlDatabase::removeDatabase(connName);
}

void frmConfigDb::on_btnInit_clicked()
{
    QString sqlName = QString("%1/db/%2.sql").arg(QUIHelper::appPath()).arg(connFlag);
    QFile file(sqlName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QUIHelper::showMessageBoxError("数据库脚本文件打开失败!", 3);
        return;
    }

    QElapsedTimer time;
    time.start();
    {
        DbInfo dbInfo;
        dbInfo.connName = connName;
        dbInfo.dbName = AppConfig::LocalDbName;
        dbInfo.hostName = AppConfig::LocalHostName;
        dbInfo.hostPort = AppConfig::LocalHostPort;
        dbInfo.userName = AppConfig::LocalUserName;
        dbInfo.userPwd = AppConfig::LocalUserPwd;

        QString dbType = AppConfig::LocalDbType.toUpper();
        if (dbType == "SQLITE") {
            dbInfo.dbName = DbHelper::getDbDefaultFile(connFlag);
            //先删除原来的数据库文件
            QFile(dbInfo.dbName).remove();
        }

        //初始化数据库连接并打开数据库
        QSqlDatabase database;
        if (!DbHelper::initDatabase(true, dbType, database, dbInfo)) {
            QUIHelper::showMessageBoxError("数据库服务器未开启!", 5);
            return;
        }
        if (!database.open()) {
            QString error = database.lastError().text();
            QUIHelper::showMessageBoxError("连接数据库失败!\n" + error, 3);
            return;
        }

        QSqlQuery query(QSqlDatabase::database(connName));

        //第一步,删除原有数据库
        QString sql = QString("DROP DATABASE IF EXISTS %1;").arg(dbInfo.dbName);
        qDebug() << TIMEMS << "sql:" << sql << "result:" << query.exec(sql);

        //第二步,新建数据库
        sql = QString("CREATE DATABASE %1;").arg(dbInfo.dbName);
        qDebug() << TIMEMS << "sql:" << sql << "result:" << query.exec(sql);

        //第三步,切换到新建的数据库库并执行建表语句
        database.close();
        if (!DbHelper::initDatabase(false, dbType, database, dbInfo)) {
            QUIHelper::showMessageBoxError("数据库服务器未开启!", 5);
            return;
        }
        if (!database.open()) {
            QString error = database.lastError().text();
            QUIHelper::showMessageBoxError("打开数据库失败!\n" + error, 3);
            return;
        }

        //将执行出错的sql语句输出到文件方便查看
        QString fileName2 = QString("%1/db/error.sql").arg(QUIHelper::appPath());
        QFile file2(fileName2);

        QSqlQuery query2(QSqlDatabase::database(connName));

        sql = "BEGIN;";
        qDebug() << TIMEMS << "sql:" << sql << "result:" << query2.exec(sql);

        while (!file.atEnd()) {
            sql = QString::fromUtf8(file.readLine());
            sql.replace("\n", "");

            //有些数据库不支持的语句跳过去
            if (DbHelper::existNoSupportSql(sql)) {
                continue;
            }

            //重新纠正sql语句
            DbHelper::checkSql(dbType, sql);

            if (!query2.exec(sql)) {
                //打印及输出错误信息
                QString error = query2.lastError().text();
                qDebug() << TIMEMS << "sql:" << sql << error;

                //没打开则先打开
                if (!file2.isOpen()) {
                    file2.open(QFile::WriteOnly | QFile::Append);
                }

                QString msg = QString("时间[%1]  语句: %2  错误: %3\n").arg(DATETIME).arg(sql).arg(error);
                file2.write(msg.toUtf8());
            }
        }

        sql = "COMMIT;";
        qDebug() << TIMEMS << "sql:" << sql << "result:" << query2.exec(sql);
        database.close();
    }

    QSqlDatabase::removeDatabase(connName);
    double ms = time.elapsed();
    QString info = QString("数据库脚本执行成功,总共用时 %1 秒!").arg(QString::number(ms / 1000, 'f', 1));
    QUIHelper::showMessageBoxInfo(info, 3);
}

void frmConfigDb::on_cboxDbType_activated(int)
{
    //自动切换默认端口号和其他信息
    QString hostPort, userName, userPwd;
    QString dbType = ui->cboxDbType->currentText().toUpper();
    DbHelper::getDbDefaultInfo(dbType, hostPort, userName, userPwd);
    if (!hostPort.isEmpty() && autoSetInfo) {
        ui->txtHostPort->setText(hostPort);
        ui->txtUserName->setText(userName);
        ui->txtUserPwd->setText(userPwd);
    }
}
