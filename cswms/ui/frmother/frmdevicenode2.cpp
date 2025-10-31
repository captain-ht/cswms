#include "frmdevicenode2.h"
#include "ui_frmdevicenode2.h"
#include "quihelper.h"
#include "dbquery.h"
#include "deviceserver.h"
#include "frmdevicecontrol.h"

frmDeviceNode2::frmDeviceNode2(QWidget *parent) : QWidget(parent), ui(new Ui::frmDeviceNode2)
{
    ui->setupUi(this);
    this->initStyle();
    this->initForm();
}

frmDeviceNode2::~frmDeviceNode2()
{
    delete ui;
}

bool frmDeviceNode2::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (!online) {
            //双击重连当前端口下的所有设备
            QString portName = DbQuery::getPortName(deviceName);
            DeviceServer::Instance()->readValue(portName, 255, true);
        } else {
            if (AppData::CurrentUserType.contains("管理员")) {
                frmDeviceControl::Instance()->setPositionID(positionID);
                frmDeviceControl::Instance()->show();
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

QString frmDeviceNode2::getPositionID()
{
    return this->positionID;
}

QString frmDeviceNode2::getDeviceName()
{
    return this->deviceName;
}

QString frmDeviceNode2::getNodeName()
{
    return this->nodeName;
}

void frmDeviceNode2::initStyle()
{
    qss = QString("QLabel:disabled{background:none;color:%1;}").arg(QUIConfig::BorderColor);
    this->setStyleSheet(qss);
}

void frmDeviceNode2::initForm()
{
    online = false;
    alarm = false;
    this->installEventFilter(this);

    QFont font;
    font.setPixelSize(QUIConfig::FontSize + 2);
    ui->labNodeInfo->setFont(font);
    //QUIHelper::setLabStyle(ui->labName, 2);
}

void frmDeviceNode2::setPositionID(const QString &positionID)
{
    this->positionID = positionID;
    //ui->labPositionID->setText("位号: " + positionID);
    ui->labNodeInfo->setText(positionID + " -- " + nodeName);
}

void frmDeviceNode2::setDeviceName(const QString &deviceName)
{
    this->deviceName = deviceName;
}

void frmDeviceNode2::setNodeName(const QString &nodeName)
{
    this->nodeName = nodeName;
    //ui->labNodeName->setText("设备: " + nodeName);
    ui->labNodeInfo->setText(positionID + " -- " + nodeName);
}

void frmDeviceNode2::setNodeType(const QString &nodeType)
{
    this->nodeType = nodeType;
    //ui->labNodeType->setText("型号: " + nodeType);
}

void frmDeviceNode2::setNodeSign(const QString &nodeSign)
{
    this->nodeSign = nodeSign;
    //ui->labNodeSign->setText(nodeSign);
    ui->gaugeSpeed->setUnit("");
    ui->gaugeSpeed->setText(nodeSign);
}

void frmDeviceNode2::setValue(float value)
{
    this->value = value;

    //有两个传感器 是开关量 数值是 1、2  显示时 用正常 和异常代替
    if (nodeType == "SJ-0001" || nodeType == "JG-0001") {
        ui->gaugeSpeed->setValue(value == 1 ? ui->gaugeSpeed->getMinValue() : ui->gaugeSpeed->getMaxValue());
    } else {
        ui->gaugeSpeed->setValue(value);
    }
}

void frmDeviceNode2::setOnline(bool online)
{
    this->online = online;
    this->setEnabled(online);
    //ui->labNodeName->setEnabled(online);
}

void frmDeviceNode2::setAlarm(bool alarm)
{
    this->alarm = alarm;
    //QUIHelper::setLabStyle(ui->labValue, alarm ? 0 : 2);
}

void frmDeviceNode2::setSelect(bool select)
{
    //选中特殊显示,比如边框加粗
    QString qss;
    if (select) {
        qss = QString("#gboxDeviceTitle,#gboxDevicePanel{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}"
                      "#labNodeType,#labPositionID,#labNodeName,#labNodeInfo{color:%3;}GaugeSpeed{qproperty-textColor:%3;}")
                .arg("#1DB7BF").arg("#1DB7BF").arg("#FFFFFF");
    } else {
        qss = QString("#gboxDeviceTitle,#gboxDevicePanel{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}"
                      "#labNodeType,#labPositionID,#labNodeName,#labNodeInfo{color:%3;}GaugeSpeed{qproperty-textColor:%3;}")
                .arg(QUIConfig::NormalColorStart).arg(QUIConfig::NormalColorEnd).arg(QUIConfig::TextColor);
    }

    this->setStyleSheet(this->qss + qss);
}
