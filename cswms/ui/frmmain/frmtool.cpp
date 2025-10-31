#include "frmtool.h"
#include "ui_frmtool.h"
#include "quihelper.h"
#include "appinit.h"
#include "dbquery.h"

frmTool::frmTool(QWidget *parent) : QWidget(parent), ui(new Ui::frmTool)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->initConfig();
    on_btnOpen_clicked();
    on_btnListen_clicked();
    QUIHelper::setFormInCenter(this);
}

frmTool::~frmTool()
{
    delete ui;
}

void frmTool::initForm()
{
    //读取数据
    isComOk = false;
    timerRead = new QTimer(this);
    timerRead->setInterval(100);
    connect(timerRead, SIGNAL(timeout()), this, SLOT(readDataCom()));

    isTcpOk = false;
    tcpSocket = 0;
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    spinBox << ui->spinBox1_1 << ui->spinBox1_2 << ui->spinBox1_3 << ui->spinBox1_4
            << ui->spinBox1_5 << ui->spinBox1_6 << ui->spinBox1_7 << ui->spinBox1_8
            << ui->spinBox1_9 << ui->spinBox1_10 << ui->spinBox1_11 << ui->spinBox1_12
            << ui->spinBox1_13 << ui->spinBox1_14 << ui->spinBox1_15 << ui->spinBox1_16;

    timerData = new QTimer(this);
    timerData->setInterval(1000);
    connect(timerData, SIGNAL(timeout()), this, SLOT(loadData()));
}

void frmTool::initData()
{
    //初始化表格
    QUIHelper::initTableView(ui->tableView, AppData::RowHeight, true, true);

    model = new QSqlTableModel(this);
    model->setTable("NodeData");
    model->setSort(0, Qt::AscendingOrder);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    ui->tableView->setModel(model);
    ui->tableView->setProperty("model", true);

    columnNames.clear();
    columnNames << "位号" << "当前值" << "状态" << "时间";

    columnWidths.clear();
    columnWidths << 160 << 100 << 100 << 220;

    for (int i = 0; i < columnNames.count(); i++) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }
}

void frmTool::loadData()
{
    //随机生成数据
    int count = model->rowCount();
    for (int i = 0; i < count; i++) {
        int value = rand() % 100;
        int status = rand() % 5;
        model->setData(model->index(i, 1), value);
        //model->setData(model->index(i, 2), status);
    }

    on_btnSave_clicked();
}

void frmTool::initConfig()
{
    ui->cboxDeviceType->addItems(DbData::TypeInfo_DeviceType);
    ui->cboxDeviceType->setCurrentIndex(ui->cboxDeviceType->findText(AppConfig::DeviceType));
    connect(ui->cboxDeviceType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    for (int i = 1; i <= 64; i++) {
        ui->cboxDeviceAddr->addItem(QString::number(i));
    }

    ui->cboxDeviceAddr->setCurrentIndex(ui->cboxDeviceAddr->findText(QString::number(AppConfig::DeviceAddr)));
    connect(ui->cboxDeviceAddr, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList comList;
    for (int i = 1; i <= 20; i++) {
        comList << QString("COM%1").arg(i);
    }

    comList << "ttyUSB0" << "ttyS0" << "ttyS1" << "ttyS2" << "ttyS3" << "ttyS4";
    ui->cboxPortName->addItems(comList);
    ui->cboxPortName->setCurrentIndex(ui->cboxPortName->findText(AppConfig::PortName));
    connect(ui->cboxPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList baudList;
    baudList << "1200" << "2400" << "4800" << "9600" << "19200" << "115200";

    ui->cboxBaudRate->addItems(baudList);
    ui->cboxBaudRate->setCurrentIndex(ui->cboxBaudRate->findText(QString::number(AppConfig::BaudRate)));
    connect(ui->cboxBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(AppConfig::ListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    QStringList list = AppConfig::NodeValues.split("|");
    for (int i = 0; i < spinBox.count(); i++) {
        spinBox.at(i)->setValue(list.at(i).toInt());
        connect(spinBox.at(i), SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    }

    ui->ckAutoData->setChecked(AppConfig::AutoData);
    connect(ui->ckAutoData, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->tabWidget->setCurrentIndex(AppConfig::TabIndex);
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(saveConfig()));
}

void frmTool::saveConfig()
{
    AppConfig::DeviceType = ui->cboxDeviceType->currentText();
    AppConfig::DeviceAddr = ui->cboxDeviceAddr->currentText().toInt();
    AppConfig::PortName = ui->cboxPortName->currentText();
    AppConfig::BaudRate = ui->cboxBaudRate->currentText().toInt();
    AppConfig::ListenPort = ui->txtListenPort->text().trimmed().toInt();

    QStringList list;
    for(int i = 0; i < spinBox.count(); i++) {
        list << QString::number(spinBox.at(i)->value());
    }

    AppConfig::NodeValues = list.join("|");
    AppConfig::AutoData = ui->ckAutoData->isChecked();
    AppConfig::TabIndex = ui->tabWidget->currentIndex();
    AppConfig::writeConfig();
}

void frmTool::newConnection()
{
    while (tcpServer->hasPendingConnections()) {
        isTcpOk = true;
        tcpSocket = tcpServer->nextPendingConnection();
        tcpSocket->readAll();
        append(6, QString("客户端上线: %1").arg(tcpSocket->peerAddress().toString()));
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readDataTcp()));
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disConnected()));
    }
}

void frmTool::disConnected()
{
    disconnect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readDataTcp()));
    disconnect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disConnected()));
    tcpSocket->deleteLater();
    tcpSocket = 0;
    isTcpOk = false;
}

void frmTool::readDataTcp()
{
    if (tcpSocket->bytesAvailable() > 0) {
        checkData(tcpSocket->readAll(), false);
    }
}

void frmTool::readDataCom()
{
    if (com->bytesAvailable() > 0) {
        checkData(com->readAll());
    }
}

void frmTool::checkData(const QByteArray &data, bool isCom)
{
    int dataLen = data.length();
    if (dataLen <= 0) {
        return;
    }

    QByteArray body;
    QString type = ui->cboxDeviceType->currentText();
    if (type == "FC-1003-1") {
        for (int i = 0; i < 1; i++) {
            body.append(QUIHelper::ushortToByte(spinBox.at(i)->value()));
        }
    } else if (type == "FC-1003-8") {
        for (int i = 0; i < 8; i++) {
            body.append(QUIHelper::ushortToByte(spinBox.at(i)->value()));
        }
    } else if (type == "FC-1003-16") {
        for (int i = 0; i < 16; i++) {
            body.append(QUIHelper::ushortToByte(spinBox.at(i)->value()));
        }
    } else {
        for (int i = 0; i < 4; i++) {
            body.append(QUIHelper::ushortToByte(spinBox.at(i)->value()));
        }
    }

    //模拟回复数据 01 03 08 00 14 03 12 00 00 00 00 79 E6
    QByteArray buffer;
    buffer.append(ui->cboxDeviceAddr->currentText().toInt());
    buffer.append(0x03);
    buffer.append(body.length());
    buffer.append(body);
    buffer.append(QUIHelper::getCRCCode(buffer));

    if (isCom) {
        com->write(buffer);
        append(1, QUIHelper::byteArrayToHexStr(data));
        append(0, QUIHelper::byteArrayToHexStr(buffer));
    } else {
        tcpSocket->write(buffer);
        append(5, QUIHelper::byteArrayToHexStr(data));
        append(4, QUIHelper::byteArrayToHexStr(buffer));
    }
}

void frmTool::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;

    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

    if (ui->btnShowData->text() == "继续显示") {
        return;
    }

    //过滤回车换行符
    QString strData = data;
    strData.replace("\r", "");
    strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "串口发送 >>";
        ui->txtMain->setTextColor(QColor("dodgerblue"));
    } else if (type == 1) {
        strType = "串口接收 <<";
        ui->txtMain->setTextColor(QColor("red"));
    } else if (type == 2) {
        strType = "处理延时 >>";
        ui->txtMain->setTextColor(QColor("gray"));
    } else if (type == 3) {
        strType = "正在校验 >>";
        ui->txtMain->setTextColor(QColor("green"));
    } else if (type == 4) {
        strType = "网络发送 >>";
        ui->txtMain->setTextColor(QColor(24, 189, 155));
    } else if (type == 5) {
        strType = "网络接收 <<";
        ui->txtMain->setTextColor(QColor(255, 107, 107));
    } else if (type == 6) {
        strType = "提示信息 >>";
        ui->txtMain->setTextColor(QColor(100, 184, 255));
    }

    strData = QString("时间[%1] %2 %3").arg(TIMEMS).arg(strType).arg(strData);
    ui->txtMain->append(strData);
    currentCount++;
}

void frmTool::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开串口") {
        com = new QextSerialPort(ui->cboxPortName->currentText(), QextSerialPort::Polling);
        isComOk = com->open(QIODevice::ReadWrite);
        if (isComOk) {
            com->setBaudRate((BaudRateType)ui->cboxBaudRate->currentText().toInt());
            com->setFlowControl(FLOW_OFF);
            com->setTimeout(10);

            ui->btnOpen->setText("关闭串口");
            timerRead->start();
        }
    } else {
        timerRead->stop();
        com->close();
        com->deleteLater();

        ui->btnOpen->setText("打开串口");
        on_btnClearData_clicked();
        isComOk = false;
    }
}

void frmTool::on_btnListen_clicked()
{
    if (ui->btnListen->text() == "监听服务") {
        int port = ui->txtListenPort->text().toInt();
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
        isTcpOk = tcpServer->listen(QHostAddress::AnyIPv4, port);
#else
        isTcpOk = tcpServer->listen(QHostAddress::Any, port);
#endif

        if (isTcpOk) {
            ui->btnListen->setText("停止服务");
        }
    } else {
        if (tcpSocket != 0) {
            tcpSocket->disconnectFromHost();
        }

        tcpServer->close();
        ui->btnListen->setText("监听服务");
        on_btnClearData_clicked();
        isTcpOk = false;
    }
}

void frmTool::on_btnClearData_clicked()
{
    append(0, "", true);
}

void frmTool::on_btnShowData_clicked()
{
    ui->btnShowData->setText(ui->btnShowData->text() == "暂停显示" ? "继续显示" : "暂停显示");
}

void frmTool::on_btnAdd_clicked()
{
    int count = model->rowCount();
    model->insertRow(count);

    QString positionID = model->index(count - 1, 0).data().toString();
    float nodeValue = model->index(count - 1, 1).data().toFloat();
    int nodeStatus = model->index(count - 1, 2).data().toInt();
    QString saveTime = model->index(count - 1, 3).data().toString();

    //位号末尾数字的话自动递增
    int id = positionID.right(3).toInt();
    if (id > 0) {
        positionID = QString("%1%2").arg(positionID.left(positionID.length() - 3)).arg(id + 1, 3, 10, QChar('0'));
    }

    //当前为第一行时候的默认值
    if (count == 0) {
        positionID = AppData::DefaultPositionID;
        nodeValue = 25;
        nodeStatus = 1;
        saveTime = DATETIME;        
    }

    //设置新增加的行默认值
    model->setData(model->index(count, 0), positionID);
    model->setData(model->index(count, 1), nodeValue);
    model->setData(model->index(count, 2), nodeStatus);
    model->setData(model->index(count, 3), saveTime);

    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmTool::on_btnSave_clicked()
{
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
    } else {
        model->database().rollback();
        QUIHelper::showMessageBoxError("保存信息失败,信息不能为空,请重新填写!");
    }
}

void frmTool::on_btnDelete_clicked()
{
    if (ui->tableView->currentIndex().row() < 0) {
        QUIHelper::showMessageBoxError("请选择要删除的记录!");
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要删除该记录吗?") == QMessageBox::Yes) {
        int row = ui->tableView->currentIndex().row();
        model->removeRow(row);
        model->submitAll();

        int count = model->rowCount();
        ui->tableView->setCurrentIndex(model->index(count - 1, 0));
    }
}

void frmTool::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmTool::on_btnClear_clicked()
{
    int count = model->rowCount();
    if (count <= 0) {
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要清空所有记录吗?") == QMessageBox::Yes) {
        DbQuery::clearNodeData();
        model->select();
    }
}

void frmTool::on_ckAutoData_stateChanged(int arg1)
{
    if (arg1 == 0) {
        timerData->stop();
    } else {
        timerData->start();
    }
}
