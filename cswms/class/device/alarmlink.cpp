#include "alarmlink.h"
#include "quihelper.h"
#include "dbquery.h"
#include "deviceserver.h"

#define zero (char)0x00

QScopedPointer<AlarmLink> AlarmLink::self;
AlarmLink *AlarmLink::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new AlarmLink);
        }
    }

    return self.data();
}

AlarmLink::AlarmLink(QObject *parent) : QObject(parent)
{
    timerAlarm = new QTimer(this);
    connect(timerAlarm, SIGNAL(timeout()), this, SLOT(checkAlarm()));
    timerAlarm->setInterval(1000);
}

void AlarmLink::readData()
{
    QextSerialPort *com = (QextSerialPort *)sender();
    if (com->bytesAvailable() <= 0) {
        return;
    }

    QUIHelper::sleep(10);
    QByteArray buffer = com->readAll();
    QString portName = "报警联动";

    //至少要多少个字节,保证下面取数据不出错
    int size = buffer.size();
    if (size < 5) {
        return;
    }

    //01 03 08 00 00 00 00 00 00 00 00 95 D7
    //01 03 08 00 14 03 12 00 00 00 00 79 E6

    //取出首字节,判断是否为当前地址集合中的地址
    quint8 addr = buffer.at(0);
    quint8 cmd = buffer.at(1);
    quint8 len = buffer.at(2);

    //如果是错误码则直接解析错误信息
    QList<quint8> cmds;
    cmds << 0x03 << 0x04 << 0x06;
    if (!cmds.contains(cmd)) {
        emit receiveError(portName, addr, QString("数据出错: %1").arg(QUIHelper::byteArrayToHexStr(buffer)));
        buffer.clear();
        return;
    }

    //后面的数据长度必须大于等于长度数据位表示的长度
    if ((cmd == 0x03 || cmd == 0x04 || cmd == 0x06) && size < len + 5) {
        return;
    }

    //放在这里发出去数据是准确的完整的
    emit receiveData(portName, addr, buffer);
}

void AlarmLink::init()
{
    //找出报警联动信息中不属于通信端口中的串口,这里打开
    for (int i = 0; i < DbData::AlarmLink_Count; i++) {
        QString comName = DbData::AlarmLink_ComName.at(i);
        if (!DbData::PortInfo_ComName.contains(comName) && !comNames.contains(comName)) {
            int baudRate = DbData::AlarmLink_BaudRate.at(i);
            QextSerialPort *com = new QextSerialPort(QextSerialPort::EventDriven, this);
            connect(com, SIGNAL(readyRead()), this, SLOT(readData()));
            com->setPortName(comName);
            com->setBaudRate((BaudRateType)baudRate);
            if (com->open(QextSerialPort::ReadWrite)) {
                comNames << comName;
                coms << com;
                DbQuery::addUserLog("设备上报", QString("联动串口%1打开成功").arg(comName));
            } else {
                DbQuery::addUserLog("设备上报", QString("联动串口%1打开失败").arg(comName));
                QUIHelper::showMessageBoxError(QString("联动串口%1打开失败,请确认是否存在!").arg(comName), 3);
            }
        }
    }

    reset();
}

void AlarmLink::start()
{
    timerAlarm->start();
    DbQuery::addUserLog("设备上报", "联动服务启动");
}

void AlarmLink::stop()
{
    timerAlarm->stop();
    DbQuery::addUserLog("设备上报", "联动服务停止");
}

void AlarmLink::checkAlarm()
{
    if (alarmPositionIDs.count() == 0) {
        return;
    }

    //找出位号对应的报警联动的信息,会有多个联动信息,挨个发送
    QMutexLocker lock(&mutex);
    QString positionID = alarmPositionIDs.takeFirst();
    checkAlarm(positionID, true);
}

void AlarmLink::checkAlarm(const QString &positionID, bool alarm)
{
    for (int i = 0; i < DbData::AlarmLink_Count; i++) {
        if (positionID == DbData::AlarmLink_PositionID.at(i)) {
            QString comName = DbData::AlarmLink_ComName.at(i);
            quint8 modelAddr = DbData::AlarmLink_ModelAddr.at(i);
            QString linkAddr = DbData::AlarmLink_LinkAddr.at(i);

            //生成要联动的数据
            QByteArray body;
            body.append(0x06);
            body.append(zero);
            body.append(zero);

            //一次性发送所有报警继电器联动,比如设置的linkAddr为1,2,3
            //具体按照位计算,将对应占位符填充,共两个字节
            //从对应模块地址中取出原来的再替换并更新
            QString strBin = "0000000000000000";
            QString temp = QString("%1|%2").arg(comName).arg(modelAddr);
            int index = -1;
            for (int i = 0; i < alarmIDs.count(); i++) {
                if (alarmIDs.at(i).startsWith(temp)) {
                    index = i;
                    break;
                }
            }

            //重新更新报警位,并保留以前的报警位
            if (index >= 0) {
                strBin = alarmIDs.at(index).split("|").last();
                QStringList linkAddrs = linkAddr.split("|");
                foreach (QString addr, linkAddrs) {
                    if (!addr.isEmpty()) {
                        strBin[16 - addr.toInt()] = alarm ? '1' : '0';
                    }
                }

                alarmIDs[index] = QString("%1|%2|%3").arg(comName).arg(modelAddr).arg(strBin);
            }

            quint16 addr = QUIHelper::strBinToDecimal(strBin);
            body.append(QUIHelper::ushortToByte(addr));
            QString type = QString("串口[%1] %2报警联动 %3").arg(comName).arg(alarm ? "启动" : "停止").arg(linkAddr);
            writeData(comName, type, modelAddr, body);
        }
    }
}

void AlarmLink::append(const QString &positionID)
{
    //限制最大的报警队列,已经存在的则不用继续加入
    if (alarmPositionIDs.count() > 50 || alarmPositionIDs.contains(positionID)) {
        return;
    }

    QMutexLocker lock(&mutex);
    alarmPositionIDs << positionID;
}

void AlarmLink::remove(const QString &positionID)
{
    //需要记住以前报警的,重新替换新的
    //这个还有待商议,是将该模块全部消音还是单个,单个有问题,当两个都发送到一个报警位时会不一致
    //2019-3-25 改为不需要复位报警继电器,用户通过单击消音按钮触发复位所有继电器
    //checkAlarm(positionID, false);
}

void AlarmLink::reset()
{
    //对所有的位号对应的串口发送复位数据
    QByteArray body;
    body.append(0x06);
    body.append(zero);
    body.append(zero);
    body.append(zero);
    body.append(zero);

    QStringList comNames;
    for (int i = 0; i < DbData::AlarmLink_Count; i++) {
        QString comName = DbData::AlarmLink_ComName.at(i);
        //过滤已经发送过复位的,不然大量重复发送
        if (!comNames.contains(comName)) {
            quint8 modelAddr = DbData::AlarmLink_ModelAddr.at(i);
            QString type = QString("串口[%1] %2报警联动 %3").arg(comName).arg("复位").arg(0);
            writeData(comName, type, modelAddr, body);
            comNames << comName;
            break;
        }
    }

    //构建空的已报警位队列
    alarmIDs.clear();
    alarmPositionIDs.clear();
    for (int i = 0; i < DbData::AlarmLink_Count; i++) {
        QString comName = DbData::AlarmLink_ComName.at(i);
        quint8 modelAddr = DbData::AlarmLink_ModelAddr.at(i);
        QString alarmID = QString("%1|%2|0000000000000000").arg(comName).arg(modelAddr);

        //如果已经存在该串口号该模块则不用继续
        bool exist = false;
        QString temp = QString("%1|%2").arg(comName).arg(modelAddr);
        for (int i = 0; i < alarmIDs.count(); i++) {
            if (alarmIDs.at(i).startsWith(temp)) {
                exist = true;
                break;
            }
        }

        if (!exist) {
            alarmIDs << alarmID;
        }
    }
}

void AlarmLink::writeData(int index, const QString &type, quint8 addr, const QByteArray &body)
{
    //先判断串口是否运行正常
    QByteArray data;
    data.append(addr);
    data.append(body);
    data.append(QUIHelper::getCRCCode(data));

    //需要判断数量否则越界
    if (coms.count() > index) {
        coms.at(index)->write(data);
    }

    QString portName = "报警联动";
    emit sendData(portName, addr, data);
    emit receiveInfo(portName, addr, type);
}

void AlarmLink::writeData(const QString &comName, const QString &type, quint8 modelAddr, const QByteArray &body)
{
    //如果串口不属于独立的串口则从控制器所用串口中发送数据
    int index = comNames.indexOf(comName);
    if (index >= 0) {
        writeData(index, type, modelAddr, body);
    } else {
        index = DbData::PortInfo_ComName.indexOf(comName);
        if (index >= 0) {
            QString portName = DbData::PortInfo_PortName.at(index);
            DeviceServer::Instance()->writeData(portName, type, modelAddr, body);
        }
    }
}
