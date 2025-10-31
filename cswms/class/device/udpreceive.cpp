#include "udpreceive.h"
#include "quihelper.h"
#include "dbquery.h"

QScopedPointer<UdpReceive> UdpReceive::self;
UdpReceive *UdpReceive::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new UdpReceive);
        }
    }

    return self.data();
}

UdpReceive::UdpReceive(QObject *parent) : QObject(parent)
{
    portName = "UDP端口";
    currentType = "查询数值";
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
}

UdpReceive::~UdpReceive()
{
}

void UdpReceive::readData()
{
    QHostAddress host;
    quint16 port;
    QByteArray data;

    while (udpSocket->hasPendingDatagrams()) {
        data.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(data.data(), data.size(), &host, &port);
        QString buffer = QString::fromUtf8(data);
        checkData(buffer);
    }
}

void UdpReceive::savePortInfo(const QStringList &data)
{
    //清空原有设备数据
    QString sql = "delete from PortInfo";
    DbHelper::execSql(sql);

    //开启数据库事务
    QSqlDatabase::database().transaction();

    for (int i = 0; i < data.count(); i++) {
        QStringList portInfo = data.at(i).split(";");
        int count = portInfo.count();
        sql = "insert into PortInfo(PortID,PortName,PortType,ComName,BaudRate,TcpIP,TcpPort,ReadInterval,ReadTimeout,ReadMaxtime) values('";

        for (int j = 0; j < count - 1; j++) {
            sql = sql + portInfo.at(j) + "','";
        }

        sql = sql + portInfo.at(count - 1) + "')";
        DbHelper::execSql(sql);
    }

    //提交数据库事务
    if (QSqlDatabase::database().commit()) {
        DbQuery::addUserLog("批量插入端口信息成功");
        emit receiveInfo(portName, 255, "批量插入端口信息成功");
    } else {
        QSqlDatabase::database().rollback();
        DbQuery::addUserLog("批量插入端口信息失败");
        emit receiveError(portName, 255, "批量插入端口信息失败");
    }
}

void UdpReceive::saveDeviceInfo(const QStringList &data)
{
    //清空原有设备数据
    QString sql = "delete from DeviceInfo";
    DbHelper::execSql(sql);

    //开启数据库事务
    QSqlDatabase::database().transaction();

    for (int i = 0; i < data.count(); i++) {
        QStringList portInfo = data.at(i).split(";");
        int count = portInfo.count();
        sql = "insert into DeviceInfo(DeviceID,PortName,DeviceName,DeviceAddr,DeviceType,NodeNumber) values('";

        for (int j = 0; j < count - 1; j++) {
            sql = sql + portInfo.at(j) + "','";
        }

        sql = sql + portInfo.at(count - 1) + "')";
        DbHelper::execSql(sql);
    }

    //提交数据库事务
    if (QSqlDatabase::database().commit()) {
        DbQuery::addUserLog("批量插入设备信息成功");
        emit receiveInfo(portName, 255, "批量插入设备信息成功");
    } else {
        QSqlDatabase::database().rollback();
        DbQuery::addUserLog("批量插入设备信息失败");
        emit receiveError(portName, 255, "批量插入设备信息失败");
    }
}

void UdpReceive::saveNodeInfo(const QStringList &data)
{
    //清空原有设备数据
    QString sql = "delete from NodeInfo";
    DbHelper::execSql(sql);

    //开启数据库事务
    QSqlDatabase::database().transaction();

    for (int i = 0; i < data.count(); i++) {
        QStringList portInfo = data.at(i).split(";");
        int count = portInfo.count();
        sql = "insert into NodeInfo(NodeID,PositionID,DeviceName,NodeName,NodeAddr,NodeType,NodeClass,NodeSign,NodeUpper,NodeLimit,NodeMax,"
              "NodeMin,NodeRange,NodeEnable,NodeSound,NodeImage,SaveInterval,DotCount,AlarmDelay,AlarmType,NodeX,NodeY) values('";

        for (int j = 0; j < count - 1; j++) {
            sql = sql + portInfo.at(j) + "','";
        }

        sql = sql + portInfo.at(count - 1) + "')";
        DbHelper::execSql(sql);
    }

    //提交数据库事务
    if (QSqlDatabase::database().commit()) {
        DbQuery::addUserLog("批量插入变量信息成功");
        emit receiveInfo(portName, 255, "批量插入变量信息成功");
    } else {
        QSqlDatabase::database().rollback();
        DbQuery::addUserLog("批量插入变量信息失败");
        emit receiveError(portName, 255, "批量插入变量信息失败");
    }
}

void UdpReceive::checkData(const QString &data)
{
    //取出端口名称+地址+数据
    QStringList list = data.split("|");
    if (list.count() != 2) {
        return;
    }

    //根据不同的端口处理数据,表信息也是端口号字段发过来的
    QString portName = list.at(0);
    if (portName == "PortInfo") {
        emit receiveInfo(portName, 255, "收到端口表数据");
        savePortInfo(list.at(1).split(" "));
        return;
    } else if (portName == "DeviceInfo") {
        emit receiveInfo(portName, 255, "收到设备表数据");
        saveDeviceInfo(list.at(1).split(" "));
        return;
    } else if (portName == "NodeInfo") {
        emit receiveInfo(portName, 255, "收到变量表数据");
        saveNodeInfo(list.at(1).split(" "));
        //变量表是最后发过来的,收到后延时重启应用新的数据
        QTimer::singleShot(3000, this, SLOT(reboot()));
        return;
    }

    QByteArray buffer = QUIHelper::hexStrToByteArray(list.at(1));

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

    //来过消息的设备,立马更新最后的消息时间,以及判断设备上线
    int index = addrs.indexOf(addr);
    times[index] = QDateTime::currentDateTime();
    if ((!lives.at(index) && onlines.at(index)) || !onlines.at(index)) {
        onlines[index] = true;
        lives[index] = true;
        emit receiveOnline(portName, addr, true);
        emit receiveInfo(portName, addr, "设备上线");
    }

    //根据不同的cmd+不同的命令类型 取出对应的数据内容
    if (cmd == 0x03) {
        QString info;
        if (currentType == "查询数值") {
            QList<quint16> values;
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
}

void UdpReceive::start()
{
    udpSocket->abort();

#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
    bool ok = udpSocket->bind(QHostAddress::AnyIPv4, AppConfig::NetReceivePort);
#else
    bool ok = udpSocket->bind(QHostAddress::Any, AppConfig::NetReceivePort);
#endif

    QString result = QString("网络接收监听%1").arg(ok ? "成功" : "失败");
    DbQuery::addUserLog("设备上报", result);
    emit receiveInfo(portName, 255, result);
}

void UdpReceive::stop()
{
    QString result = "网络接收停止服务";
    DbQuery::addUserLog("设备上报", result);
    emit receiveInfo(portName, 255, result);
}

void UdpReceive::reboot()
{
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
    qApp->closeAllWindows();
}

void UdpReceive::setAddrs(const QList<quint16> &addrs)
{
    this->addrs = addrs;

    //重新设置设备对应的最后消息时间+是否在线
    times.clear();
    onlines.clear();
    lives.clear();

    int count = addrs.count();
    for (int i = 0; i < count; i++) {
        //最后消息时间为当前时间
        times << QDateTime::currentDateTime();
        //假设设备在线,不然轮询会跳过去
        onlines << true;
        //是否首次处理过存活
        lives << false;
    }
}
