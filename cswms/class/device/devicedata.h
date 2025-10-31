#ifndef DEVICEDATA_H
#define DEVICEDATA_H

#include "head.h"

class DeviceData
{
public:
    //从设备采集
    static QByteArray getSendData1(const QString &portName, quint8 addr);
    //从设备采集2
    static QByteArray getSendData2(const QString &portName, quint8 addr);
};

#endif // DEVICEDATA_H
