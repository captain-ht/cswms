#include "frmdevicecontrol.h"
#include "ui_frmdevicecontrol.h"
#include "quihelper.h"
#include "deviceserver.h"

frmDeviceControl *frmDeviceControl::self = NULL;
frmDeviceControl *frmDeviceControl::Instance()
{
    if (!self) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new frmDeviceControl;
        }
    }

    return self;
}

frmDeviceControl::frmDeviceControl(QWidget *parent) : QWidget(parent), ui(new Ui::frmDeviceControl)
{
    ui->setupUi(this);
    this->initForm();
    QUIHelper::setFormInCenter(this);
}

frmDeviceControl::~frmDeviceControl()
{
    delete ui;
}

void frmDeviceControl::initForm()
{
    //置顶显示
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    this->setWindowTitle("变量信息");
    this->setStyleSheet("*{font:30px;}");
}

void frmDeviceControl::setPositionID(const QString &positionID)
{
    //通过位号查找其他信息
    int index = DbData::NodeInfo_PositionID.indexOf(positionID);
    quint8 nodeID = DbData::NodeInfo_NodeID.at(index);
    QString deviceName = DbData::NodeInfo_DeviceName.at(index);
    QString nodeName = DbData::NodeInfo_NodeName.at(index);
    QString nodeType = DbData::NodeInfo_NodeType.at(index);
    QString nodeClass = DbData::NodeInfo_NodeClass.at(index);
    QString nodeSign = DbData::NodeInfo_NodeSign.at(index);

    ui->labNodeID->setText(QString("编   号: %1").arg(nodeID));
    ui->labPositionID->setText(QString("位   号: %1").arg(positionID));
    ui->labDeviceName->setText(QString("控制器: %1").arg(deviceName));
    ui->labNodeName->setText(QString("变量: %1").arg(nodeName));
    ui->labNodeType->setText(QString("类   型: %1").arg(nodeType));
    ui->labNodeSign->setText(QString("气   种: %1 %2").arg(nodeClass).arg(nodeSign));

#if 0
    index = DbData::DeviceInfo_DeviceName.indexOf(deviceName);
    if (index >= 0) {
        QString portName = DbData::DeviceInfo_PortName.at(index);
        quint8 addr = DbData::DeviceInfo_DeviceAddr.at(index);
        DeviceServer::Instance()->readValueAD4(portName, addr);
        DeviceServer::Instance()->readAlarm(portName, addr);
        DeviceServer::Instance()->readDate(portName, addr);
        DeviceServer::Instance()->readDateInit(portName, addr);
    }
#endif
}
