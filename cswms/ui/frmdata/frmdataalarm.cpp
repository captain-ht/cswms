#include "frmdataalarm.h"
#include "ui_frmdataalarm.h"
#include "quihelper.h"
#include "dbquery.h"
#include "dbpage.h"
#include "datahelper.h"

frmDataAlarm::frmDataAlarm(QWidget *parent) : QWidget(parent), ui(new Ui::frmDataAlarm)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    on_btnSelect_clicked();
}

frmDataAlarm::~frmDataAlarm()
{
    delete ui;
}

void frmDataAlarm::initForm()
{
    ui->frameRight->setProperty("flag", "navbtn");
    ui->frameBottom->setProperty("flag", "navlab");
    ui->cboxDeviceName->addItems(DbData::DeviceInfo_DeviceName);
    ui->cboxContent->addItems(AppData::AlarmTypes);

    ui->dateStart->setDate(QDate::currentDate());
    ui->dateEnd->setDate(QDate::currentDate().addDays(1));
    ui->dateStart->calendarWidget()->setLocale(QLocale::Chinese);
    ui->dateEnd->calendarWidget()->setLocale(QLocale::Chinese);
    QUIHelper::initTableView(ui->tableView, AppData::RowHeight);

    //增加鼠标右键删除
    QAction *action = new QAction("删除记录", this);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(doAction()));
    ui->tableView->addAction(action);
    if (AppData::CurrentUserType.contains("管理员")) {
        ui->tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
    }
}

void frmDataAlarm::initData()
{
    whereSql = "where 1=1";
    columnNames << "编号" << "位号" << "控制器名称" << "变量名称" << "报警值" << "单位" << "报警类型"
                << "触发时间" << "结束时间" << "确认用户" << "确认时间" << "确认信息";
    if (QUIHelper::deskWidth() < 1440) {
        columnWidths << 40 << 80 << 120 << 80 << 50 << 0 << 100 << 135 << 0 << 80 << 135 << 100;
    } else {
        columnWidths << 60 << 90 << 130 << 100 << 50 << 0 << 100 << 135 << 0 << 80 << 135 << 110;
    }

    //实例化翻页类
    dbPage = new DbPage(this);
    //关联查询记录总数
    connect(DbData::DbLocal, SIGNAL(receiveCount(QString, int, int)), dbPage, SLOT(receiveCount(QString, int, int)));

    dbPage->setAllCenter(true);
    dbPage->setColumnNames(columnNames);
    dbPage->setColumnWidths(columnWidths);
    dbPage->setRecordsPerpage(AppConfig::RecordsPerpage);
    dbPage->setDbType(AppConfig::LocalDbType);
    dbPage->setTableName("AlarmLog");
    dbPage->setOrderSql(QString("LogID %1").arg(AppConfig::AlarmLogOrder));
    dbPage->setControl(ui->tableView, ui->labPageTotal, ui->labPageCurrent, ui->labResultTotal, ui->labRecordsPerpage,
                       ui->labSelectTime, 0, ui->btnFirst, ui->btnPrevious, ui->btnNext, ui->btnLast);

    //2019-6-24 隐藏结束时间,大部分时候不需要
    ui->tableView->setColumnHidden(5, true);
    ui->tableView->setColumnHidden(8, true);
}

void frmDataAlarm::doAction()
{
    if (!AppFun::checkPermission("删除记录")) {
        return;
    }

    QAction *action = (QAction *)sender();
    QString text = action->text();
    if (text == "删除记录") {
        if (QUIHelper::showMessageBoxQuestion("确定要删除选中的记录吗?") == QMessageBox::Yes) {
            //统计当前选中了多少行记录,找出唯一标识,逐个删除
            QStringList ids;
            QItemSelectionModel *selections = ui->tableView->selectionModel();
            QModelIndexList selected = selections->selectedIndexes();
            foreach (QModelIndex index, selected) {
                if (index.column() == 0) {
                    ids << index.data().toString();
                }
            }

            DbQuery::deleteAlarmLog(ids.join(","));
            on_btnSelect_clicked();
            QString msg = "删除报警记录成功";
            DbQuery::addUserLog(msg);
            QUIHelper::showMessageBoxError(msg, 3);
        }
    }
}

void frmDataAlarm::on_btnSelect_clicked()
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
            sql += " and datetime(StartTime)>='" + dateStart.toString("yyyy-MM-dd HH:mm:ss") + "'";
            sql += " and datetime(StartTime)<='" + dateEnd.toString("yyyy-MM-dd HH:mm:ss") + "'";
        } else if (AppConfig::LocalDbType.toUpper() == "MYSQL") {
            sql += " and unix_timestamp(StartTime)>=unix_timestamp('" + dateStart.toString("yyyy-MM-dd HH:mm:ss") + "')";
            sql += " and unix_timestamp(StartTime)<=unix_timestamp('" + dateEnd.toString("yyyy-MM-dd HH:mm:ss") + "')";
        }
    }

    if (ui->ckDeviceName->isChecked()) {
        sql += " and DeviceName='" + ui->cboxDeviceName->currentText() + "'";
    }

    if (ui->ckNodeName->isChecked()) {
        sql += " and NodeName='" + ui->cboxNodeName->currentText() + "'";
    }

    if (ui->ckContent->isChecked()) {
        sql += " and Content='" + ui->cboxContent->currentText() + "'";
    }

    if (ui->ckNodeValue->isChecked()) {
        sql += " and NodeValue='" + ui->txtNodeValue->text() + "'";
    }

    if (ui->ckPositionID->isChecked()) {
        sql += " and PositionID='" + ui->txtPositionID->text() + "'";
    }

    //绑定数据到表格
    whereSql = sql;
    dbPage->setWhereSql(whereSql);
    dbPage->select();
    DbData::DbLocal->selectCount("AlarmLog", "LogID", whereSql);
}

void frmDataAlarm::dataout(int type)
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
        QString defaultName = QString("alarmlog_%1.%2").arg(STRDATETIME).arg(flag);
        QString filter = QString("保存文件(*.%1)").arg(flag);
        fileName = AppFun::getSaveFileName(defaultName, filter);
        if (fileName.isEmpty()) {
            return;
        }
    }

    //设定导出数据字段及排序字段
    QString columns = "LogID,PositionID,DeviceName,NodeName,NodeValue,NodeSign,Content,StartTime,ConfirmUser,ConfirmTime,ConfirmContent";
    QString where = whereSql + " order by " + QString("LogID %1").arg(AppConfig::AlarmLogOrder);
    QStringList content = DataCsv::getContent("AlarmLog", columns, where, "", ";");

    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "编号" << "位号" << "控制器名称" << "变量名称" << "报警值" << "单位" << "报警类型"
                << "触发时间" << "确认用户" << "确认时间" << "确认信息";
    columnWidths << 50 << 80 << 120 << 100 << 50 << 50 << 100 << 135 << 80 << 135 << 120;

    QString name = ui->ckNodeName->isChecked() ? ui->cboxNodeName->currentText() : "所有变量";
    name = name + "报警记录";
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
    dataContent.landscape = true;
    DataHelper::DataOut(dataContent);

    //插入日志记录+询问打开文件
    QString msg = QString("导出%1").arg(name);
    if (type == 0) {
        msg = QString("打印%1").arg(name);
    }
    DbQuery::addUserLog(msg);
    QUIHelper::openFile(fileName, msg);
}

void frmDataAlarm::on_btnXls_clicked()
{
    dataout(2);
}

void frmDataAlarm::on_btnPrint_clicked()
{
    dataout(0);
}

void frmDataAlarm::on_btnPdf_clicked()
{
    dataout(1);
}

void frmDataAlarm::on_btnDelete_clicked()
{
    if (!AppFun::checkPermission("删除记录")) {
        return;
    }

    QString timeStart, timeEnd;
    QUIHelper::showDateSelect(timeStart, timeEnd);
    if (!timeStart.isEmpty() && !timeEnd.isEmpty()) {
        if (QUIHelper::showMessageBoxQuestion("确定要删除该时间段的数据吗?") == QMessageBox::Yes) {
            DbQuery::deleteAlarmLog(timeStart, timeEnd);
            QString msg = "删除报警记录成功";
            DbQuery::addUserLog(msg);
            QUIHelper::showMessageBoxInfo(QString("%1!").arg(msg), 3);

            //重新查询数据
            on_btnSelect_clicked();
        }
    }
}

void frmDataAlarm::on_btnClear_clicked()
{
    if (!AppFun::checkPermission("删除记录")) {
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要清空所有数据吗?") == QMessageBox::Yes) {
        DbQuery::clearAlarmLog();
        QString msg = "清空报警记录成功";
        DbQuery::addUserLog(msg);
        QUIHelper::showMessageBoxInfo(QString("%1!").arg(msg), 3);

        //重新查询数据
        on_btnSelect_clicked();
    }
}

void frmDataAlarm::on_cboxDeviceName_currentIndexChanged(int)
{
    ui->cboxNodeName->clear();
    for (int i = 0; i < DbData::NodeInfo_Count; i++) {
        if (DbData::NodeInfo_DeviceName.at(i) == ui->cboxDeviceName->currentText()) {
            ui->cboxNodeName->addItem(DbData::NodeInfo_NodeName.at(i), DbData::NodeInfo_PositionID.at(i));
        }
    }
}

void frmDataAlarm::on_cboxNodeName_currentIndexChanged(int index)
{
    ui->txtPositionID->setText(ui->cboxNodeName->itemData(index).toString());
}

void frmDataAlarm::on_tableView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    if (row < 0) {
        return;
    }

    QAbstractItemModel *model = ui->tableView->model();
    QString logID = model->index(row, 0).data().toString();
    QString confirmContent = model->index(row, 11).data().toString();

    //弹出输入确认信息,为空说明单击了取消
    confirmContent = QUIHelper::showInputBox("请输入确认信息", 0, 0, "", false, confirmContent);
    if (!confirmContent.isEmpty()) {
        DbQuery::confirmAlarmLog(logID, confirmContent);
        on_btnSelect_clicked();
    }
}
