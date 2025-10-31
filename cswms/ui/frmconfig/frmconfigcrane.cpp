#include "frmconfigcrane.h"
#include "ui_frmconfigcrane.h"
#include "quihelper.h"
#include "dbquery.h"
#include "dbdelegate.h"
#include "datahelper.h"

frmConfigCrane::frmConfigCrane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmConfigCrane)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->changeStyle();
}

frmConfigCrane::~frmConfigCrane()
{
    delete ui;
}


void frmConfigCrane::showEvent(QShowEvent *)
{
    model->select();
}

void frmConfigCrane::initForm()
{
    QUIHelper::initTableView(ui->tableView, AppData::RowHeight, false, true, false);
    ui->widgetTop->setProperty("flag", "navbtn");
    if (QUIHelper::deskWidth() < 1440) {
        ui->labTip->setText("提示 → 改动后需重启应用");
    } else {
        ui->labTip->setText("提示 → 必须和现场控制器信息一致,改动后需重启应用");
    }
}

void frmConfigCrane::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //指定表名
    model->setTable("CraneInfo");
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
    columnNames << "编号" << "起重机名称" << "起重机类型" << "起重机型号" << "起重机吨位" << "起重机跨度" << "累计工作时间" << "工作时间" << "工作循环";
    columnWidths << 80 << 100 << 80 << 100 << 80 << 80 << 100 << 100 << 100;

    //挨个设置列名和列宽
    for (int i = 0; i < columnNames.count(); i++) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }

    //编号委托
    QStringList deviceID;
    for (int i = 1; i <= 255; i++) {
        deviceID.append(QString::number(i));
    }

    DbDelegate *d_cbox_deviceID = new DbDelegate(this);
    d_cbox_deviceID->setDelegateType("QComboBox");
    d_cbox_deviceID->setDelegateValue(deviceID);
    //ui->tableView->setItemDelegateForColumn(0, d_cbox_deviceID);

    //通信端口委托
    d_cbox_portName = new DbDelegate(this);
    d_cbox_portName->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(1, d_cbox_portName);
    portNameChanged();

    //设备地址委托
    QStringList deviceAddr;
    for (int i = 1; i <= 255; i++) {
        deviceAddr.append(QString::number(i));
    }

    DbDelegate *d_cbox_deviceAddr = new DbDelegate(this);
    d_cbox_deviceAddr->setDelegateType("QComboBox");
    d_cbox_deviceAddr->setDelegateValue(deviceAddr);
    ui->tableView->setItemDelegateForColumn(3, d_cbox_deviceAddr);

    //控制器型号委托,关联控制器型号,下拉变化右侧值改动
    d_cbox_deviceType = new DbDelegate(this);
    connect(d_cbox_deviceType, SIGNAL(valueChanged(QString)), this, SLOT(typeValueChanged(QString)));
    d_cbox_deviceType->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(4, d_cbox_deviceType);
    deviceTypeChanged();

    //变量数量委托
    DbDelegate *d_txt_nodeNumber = new DbDelegate(this);
    d_txt_nodeNumber->setDelegateType("QLineEdit");
    if (AppConfig::WorkMode == 0) {
        d_txt_nodeNumber->setDelegateEnable(false);
    } else {
        connect(d_txt_nodeNumber, SIGNAL(valueChanged(QString)), this, SLOT(numberValueChanged(QString)));
    }
    //connect(d_txt_nodeNumber, SIGNAL(valueChanged(QString)), this, SLOT(numberValueChanged(QString)));
    ui->tableView->setItemDelegateForColumn(5, d_txt_nodeNumber);

    //用来切换样式自动改变颜色
    delegates << d_cbox_deviceID;
    delegates << d_cbox_portName;
    delegates << d_cbox_deviceAddr;
    delegates << d_cbox_deviceType;
    delegates << d_txt_nodeNumber;
}

void frmConfigCrane::changeStyle()
{
    foreach (DbDelegate *delegate, delegates) {
        delegate->setTextColor(QUIConfig::TextColor);
        delegate->setSelectBgColor(QUIConfig::NormalColorStart);
    }
}

void frmConfigCrane::deviceTypeChanged()
{
    d_cbox_deviceType->setDelegateValue(DbData::TypeInfo_DeviceType);
}

void frmConfigCrane::portNameChanged()
{
    d_cbox_portName->setDelegateValue(DbData::PortInfo_PortName);
}

void frmConfigCrane::typeValueChanged(const QString &value)
{
    //自动设置最大探变量数量
    int nodeNumber = 8;
    int index = DbData::TypeInfo_DeviceType.indexOf(value);
    if (index >= 0) {
        nodeNumber = DbData::TypeInfo_NodeNumber.at(index);
    }

    int row = ui->tableView->currentIndex().row();
    model->setData(model->index(row, 5), nodeNumber);
}

void frmConfigCrane::numberValueChanged(const QString &value)
{
    //判断填写的最大值是否超过
    int row = ui->tableView->currentIndex().row();
    QString deviceType = model->index(row, 4).data().toString();
    int index = DbData::TypeInfo_DeviceType.indexOf(deviceType);
    if (index >= 0) {
        int nodeNumber = DbData::TypeInfo_NodeNumber.at(index);
        if (value.toInt() > nodeNumber) {
            model->setData(model->index(row, 5), nodeNumber);
            QUIHelper::showMessageBoxError(QString("当前变量最大变量数量为 %1 !\n请重新输入!").arg(nodeNumber));
        }
    }
}

void frmConfigCrane::on_btnAdd_clicked()
{
    int count = model->rowCount();
    model->insertRow(count);

    int deviceID = model->index(count - 1, 0).data().toInt() + 1;
    QString portName = model->index(count - 1, 1).data().toString();
    QString deviceName = model->index(count - 1, 2).data().toString();
    int deviceAddr = model->index(count - 1, 3).data().toInt() + 1;
    QString deviceType = model->index(count - 1, 4).data().toString();
    int nodeNumber = model->index(count - 1, 5).data().toInt();

    if (deviceName.endsWith("#调度室控制器")) {
        deviceName = QString("%1#调度室控制器").arg(deviceAddr);
    }

    //设置第一条数据,在没有上一条参考数据时
    if (count == 0) {
        deviceID = 1;
        portName = DbData::PortInfo_Count > 0 ? DbData::PortInfo_PortName.first() : "通信端口-1";
        deviceName = "1#调度室控制器";
        deviceAddr = 1;
        deviceType = "FC-1003-8";
        nodeNumber = 8;

        if (AppConfig::WorkMode == 2) {
            deviceName = "默认控制器";
            deviceType = "WF-1000";
            nodeNumber = 16;
        }
    }

    //设置新增加的行默认值
    model->setData(model->index(count, 0), deviceID);
    model->setData(model->index(count, 1), portName);
    model->setData(model->index(count, 2), deviceName);
    model->setData(model->index(count, 3), deviceAddr);
    model->setData(model->index(count, 4), deviceType);
    model->setData(model->index(count, 5), nodeNumber);

    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigCrane::on_btnSave_clicked()
{
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
        DbQuery::loadDeviceInfo();

        //先同步变量表的控制器名称
        emit deviceNameChanged();
    } else {
        model->database().rollback();
        QUIHelper::showMessageBoxError("保存信息失败,信息不能为空,请重新填写!");
    }
}

void frmConfigCrane::on_btnDelete_clicked()
{
    if (ui->tableView->currentIndex().row() < 0) {
        QUIHelper::showMessageBoxError("请选择要删除的控制器!");
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要删除该控制器吗? 控制器对应的所有变量都会被删除!") == QMessageBox::Yes) {
        int row = ui->tableView->currentIndex().row();
        QString deviceName = model->index(row, 2).data().toString();
        DbQuery::deleteNodeInfo(deviceName);

        model->removeRow(row);
        model->submitAll();

        int count = model->rowCount();
        ui->tableView->setCurrentIndex(model->index(count - 1, 0));
    }
}

void frmConfigCrane::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigCrane::on_btnClear_clicked()
{
    int count = model->rowCount();
    if (count <= 0) {
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要清空所有控制器信息吗?") == QMessageBox::Yes) {
        DbQuery::clearDeviceInfo();
        model->select();
    }
}

void frmConfigCrane::dataout(int type)
{
    //type 0-打印 1-导出pdf 2-导出xls
    QString flag = "pdf";
    if (type == 2) {
        flag = "xls";
    }

    //弹出保存文件对话框
    QString fileName;
    if (type == 1 || type == 2) {
        QString defaultName = QString("deviceinfo_%1.%2").arg(STRDATETIME).arg(flag);
        QString filter = QString("保存文件(*.%1)").arg(flag);
        fileName = AppFun::getSaveFileName(defaultName, filter);
        if (fileName.isEmpty()) {
            return;
        }
    }

    QString columns = "DeviceID,PortName,DeviceName,DeviceAddr,DeviceType,NodeNumber";
    QString where = "order by DeviceID asc";
    QStringList content = DataCsv::getContent("DeviceInfo", columns, where, "", ";");

    QString name = "控制器信息";
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

void frmConfigCrane::on_btnXls_clicked()
{
    dataout(2);
}

void frmConfigCrane::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigCrane::on_btnInput_clicked()
{
    AppFun::inputData(model, columnNames, "DeviceInfo", "控制器");
    on_btnSave_clicked();
}

void frmConfigCrane::on_btnOutput_clicked()
{
    AppFun::outputData("DeviceID asc", columnNames, "DeviceInfo", "控制器");
}
