#include "frmconfigport.h"
#include "ui_frmconfigport.h"
#include "quihelper.h"
#include "dbquery.h"
#include "dbdelegate.h"
#include "datahelper.h"

frmConfigPort::frmConfigPort(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigPort)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->changeStyle();
}

frmConfigPort::~frmConfigPort()
{
    delete ui;
}

void frmConfigPort::showEvent(QShowEvent *)
{
    model->select();
}

void frmConfigPort::initForm()
{
    QUIHelper::initTableView(ui->tableView, AppData::RowHeight, false, true, false);
    ui->widgetTop->setProperty("flag", "navbtn");
    if (QUIHelper::deskWidth() < 1440) {
        ui->labTip->setText("提示 → 改动后需重启应用");
    } else {
        ui->labTip->setText("提示 → 必须和现场端口信息一致,改动后需重启应用");
    }
}

void frmConfigPort::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //指定表名
    model->setTable("PortInfo");
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
    columnNames << "编号" << "端口名称" << "端口类型" << "通讯协议" << "串口号" << "波特率" << "IP地址" << "网络端口" << "采集周期(秒)" << "通讯超时(次)" << "重连时间(秒)";
    columnWidths << 60 << 120 << 120 << 120 << 80 << 80 << 150 << 80 << 100 << 100 << 100;

    //挨个设置列名和列宽
    for (int i = 0; i < columnNames.count(); i++) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }

    //编号委托
    QStringList portID;
    for (int i = 1; i <= 30; i++) {
        portID.append(QString::number(i));
    }

    DbDelegate *d_cbox_portID = new DbDelegate(this);
    d_cbox_portID->setDelegateType("QComboBox");
    d_cbox_portID->setDelegateValue(portID);
    //ui->tableView->setItemDelegateForColumn(0, d_cbox_portID);

    //端口类型委托
    QStringList portType;
    portType << "COM" << "NET";
    DbDelegate *d_cbox_portType = new DbDelegate(this);
    d_cbox_portType->setDelegateType("QComboBox");
    d_cbox_portType->setDelegateValue(portType);
    ui->tableView->setItemDelegateForColumn(2, d_cbox_portType);

    //协议类型委托
    QStringList protocol;
    protocol << "TCPIPModbus" << "ModbusRTU" << "ModbusASCII" << "S7_200";
    DbDelegate *d_cbox_protocol = new DbDelegate(this);
    d_cbox_protocol->setDelegateType("QComboBox");
    d_cbox_protocol->setDelegateValue(protocol);
    ui->tableView->setItemDelegateForColumn(3, d_cbox_protocol);

    //用来切换样式自动改变颜色
    delegates << d_cbox_portID;
    delegates << d_cbox_portType;
}

void frmConfigPort::changeStyle()
{
    foreach (DbDelegate *delegate, delegates) {
        delegate->setTextColor(QUIConfig::TextColor);
        delegate->setSelectBgColor(QUIConfig::NormalColorStart);
    }
}

void frmConfigPort::on_btnAdd_clicked()
{
    int count = model->rowCount();
    model->insertRow(count);

    int portID = model->index(count - 1, 0).data().toInt() + 1;
    QString portName = model->index(count - 1, 1).data().toString();
    QString portType = model->index(count - 1, 2).data().toString();
    QString protocol = model->index(count - 1, 3).data().toString();
    QString comName = model->index(count - 1, 4).data().toString();
    int baudRate = model->index(count - 1, 5).data().toInt();
    QString tcpIP = model->index(count - 1, 6).data().toString();
    int tcpPort = model->index(count - 1, 7).data().toInt();
    int readInterval = model->index(count - 1, 8).data().toInt();
    int readTimeout = model->index(count - 1, 9).data().toInt();
    int readMaxtime = model->index(count - 1, 10).data().toInt();

    if (portName.startsWith("通信端口-")) {
        portName = QString("通信端口-%1").arg(portID);
    }

    QStringList list = tcpIP.split(".");
    if (list.count() == 4) {
        tcpIP = QString("%1.%2.%3.%4").arg(list.at(0)).arg(list.at(1)).arg(list.at(2)).arg(list.at(3).toInt() + 1);
    }

    //当前为第一行时候的默认值
    if (count == 0) {
        portID = 1;
        portName = "通信端口-1";
        portType = "COM";
        protocol = "ModbusRTU";
        comName = "COM1";
        baudRate = 9600;
        tcpIP = "127.0.0.1";
        tcpPort = 502;
        readInterval = 1;
        readTimeout = 3;
        readMaxtime = 60;
    }

    //设置新增加的行默认值
    model->setData(model->index(count, 0), portID);
    model->setData(model->index(count, 1), portName);
    model->setData(model->index(count, 2), portType);
    model->setData(model->index(count, 3), protocol);
    model->setData(model->index(count, 4), comName);
    model->setData(model->index(count, 5), baudRate);
    model->setData(model->index(count, 6), tcpIP);
    model->setData(model->index(count, 7), tcpPort);
    model->setData(model->index(count, 8), readInterval);
    model->setData(model->index(count, 9), readTimeout);
    model->setData(model->index(count, 10), readMaxtime);

    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigPort::on_btnSave_clicked()
{
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
        DbQuery::loadPortInfo();

        //先同步更新设备表对应的端口名称
        emit portNameChanged();
    } else {
        model->database().rollback();
        QUIHelper::showMessageBoxError("保存信息失败,信息不能为空,请重新填写!");
    }
}

void frmConfigPort::on_btnDelete_clicked()
{
    if (ui->tableView->currentIndex().row() < 0) {
        QUIHelper::showMessageBoxError("请选择要删除的端口!");
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要删除该端口吗? 对应端口的所有控制器会一并删除!") == QMessageBox::Yes) {
        int row = ui->tableView->currentIndex().row();
        QString portName = model->index(row, 1).data().toString();
        DbQuery::deleteDeviceInfo(portName);

        model->removeRow(row);
        model->submitAll();

        int count = model->rowCount();
        ui->tableView->setCurrentIndex(model->index(count - 1, 0));
    }
}

void frmConfigPort::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigPort::on_btnClear_clicked()
{
    int count = model->rowCount();
    if (count <= 0) {
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要清空所有端口信息吗?") == QMessageBox::Yes) {
        DbQuery::clearPortInfo();
        model->select();
    }
}

void frmConfigPort::dataout(int type)
{
    //type 0-打印 1-导出pdf 2-导出xls
    QString flag = "pdf";
    if (type == 2) {
        flag = "xls";
    }

    //弹出保存文件对话框
    QString fileName;
    if (type == 1 || type == 2) {
        QString defaultName = QString("portinfo_%1.%2").arg(STRDATETIME).arg(flag);
        QString filter = QString("保存文件(*.%1)").arg(flag);
        fileName = AppFun::getSaveFileName(defaultName, filter);
        if (fileName.isEmpty()) {
            return;
        }
    }

    QString columns = "PortID,PortName,PortType,ComName,BaudRate,TcpIP,TcpPort,ReadInterval,ReadTimeout,ReadMaxtime";
    QString where = "order by PortID asc";
    QStringList content = DataCsv::getContent("PortInfo", columns, where, "", ";");

    QString name = "端口信息";
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

void frmConfigPort::on_btnXls_clicked()
{
    dataout(2);
}

void frmConfigPort::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigPort::on_btnInput_clicked()
{
    AppFun::inputData(model, columnNames, "PortInfo", "端口");
    on_btnSave_clicked();
}

void frmConfigPort::on_btnOutput_clicked()
{
    AppFun::outputData("PortID asc", columnNames, "PortInfo", "端口");
}
