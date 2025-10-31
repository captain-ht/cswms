#include "udpsend.h"
#include "quihelper.h"
#include "dbquery.h"

QScopedPointer<UdpSend> UdpSend::self;
UdpSend *UdpSend::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new UdpSend);
        }
    }

    return self.data();
}

UdpSend::UdpSend(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
}

UdpSend::~UdpSend()
{
}

void UdpSend::sendData(const QString &portName, quint8 addr, const QByteArray &data)
{
    if (!AppConfig::UseNetSend) {
        return;
    }

    //取出ip和端口
    QList<QString> ips;
    QList<int> ports;
    QStringList list = AppConfig::NetSendInfo.split(";");
    foreach (QString str, list) {
        if (str.contains(":")) {
            QStringList temp = str.split(":");
            ips << temp.at(0);
            ports << temp.at(1).toInt();
        }
    }

    //逐个发送
    for (int i = 0; i < ips.count(); i++) {
        if (data.size() > 0) {
            QString buffer;
            if (portName == "PortInfo" || portName == "DeviceInfo" || portName == "NodeInfo") {
                buffer = QString("%1|%2").arg(portName).arg(QString(data));
            } else {
                buffer = QString("%1|%2").arg(portName).arg(QUIHelper::byteArrayToHexStr(data));
            }

            udpSocket->writeDatagram(buffer.toUtf8(), QHostAddress(ips.at(i)), ports.at(i));
            udpSocket->flush();
        }
    }
}

void UdpSend::start()
{
    DbQuery::addUserLog("设备上报", "网络转发启动服务");
}

void UdpSend::stop()
{
    DbQuery::addUserLog("设备上报", "网络转发停止服务");
}

void UdpSend::sendInfo()
{
    if (!AppConfig::UseNetSend) {
        return;
    }

    //发送端口信息表+设备信息表+变量信息表
    QStringList portInfo, deviceInfo, nodeInfo;

    for (int i = 0; i < DbData::PortInfo_Count; i++) {
        QStringList list;
        list << QString::number(DbData::PortInfo_PortID.at(i));
        list << DbData::PortInfo_PortName.at(i);
        list << DbData::PortInfo_PortType.at(i);
        list << DbData::PortInfo_ComName.at(i);
        list << QString::number(DbData::PortInfo_BaudRate.at(i));
        list << DbData::PortInfo_TcpIP.at(i);
        list << QString::number(DbData::PortInfo_TcpPort.at(i));
        list << QString::number(DbData::PortInfo_ReadInterval.at(i));
        list << QString::number(DbData::PortInfo_ReadTimeout.at(i));
        list << QString::number( DbData::PortInfo_ReadMaxtime.at(i));
        portInfo << list.join(";");
    }

    for (int i = 0; i < DbData::DeviceInfo_Count; i++) {
        QStringList list;
        list << QString::number(DbData::DeviceInfo_DeviceID.at(i));
        list << DbData::DeviceInfo_PortName.at(i);
        list << DbData::DeviceInfo_DeviceName.at(i);
        list << QString::number(DbData::DeviceInfo_DeviceAddr.at(i));
        list << DbData::DeviceInfo_DeviceType.at(i);
        list << QString::number(DbData::DeviceInfo_NodeNumber.at(i));
        deviceInfo << list.join(";");
    }

    for (int i = 0; i < DbData::NodeInfo_Count; i++) {
        QStringList list;
        list << QString::number(DbData::NodeInfo_NodeID.at(i));
        list << DbData::NodeInfo_PositionID.at(i);
        list << DbData::NodeInfo_DeviceName.at(i);
        list << DbData::NodeInfo_NodeName.at(i);
        list << QString::number(DbData::NodeInfo_NodeAddr.at(i));
        list << DbData::NodeInfo_NodeType.at(i);
        list << DbData::NodeInfo_NodeClass.at(i);
        list << DbData::NodeInfo_NodeSign.at(i);
        list << QString::number(DbData::NodeInfo_NodeUpper.at(i));
        list << QString::number(DbData::NodeInfo_NodeLimit.at(i));
        list << QString::number(DbData::NodeInfo_NodeMax.at(i));
        list << QString::number(DbData::NodeInfo_NodeMin.at(i));
        list << QString::number(DbData::NodeInfo_NodeRange.at(i));
        list << DbData::NodeInfo_NodeEnable.at(i);
        list << DbData::NodeInfo_NodeSound.at(i);
        list << DbData::NodeInfo_NodeImage.at(i);
        list << QString::number(DbData::NodeInfo_SaveInterval.at(i));
        list << QString::number(DbData::NodeInfo_DotCount.at(i));
        list << QString::number(DbData::NodeInfo_AlarmDelay.at(i));
        list << DbData::NodeInfo_AlarmType.at(i);
        list << QString::number(DbData::NodeInfo_NodeX.at(i));
        list << QString::number(DbData::NodeInfo_NodeY.at(i));
        nodeInfo << list.join(";");
    }

    sendData("PortInfo", 255, QString(portInfo.join(" ")).toUtf8());
    sendData("DeviceInfo", 255, QString(deviceInfo.join(" ")).toUtf8());
    sendData("NodeInfo", 255, QString(nodeInfo.join(" ")).toUtf8());
}
