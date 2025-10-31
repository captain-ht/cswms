#include "devicedata.h"
#include "quihelper.h"
#include "dbquery.h"

QByteArray DeviceData::getSendData1(const QString &portName, quint8 addr)
{
    //在地址为1的控制器上查询设备1到设备68的数据   01 03 00 00 00 44 45 F9
    //在地址为1的控制器上查询设备1到设备4的数据    01 03 00 00 00 04 44 09
    //在地址为1的控制器上查询设备1到设备1的数据    01 03 00 00 00 01 84 0A

    //找到当前索引位置的设备地址对应有多少个变量
    quint16 nodeNumber = DbQuery::getNodeNumber(portName, addr);
    //找到当前索引位置的设备地址对应变量的最小寄存器地址
    quint16 nodeMinAddr = DbQuery::getNodeMinAddr(portName, addr);

    QByteArray body;
    body.append(0x03);
    body.append(QUIHelper::ushortToByte(nodeMinAddr));
    body.append(QUIHelper::ushortToByte(nodeNumber));
    return body;
}

QByteArray DeviceData::getSendData2(const QString &portName, quint8 addr)
{
    //找到当前索引位置的设备地址对应有多少个变量
    quint16 nodeNumber = DbQuery::getNodeNumber(portName, addr);
    //每个变量4个寄存器+控制器状态2寄存器 每个寄存器2字节
    nodeNumber = (nodeNumber * 4) + 2;
    //起始地址寄存器固定为0
    quint16 nodeMinAddr = 0;

    QByteArray body;
    body.append(0x03);
    body.append(QUIHelper::ushortToByte(nodeMinAddr));
    body.append(QUIHelper::ushortToByte(nodeNumber));
    return body;
}
