#include "deviceclient.h"
#include "quihelper.h"
#include "devicedata.h"

QScopedPointer<DeviceClient> DeviceClient::self;
DeviceClient *DeviceClient::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new DeviceClient);
        }
    }

    return self.data();
}

DeviceClient::DeviceClient(QObject *parent) : QObject(parent)
{
    currentIndex = 0;

    portType = "Modbus_Com";
    portName = "默认端口";
    comName = "COM1";
    baudRate = 9600;
    tcpIP = "127.0.0.1";
    tcpPort = 502;
    readInterval = 1000;
    readTimeout = 3;
    readMaxtime = 60 * 1000;
    addrs.clear();

    isOk = false;
    pause = false;
    lastTime = QDateTime::currentDateTime();

    //定时器处理重连
    timerConn = new QTimer(this);
    connect(timerConn, SIGNAL(timeout()), this, SLOT(checkConn()));
    timerConn->setInterval(5000);

    //定时器处理设备离线
    timerOnline = new QTimer(this);
    connect(timerOnline, SIGNAL(timeout()), this, SLOT(checkOnline()));
    timerOnline->setInterval(1000);

    //定时器逐个地址读取值
    timerRead = new QTimer(this);
    connect(timerRead, SIGNAL(timeout()), this, SLOT(readValue()));
    timerRead->setInterval(readInterval);

    //定时器处理读取到的数据
    timerCheck = new QTimer(this);
    connect(timerCheck, SIGNAL(timeout()), this, SLOT(checkValue()));
    timerCheck->setInterval(100);

    //定时器重新轮询所有设备
    timerReadAll = new QTimer(this);
    connect(timerReadAll, SIGNAL(timeout()), this, SLOT(readValueAll()));
    timerReadAll->setInterval(readMaxtime);
}

QString DeviceClient::getPortName() const
{
    return this->portName;
}

QList<quint8> DeviceClient::getAddrs() const
{
    return this->addrs;
}

void DeviceClient::connected()
{
    //如果从不正常转为正常则立即全部重新读取一次
    if (!isOk) {
        readValueAll();
    }

    isOk = true;
    emit receiveInfo(portName, 255, "串口打开成功");
}

void DeviceClient::disconnected()
{
    isOk = false;
    emit receiveInfo(portName, 255, "串口打开失败");
}

void DeviceClient::checkConn()
{
    //连接不正常则重新连接
    if (!isOk) {
        buffer.clear();
        if (portType == "Modbus_Com") {
            com->setPortName(comName);
            com->setBaudRate((BaudRateType)baudRate);
            if (com->open(QextSerialPort::ReadWrite)) {
                connected();
            } else {
                disconnected();
            }
        } else if (portType == "Modbus_Tcp_Rtu") {
            tcpSocket->abort();
            tcpSocket->connectToHost(tcpIP, tcpPort);
            emit receiveInfo(portName, 255, "尝试连接设备");
        }
    }
}

void DeviceClient::checkOnline()
{
    //计算超时时间=采集间隔*超时次数*在线设备的个数,必须放在这里动态计算,因为设备上线下线会变
    //为了防止计算失误,在线设备数量+1
    int onlineCount = 1;
    foreach (bool online, onlines) {
        if (online) {
            onlineCount++;
        }
    }

    //单位是毫秒
    int timeout = readInterval * readTimeout * onlineCount;
    QDateTime now = QDateTime::currentDateTime();
    int count = addrs.count();
    for (int i = 0; i < count; i++) {
        //只有处于在线状态的设备才需要处理
        if (onlines.at(i)) {
            quint64 sec = times.at(i).msecsTo(now);
            if (sec > timeout) {
                quint8 addr = addrs.at(i);
                onlines[i] = false;
                emit receiveOnline(portName, addr, false);
                emit receiveInfo(portName, addr, "设备离线");
            }
        }
    }
}

void DeviceClient::readData()
{
    //更新最后的消息时间,将数据存入队列,让定时器去处理数据
    lastTime = QDateTime::currentDateTime();
    if (portType == "Modbus_Com") {
        if (com->bytesAvailable() > 0) {
            buffer.append(com->readAll());
        }
    } else if (portType == "Modbus_Tcp_Rtu") {
        if (tcpSocket->bytesAvailable() > 0) {
            buffer.append(tcpSocket->readAll());
        }
    }
}

void DeviceClient::readValue()
{
    //还没有连接上或者地址为空则不需要处理
    int addrCount = addrs.count();
    if (!isOk || addrCount == 0) {
        return;
    }

    //优先执行命令
    if (doAddrs.count() > 0) {
        QString type = doTypes.takeFirst();
        quint8 addr = doAddrs.takeFirst();
        QByteArray body = doBodys.takeFirst();
        writeData(type, addr, body);

        //额外执行的命令可能比较多导致设备离线,这里需要更新所有设备的时间
        for (int i = 0; i < addrs.count(); i++) {
            times[i] = QDateTime::currentDateTime();
        }

        return;
    }

    //暂停不需要轮询,更新最后的消息时间,不然时间久了会判断离线
    if (pause) {
        for (int i = 0; i < addrs.count(); i++) {
            times[i] = QDateTime::currentDateTime();
        }
        return;
    }

    //如果没有一个在线的设备则不需要处理
    bool existLive = false;
    for (int i = 0; i < addrCount; i++) {
        if (onlines.at(i)) {
            existLive = true;
            break;
        }
    }

    if (!existLive) {
        emit receiveInfo(portName, 255, "没有一个在线设备,跳过遍历");
        return;
    }

    //如果已经到了末尾则重新开始
    if (currentIndex == addrCount) {
        currentIndex = 0;
    }

    //跳过离线的设备,加速读取,递归
    if (!onlines.at(currentIndex)) {
        currentIndex++;
        readValue();
        return;
    }

    readValue(addrs.at(currentIndex));
    currentIndex++;
}

void DeviceClient::checkValue()
{
    if (!isOk) {
        return;
    }

    QMutexLocker locker(&mutex);
    readData();

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

    //如果数据过长则丢弃当前数据包,不然一旦产生了错误的数据会一直累积
    if (size > 517) {
        emit receiveError(portName, addr, QString("数据出错: %1").arg(QUIHelper::byteArrayToHexStr(buffer)));
        buffer.clear();
        return;
    }

    //后面的数据长度必须大于等于长度数据位表示的长度
    if ((cmd == 0x03 || cmd == 0x04 || cmd == 0x06) && size < len + 5) {
        emit receiveError(portName, addr, QString("数据不全,等待完整数据再解析: %1").arg(QUIHelper::byteArrayToHexStr(buffer)));
        return;
    }

    //放在这里发出去数据是准确的完整的
    emit receiveData(portName, addr, buffer);

    //过滤不存在的地址,防止索引越界
    int index = addrs.indexOf(addr);
    if (index < 0) {
        emit receiveError(portName, addr, "地址出错: 当前地址不在设定的地址集合中");
        buffer.clear();
        return;
    }

    //来过消息的设备,立马更新最后的消息时间,以及判断设备上线
    times[index] = QDateTime::currentDateTime();
    if (!onlines.at(index)) {
        onlines[index] = true;
        emit receiveOnline(portName, addr, true);
        emit receiveInfo(portName, addr, "设备上线");
    }

    //根据不同的cmd+不同的命令类型 取出对应的数据内容
    if (cmd == 0x03) {
        QString info;
        if (currentType == "查询数值") {
            QList<quint16> values;
            //每个变量状态1个寄存器=2字节
            for (int i = 3; i < size - 2; i = i + 2) {
                values << (float)QUIHelper::byteToUShort(buffer.mid(i, 2));
            }

            QStringList list;
            foreach (quint16 value, values) {
                list << QString::number(value);
            }

            info = QString("%1返回: %2").arg(currentType).arg(list.join(" "));
            emit receiveValue(portName, addr, values);
        }

        //发送对应的文字解析
        if (!info.isEmpty()) {
            emit receiveInfo(portName, addr, info);
        }
    } else if (cmd == 0x04) {

    } else if (cmd == 0x06) {

    }

    //重新赋值
    buffer.clear();
}

void DeviceClient::setPortType(const QString &portType)
{
    this->portType = portType;
}

void DeviceClient::setPortName(const QString &portName)
{
    this->portName = portName;
}

void DeviceClient::setComName(const QString &comName)
{
    this->comName = comName;
}

void DeviceClient::setBaudRate(int baudRate)
{
    this->baudRate = baudRate;
}

void DeviceClient::setTcpIP(const QString &tcpIP)
{
    this->tcpIP = tcpIP;
}

void DeviceClient::setTcpPort(int tcpPort)
{
    this->tcpPort = tcpPort;
}

void DeviceClient::setReadInterval(int readInterval)
{
    this->readInterval = readInterval;
    timerRead->setInterval(readInterval);
}

void DeviceClient::setReadTimeout(int readTimeout)
{
    this->readTimeout = readTimeout;
}

void DeviceClient::setReadMaxtime(int readMaxtime)
{
    this->readMaxtime = readMaxtime;
    timerReadAll->setInterval(readMaxtime);
}

void DeviceClient::setAddrs(const QList<quint8> &addrs)
{
    this->addrs = addrs;

    //重新设置设备对应的最后消息时间+是否在线
    times.clear();
    onlines.clear();

    int count = addrs.count();
    for (int i = 0; i < count; i++) {
        times << QDateTime::currentDateTime();
        onlines << false;
    }
}

void DeviceClient::init()
{
    if (portType == "Modbus_Com") {
        com = new QextSerialPort(QextSerialPort::Polling, this);
        //connect(com, SIGNAL(readyRead()), this, SLOT(readData()));
    } else if (portType == "Modbus_Tcp_Rtu") {
        tcpSocket = new QTcpSocket(this);
        connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(disconnected()));
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
    }
}

void DeviceClient::start()
{
    checkConn();
    timerRead->start();
    timerCheck->start();
    timerOnline->start();
    timerConn->start();
    timerReadAll->start();
}

void DeviceClient::stop()
{
    timerRead->stop();
    timerCheck->stop();
    timerOnline->stop();
    timerConn->stop();
    timerReadAll->stop();

    isOk = false;
    if (portType == "Modbus_Com") {
        com->close();
        com = 0;
    } else if (portType == "Modbus_Tcp_Rtu") {
        tcpSocket->abort();
        tcpSocket = 0;
    }
}

void DeviceClient::setPause(bool pause)
{
    this->pause = pause;
}

void DeviceClient::append(const QString &type, quint8 addr, const QByteArray &body)
{
    QMutexLocker locker(&mutex);
    if (doTypes.count() < 256) {
        doTypes << type;
        doAddrs << addr;
        doBodys << body;
    }
}

void DeviceClient::clear()
{
    QMutexLocker locker(&mutex);
    doTypes.clear();
    doAddrs.clear();
    doBodys.clear();
}

void DeviceClient::readValueAll()
{
    //暂停状态下别搞了
    if (pause) {
        return;
    }

    //将轮询命令插入优先队列执行
    foreach (quint8 addr, addrs) {
        readValue(addr, true);
    }

    emit receiveInfo(portName, 0, "开始轮询所有设备");
}

void DeviceClient::readValueAll(quint8 start, quint8 end)
{
    if (start > end) {
        return;
    }

    //将轮询命令插入优先队列执行
    clear();
    for (int i = start; i < end; i++) {
        readValue(i, true);
    }

    emit receiveInfo(portName, 0, "开始轮询指定地址集合");
}

void DeviceClient::readValue(quint8 addr, bool append)
{
    if (!isOk && !append) {
        return;
    }

    QByteArray body;
    if (AppConfig::WorkMode == 0) {
        body = DeviceData::getSendData1(portName, addr);
    } else if (AppConfig::WorkMode == 3) {
        body = DeviceData::getSendData2(portName, addr);
    }

    if (body.length() == 0) {
        return;
    }

    //如果是优先执行则添加到队列
    if (append) {
        this->append("查询数值", addr, body);
    } else {
        this->writeData("查询数值", addr, body);
    }
}

void DeviceClient::writeData(quint8 addr, quint8 cmd, quint8 data1, quint8 data2, const QString &type)
{
    if (!isOk) {
        return;
    }

    QByteArray body;
    body.append(cmd);
    body.append(data_zero);
    body.append(data1);
    body.append(data_zero);
    body.append(data2);
    append(type, addr, body);
}

void DeviceClient::writeData(const QString &type, quint8 addr, const QByteArray &body)
{
    if (!isOk) {
        return;
    }

    QByteArray data;
    data.append(addr);
    data.append(body);
    data.append(QUIHelper::getCRCCode(data));

    if (portType == "Modbus_Com") {
        com->write(data);
    } else if (portType == "Modbus_Tcp_Rtu") {
        tcpSocket->write(data);
    }

    currentType = type;
    emit sendData(portName, addr, data);
    emit receiveInfo(portName, addr, type);
}
