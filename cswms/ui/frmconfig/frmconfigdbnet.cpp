#include "frmconfigdbnet.h"
#include "ui_frmconfigdbnet.h"
#include "quihelper.h"
#include "dbconnthread.h"

frmConfigDbNet::frmConfigDbNet(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigDbNet)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmConfigDbNet::~frmConfigDbNet()
{
    delete ui;
}

void frmConfigDbNet::initForm()
{
    autoSetInfo = false;
    btnInRight = false;
    connName = "testdb";
    connFlag = QUIHelper::appName();
}

void frmConfigDbNet::initConfig()
{
    ui->btnUseNetDb->setChecked(AppConfig::UseNetDb);
    connect(ui->btnUseNetDb, SIGNAL(checkedChanged(bool)), this, SLOT(saveConfig()));

    ui->cboxDbType->addItems(DbHelper::getDbType());
    ui->cboxDbType->setCurrentIndex(ui->cboxDbType->findText(AppConfig::NetDbType));
    connect(ui->cboxDbType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtDbName->setText(AppConfig::NetDbName);
    connect(ui->txtDbName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtHostName->setText(AppConfig::NetHostName);
    connect(ui->txtHostName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtHostPort->setText(QString::number(AppConfig::NetHostPort));
    connect(ui->txtHostPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtUserName->setText(AppConfig::NetUserName);
    connect(ui->txtUserName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtUserPwd->setText(AppConfig::NetUserPwd);
    connect(ui->txtUserPwd, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
}

void frmConfigDbNet::saveConfig()
{
    AppConfig::UseNetDb = ui->btnUseNetDb->getChecked();
    AppConfig::NetDbType = ui->cboxDbType->currentText();
    AppConfig::NetDbName = ui->txtDbName->text();
    AppConfig::NetHostName = ui->txtHostName->text();
    AppConfig::NetHostPort = ui->txtHostPort->text().toInt();
    AppConfig::NetUserName = ui->txtUserName->text();
    AppConfig::NetUserPwd = ui->txtUserPwd->text();
    AppConfig::writeConfig();
}

void frmConfigDbNet::on_btnConnect_clicked()
{
    //加个大括号标明里面的变量作用域方便移除数据库连接
    {
        //构建数据库连接参数
        DbInfo dbInfo;
        dbInfo.connName = connName;
        dbInfo.dbName = AppConfig::NetDbName;
        dbInfo.hostName = AppConfig::NetHostName;
        dbInfo.hostPort = AppConfig::NetHostPort;
        dbInfo.userName = AppConfig::NetUserName;
        dbInfo.userPwd = AppConfig::NetUserPwd;

        //sqlite数据库需要设置数据库名称为数据库文件
        QString dbType = AppConfig::NetDbType.toUpper();
        if (dbType == "SQLITE") {
            dbInfo.dbName = DbHelper::getDbDefaultFile(connFlag);
            if (QFile(dbInfo.dbName).size() <= 4) {
                QUIHelper::showMessageBoxError("数据库文件不存在!", 5);
                return;
            }
        }

        //初始化数据库并测试连接
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

void frmConfigDbNet::on_btnInit_clicked()
{
    //先判断执行脚本是否存在
    QString sqlName = QString("%1/db/%2.sql").arg(QUIHelper::appPath()).arg(connFlag);
    QFile file(sqlName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QUIHelper::showMessageBoxError("数据库脚本文件打开失败!", 3);
        return;
    }

    //开启计时
    QElapsedTimer time;
    time.start();
    {
        //构建数据库连接参数
        DbInfo dbInfo;
        dbInfo.connName = connName;
        dbInfo.dbName = AppConfig::NetDbName;
        dbInfo.hostName = AppConfig::NetHostName;
        dbInfo.hostPort = AppConfig::NetHostPort;
        dbInfo.userName = AppConfig::NetUserName;
        dbInfo.userPwd = AppConfig::NetUserPwd;

        //sqlite数据库需要设置数据库名称为数据库文件
        QString dbType = AppConfig::NetDbType.toUpper();
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

void frmConfigDbNet::on_cboxDbType_activated(int)
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
