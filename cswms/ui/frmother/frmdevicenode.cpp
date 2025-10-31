#include "frmdevicenode.h"
#include "ui_frmdevicenode.h"
#include "quihelper.h"
#include "dbquery.h"
#include "deviceserver.h"
#include "frmdevicecontrol.h"

frmDeviceNode::frmDeviceNode(QWidget *parent) : QWidget(parent), ui(new Ui::frmDeviceNode)
{
    ui->setupUi(this);
    this->initStyle();
    this->initForm();
}

frmDeviceNode::~frmDeviceNode()
{
    delete ui;
}

bool frmDeviceNode::eventFilter(QObject *watched, QEvent *event)
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

QString frmDeviceNode::getPositionID()
{
    return this->positionID;
}

QString frmDeviceNode::getDeviceName()
{
    return this->deviceName;
}

QString frmDeviceNode::getNodeName()
{
    return this->nodeName;
}

void frmDeviceNode::initStyle()
{
    //取消掉重合的边框使得看起来就一条边框
    qss = QString("QFrame#gboxDeviceTitle{border-bottom-left-radius:0px;border-bottom-right-radius:0px;}"
                  "QFrame#gboxDevicePanel{border-top-width:0px;border-top-left-radius:0px;border-top-right-radius:0px;}"
                  "QLabel:disabled{background:none;color:%1;}").arg(QUIConfig::BorderColor);
    this->setStyleSheet(qss);

    QUIStyle::setLabStyle(ui->labNodeName, 1);
    QUIStyle::setLabStyle(ui->labValue, 2);
    QUIStyle::setLabStyle(ui->labNodeSign, 3);
}

void frmDeviceNode::initForm()
{
    QFont font;
    font.setPixelSize(QUIConfig::FontSize + 5);
    ui->labNodeName->setFont(font);
    ui->labValue->setFont(font);

    font.setPixelSize(QUIConfig::FontSize + 1);
    ui->labNodeSign->setFont(font);

    online = false;
    alarm = false;
    this->installEventFilter(this);
}

void frmDeviceNode::setPositionID(const QString &positionID)
{
    this->positionID = positionID;
    ui->labPositionID->setText("位号: " + positionID);
}

void frmDeviceNode::setDeviceName(const QString &deviceName)
{
    this->deviceName = deviceName;
}

void frmDeviceNode::setNodeName(const QString &nodeName)
{
    this->nodeName = nodeName;
    ui->labNodeName->setText(nodeName);
}

void frmDeviceNode::setNodeType(const QString &nodeType)
{
    this->nodeType = nodeType;
    ui->labNodeType->setText("型号: " + nodeType);
}

void frmDeviceNode::setNodeSign(const QString &nodeSign)
{
    this->nodeSign = nodeSign;
    ui->labNodeSign->setText(nodeSign);
}

void frmDeviceNode::setValue(float value)
{
    this->value = value;

    //有两个传感器 是开关量 数值是 1、2  显示时 用正常 和异常代替
    if (nodeType == "SJ-0001" || nodeType == "JG-0001") {
        ui->labValue->setText(value == 1 ? "正常" : "异常");
    } else {
        ui->labValue->setText(QString::number(value));
    }
}

void frmDeviceNode::setOnline(bool online)
{
    this->online = online;
    this->setEnabled(online);
    ui->labNodeName->setEnabled(online);
}

void frmDeviceNode::setAlarm(bool alarm)
{
    this->alarm = alarm;
    QUIStyle::setLabStyle(ui->labValue, alarm ? 0 : 2);
}

void frmDeviceNode::setSelect(bool select)
{
    //选中特殊显示,比如边框加粗
    QString qss;
    if (select) {
        qss = QString("#gboxDeviceTitle,#gboxDevicePanel{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}"
                      "#labNodeType,#labPositionID{color:%3;}").arg("#1DB7BF").arg("#1DB7BF").arg("#FFFFFF");
    } else {
        qss = QString("#gboxDeviceTitle,#gboxDevicePanel{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}"
                      "#labNodeType,#labPositionID{color:%3;}").arg(QUIConfig::NormalColorStart).arg(QUIConfig::NormalColorEnd).arg(QUIConfig::TextColor);
    }

    this->setStyleSheet(this->qss + qss);
}
