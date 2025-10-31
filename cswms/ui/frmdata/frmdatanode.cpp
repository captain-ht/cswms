#include "frmdatanode.h"
#include "ui_frmdatanode.h"
#include "quihelper.h"
#include "dbquery.h"
#include "dbpage.h"
#include "datahelper.h"

frmDataNode::frmDataNode(QWidget *parent) : QWidget(parent), ui(new Ui::frmDataNode)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    on_btnSelect_clicked();
}

frmDataNode::~frmDataNode()
{
    delete ui;
}

void frmDataNode::initForm()
{
    ui->frameRight->setProperty("flag", "navbtn");
    ui->frameBottom->setProperty("flag", "navlab");
    ui->cboxDeviceName->addItems(DbData::DeviceInfo_DeviceName);

    ui->dateStart->setDate(QDate::currentDate());
    ui->dateEnd->setDate(QDate::currentDate().addDays(1));
    ui->dateStart->calendarWidget()->setLocale(QLocale::Chinese);
    ui->dateEnd->calendarWidget()->setLocale(QLocale::Chinese);
    QUIHelper::initTableView(ui->tableView, AppData::RowHeight);
}

void frmDataNode::initData()
{
    whereSql = "where 1=1";
    columnNames = AppData::NodeDataColumns.split("|");
    columnWidths << 60 << 100 << 130 << 130 << 60 << 70 << 150;

    //实例化翻页类
    dbPage = new DbPage(this);
    //关联查询记录总数
    connect(DbData::DbLocal, SIGNAL(receiveCount(QString, int, int)), dbPage, SLOT(receiveCount(QString, int, int)));

    dbPage->setAllCenter(true);
    dbPage->setColumnNames(columnNames);
    dbPage->setColumnWidths(columnWidths);
    dbPage->setRecordsPerpage(AppConfig::RecordsPerpage);
    dbPage->setDbType(AppConfig::LocalDbType);
    dbPage->setTableName("NodeLog");
    dbPage->setOrderSql(QString("LogID %1").arg(AppConfig::NodeLogOrder));
    dbPage->setControl(ui->tableView, ui->labPageTotal, ui->labPageCurrent, ui->labResultTotal, ui->labRecordsPerpage,
                       ui->labSelectTime, 0, ui->btnFirst, ui->btnPrevious, ui->btnNext, ui->btnLast);
}

void frmDataNode::on_btnSelect_clicked()
{
    QDateTime dateStart = ui->dateStart->dateTime();
    QDateTime dateEnd = ui->dateEnd->dateTime();
    if (dateStart > dateEnd) {
        QUIHelper::showMessageBoxError("开始时间不能大于结束时间!", 3);
        return;
    }

    //构建SQL语句
    QString sql = "where 1=1";
    if (ui->ckTimeStart->isChecked()) {
        if (AppConfig::LocalDbType.toUpper() == "SQLITE") {
            sql += " and datetime(SaveTime)>='" + dateStart.toString("yyyy-MM-dd HH:mm:ss") + "'";
            sql += " and datetime(SaveTime)<='" + dateEnd.toString("yyyy-MM-dd HH:mm:ss") + "'";
        } else if (AppConfig::LocalDbType.toUpper() == "MYSQL") {
            sql += " and unix_timestamp(SaveTime)>=unix_timestamp('" + dateStart.toString("yyyy-MM-dd HH:mm:ss") + "')";
            sql += " and unix_timestamp(SaveTime)<=unix_timestamp('" + dateEnd.toString("yyyy-MM-dd HH:mm:ss") + "')";
        }
    }

    if (ui->ckDeviceName->isChecked()) {
        sql += " and DeviceName='" + ui->cboxDeviceName->currentText() + "'";
    }

    if (ui->ckNodeName->isChecked()) {
        sql += " and NodeName='" + ui->cboxNodeName->currentText() + "'";
    }

    if (ui->ckPositionID->isChecked()) {
        sql += " and PositionID='" + ui->txtPositionID->text() + "'";
    }

    //绑定数据到表格
    whereSql = sql;
    dbPage->setWhereSql(whereSql);
    dbPage->select();
    DbData::DbLocal->selectCount("NodeLog", "LogID", whereSql);
}

void frmDataNode::dataout(int type)
{
    //type 0-打印 1-导出pdf 2-导出xls
    int maxCount = 5000;
    int warnCount = 500;
    QString flag = "pdf";
    if (type == 2) {
        maxCount = 100000;
        warnCount = 10000;
        flag = "xls";
    }

    //先判断行数,超过一定大小弹出提示
    int rowCount = dbPage->getRecordsTotal();
    if (!QUIHelper::checkRowCount(rowCount, maxCount, warnCount)) {
        return;
    }

    //弹出保存文件对话框
    QString fileName;
    if (type == 1 || type == 2) {
        QString defaultName = QString("nodelog_%1.%2").arg(STRDATETIME).arg(flag);
        QString filter = QString("保存文件(*.%1)").arg(flag);
        fileName = AppFun::getSaveFileName(defaultName, filter);
        if (fileName.isEmpty()) {
            return;
        }
    }

    //设定导出数据字段及排序字段
    QString columns = "LogID,PositionID,DeviceName,NodeName,NodeValue,NodeSign,SaveTime";
    QString where = whereSql + " order by " + QString("LogID %1").arg(AppConfig::NodeLogOrder);
    QStringList content = DataCsv::getContent("NodeLog", columns, where, "", ";");

    QString name = ui->ckDeviceName->isChecked() ? ui->cboxDeviceName->currentText() : "所有变量";
    name = name + "运行记录";
    DataContent dataContent;

    //pdf和xls需要设置文件名
    if (type == 1 || type == 2) {
        dataContent.fileName = fileName;
    }
    //xls需要设置表名
    if (type == 2) {
        dataContent.sheetName = name;
    }

    //下面这些参数是公共的都必须设置的
    dataContent.title = name;
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    dataContent.content = content;
    dataContent.landscape = false;
    DataHelper::DataOut(dataContent);

    //插入日志记录+询问打开文件
    QString msg = QString("导出%1").arg(name);
    if (type == 0) {
        msg = QString("打印%1").arg(name);
    }
    DbQuery::addUserLog(msg);
    QUIHelper::openFile(fileName, msg);
}

void frmDataNode::on_btnXls_clicked()
{
    dataout(2);
}

void frmDataNode::on_btnPrint_clicked()
{
    dataout(0);
}

void frmDataNode::on_btnPdf_clicked()
{
    dataout(1);
}

void frmDataNode::on_btnDelete_clicked()
{
    if (!AppFun::checkPermission("删除记录")) {
        return;
    }

    QString timeStart, timeEnd;
    QUIHelper::showDateSelect(timeStart, timeEnd);
    if (!timeStart.isEmpty() && !timeEnd.isEmpty()) {
        if (QUIHelper::showMessageBoxQuestion("确定要删除该时间段的数据吗?") == QMessageBox::Yes) {
            DbQuery::deleteNodeLog(timeStart, timeEnd);
            QString msg = "删除运行记录成功";
            DbQuery::addUserLog(msg);
            QUIHelper::showMessageBoxInfo(QString("%1!").arg(msg), 3);

            //重新查询数据
            on_btnSelect_clicked();
        }
    }
}

void frmDataNode::on_btnClear_clicked()
{
    if (!AppFun::checkPermission("删除记录")) {
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要清空所有数据吗?") == QMessageBox::Yes) {
        DbQuery::clearNodeLog();
        QString msg = "清空运行记录成功";
        DbQuery::addUserLog(msg);
        QUIHelper::showMessageBoxInfo(QString("%1!").arg(msg), 3);

        //重新查询数据
        on_btnSelect_clicked();
    }
}

void frmDataNode::on_cboxDeviceName_currentIndexChanged(int)
{
    ui->cboxNodeName->clear();
    for (int i = 0; i < DbData::NodeInfo_Count; i++) {
        if (DbData::NodeInfo_DeviceName.at(i) == ui->cboxDeviceName->currentText()) {
            ui->cboxNodeName->addItem(DbData::NodeInfo_NodeName.at(i), DbData::NodeInfo_PositionID.at(i));
        }
    }
}

void frmDataNode::on_cboxNodeName_currentIndexChanged(int index)
{
    ui->txtPositionID->setText(ui->cboxNodeName->itemData(index).toString());
}
