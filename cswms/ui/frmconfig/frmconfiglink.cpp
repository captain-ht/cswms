#include "frmconfiglink.h"
#include "ui_frmconfiglink.h"
#include "quihelper.h"
#include "dbquery.h"
#include "dbdelegate.h"
#include "datahelper.h"

frmConfigLink::frmConfigLink(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigLink)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->changeStyle();
}

frmConfigLink::~frmConfigLink()
{
    delete ui;
}

void frmConfigLink::showEvent(QShowEvent *)
{
    model->select();
}

void frmConfigLink::initForm()
{
    QUIHelper::initTableView(ui->tableView, AppData::RowHeight, false, true, false);
    ui->widgetTop->setProperty("flag", "navbtn");
    if (QUIHelper::deskWidth() < 1440) {
        ui->labTip->setText("提示 → 改动后需重启应用");
    } else {
        ui->labTip->setText("提示 → 必须和现场联动信息一致,改动后需重启应用");
    }
}

void frmConfigLink::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //指定表名
    model->setTable("AlarmLink");
    //设置列排序
    model->setSort(0, Qt::AscendingOrder);
    //设置提交模式
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //立即查询一次
    model->select();

    //将数据库表模型设置到表格上
    ui->tableView->setModel(model);
    ui->tableView->setProperty("model", true);

    //初始化列名和列宽
    columnNames << "位号" << "串口号" << "波特率" << "模块地址" << "联动地址集合" << "启用";
    columnWidths << 150 << 100 << 100 << 90 << 220 << 40;

    //挨个设置列名和列宽
    for (int i = 0; i < columnNames.count(); i++) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }

    //位号委托
    d_cbox_positionID = new DbDelegate(this);
    d_cbox_positionID->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(0, d_cbox_positionID);
    positionIDChanged();

    //启用禁用委托
    DbDelegate *d_ckbox_linkEnable = new DbDelegate(this);
    d_ckbox_linkEnable->setDelegateColumn(5);
    d_ckbox_linkEnable->setDelegateType("QCheckBox");
    d_ckbox_linkEnable->setCheckBoxText("启用", "禁用");
    ui->tableView->setItemDelegateForColumn(5, d_ckbox_linkEnable);

    //用来切换样式自动改变颜色
    delegates << d_cbox_positionID;
    delegates << d_ckbox_linkEnable;
}

void frmConfigLink::changeStyle()
{
    foreach (DbDelegate *delegate, delegates) {
        delegate->setTextColor(QUIConfig::TextColor);
        delegate->setSelectBgColor(QUIConfig::NormalColorStart);
    }
}

void frmConfigLink::positionIDChanged()
{
    d_cbox_positionID->setDelegateValue(DbData::NodeInfo_PositionID);
}

void frmConfigLink::on_btnAdd_clicked()
{
    if (DbData::NodeInfo_Count == 0) {
        QUIHelper::showMessageBoxError("还未添加变量,请先添加好变量!", 3);
        return;
    }

    int count = model->rowCount();
    model->insertRow(count);

    QString positionID = model->index(count - 1, 0).data().toString();
    QString comName = model->index(count - 1, 1).data().toString();
    int baudRate = model->index(count - 1, 2).data().toInt();
    quint8 modelAddr = model->index(count - 1, 3).data().toInt() + 1;
    QString linkAddr = model->index(count - 1, 4).data().toString();
    QString linkEnable = model->index(count - 1, 5).data().toString();

    //当前为第一行时候的默认值
    if (count == 0) {
        positionID = DbData::NodeInfo_PositionID.first();
        comName = "COM1";
        baudRate = 9600;
        modelAddr = 1;
        linkAddr = "1";
        linkEnable = "启用";
    }

    //设置新增加的行默认值
    model->setData(model->index(count, 0), positionID);
    model->setData(model->index(count, 1), comName);
    model->setData(model->index(count, 2), baudRate);
    model->setData(model->index(count, 3), modelAddr);
    model->setData(model->index(count, 4), linkAddr);
    model->setData(model->index(count, 5), linkEnable);

    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigLink::on_btnSave_clicked()
{
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
        DbQuery::loadAlarmLink();
    } else {
        model->database().rollback();
        QUIHelper::showMessageBoxError("保存信息失败,信息不能为空,请重新填写!");
    }
}

void frmConfigLink::on_btnDelete_clicked()
{
    if (ui->tableView->currentIndex().row() < 0) {
        QUIHelper::showMessageBoxError("请选择要删除的联动信息!");
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要删除该联动信息吗?") == QMessageBox::Yes) {
        int row = ui->tableView->currentIndex().row();
        model->removeRow(row);
        model->submitAll();

        int count = model->rowCount();
        ui->tableView->setCurrentIndex(model->index(count - 1, 0));
    }
}

void frmConfigLink::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigLink::on_btnClear_clicked()
{
    int count = model->rowCount();
    if (count <= 0) {
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要清空所有联动信息吗?") == QMessageBox::Yes) {
        DbQuery::clearAlarmLink();
        model->select();
    }
}

void frmConfigLink::dataout(int type)
{
    //type 0-打印 1-导出pdf 2-导出xls
    QString flag = "pdf";
    if (type == 2) {
        flag = "xls";
    }

    //弹出保存文件对话框
    QString fileName;
    if (type == 1 || type == 2) {
        QString defaultName = QString("linkinfo_%1.%2").arg(STRDATETIME).arg(flag);
        QString filter = QString("保存文件(*.%1)").arg(flag);
        fileName = AppFun::getSaveFileName(defaultName, filter);
        if (fileName.isEmpty()) {
            return;
        }
    }

    QString columns = "PositionID,ComName,BaudRate,ModelAddr,LinkAddr,LinkEnable";
    QString where = "order by PositionID asc";
    QStringList content = DataCsv::getContent("AlarmLink", columns, where, "", ";");

    QString name = "报警联动信息";
    DataContent dataContent;

    //pdf和xls需要设置文件名
    if (type == 1 || type == 2) {
        dataContent.fileName = fileName;
    }
    //xls需要设置表名
    if (type == 2) {
        dataContent.sheetName = name;
    }

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

void frmConfigLink::on_btnXls_clicked()
{
    dataout(2);
}

void frmConfigLink::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigLink::on_btnInput_clicked()
{
    AppFun::inputData(model, columnNames, "AlarmLink", "报警联动");
    on_btnSave_clicked();
}

void frmConfigLink::on_btnOutput_clicked()
{
    AppFun::outputData("PositionID asc", columnNames, "AlarmLink", "报警联动");
}
