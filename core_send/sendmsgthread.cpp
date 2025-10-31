#include "sendmsgthread.h"
#include "qextserialport.h"

#pragma execution_character_set("utf-8")
#define TIMEMS qPrintable(QTime::currentTime().toString("hh:mm:ss zzz"))

QScopedPointer<SendMsgThread> SendMsgThread::self;
SendMsgThread *SendMsgThread::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new SendMsgThread);
        }
    }

    return self.data();
}

SendMsgThread::SendMsgThread(QObject *parent) : QThread(parent)
{
    stopped = false;
    com = NULL;
    isOpen = false;
    isLive = false;
    isRead = false;
    mode = 1;
    autoDelete = false;
    msgIndex = 0;
}

SendMsgThread::~SendMsgThread()
{
    this->stop();
    this->wait();
}

void SendMsgThread::stop()
{
    stopped = true;
    isRead = false;
    mode = 1;
    autoDelete = false;
    msgIndex = 0;
}

bool SendMsgThread::getIsOpen() const
{
    return this->isOpen;
}

quint8 SendMsgThread::getMode() const
{
    return this->mode;
}

bool SendMsgThread::getAutoDelete() const
{
    return this->autoDelete;
}

void SendMsgThread::run()
{
    while (!stopped) {
        //优先处理短信读取
        if (isRead) {
            msleep(500);
            readMsg();
            deleteMsg();
            continue;
        }

        //逐个处理发送短信读取短信和删除短信
        int count = tasks.count();
        if (count > 0) {
            lock.lockForRead();
            QString task = tasks.takeFirst();
            lock.unlock();

            QStringList list = task.split("|");
            QString type = list.at(0);
            QString cmd = list.at(1);
            QString msg = list.at(2);
            QString result;

            if (isLive) {
                if (type == "SendMsg") {
                    if (sendMsg(cmd, msg)) {
                        result = "短信发送成功";
                    } else {
                        result = "短信发送失败";
                    }
                    msleep(1000);
                } else if (type == "ReadMsg") {
                    sendAT(cmd, 0);
                    if (cmd.split("=").at(1).toInt() == 50) {
                        emit readMsgFinish();
                    }
                } else if (type == "DeleteMsg") {
                    sendAT(cmd, 0);
                    if (cmd.split("=").at(1).toInt() == 50) {
                        emit deleteMsgFinish();
                    }
                }
            } else {
                result = "短信猫通讯故障";
            }

            if (!result.isEmpty()) {
                emit receiveResult(result);
            }
        }

        msleep(100);
    }

    stopped = false;
    isRead = false;
}

bool SendMsgThread::open(const QString &portName)
{
    com = new QextSerialPort(portName, QextSerialPort::EventDriven);
    connect(com, SIGNAL(readyRead()), this, SLOT(readData()));
    isOpen = com->open(QIODevice::ReadWrite);

    if (isOpen) {
        com->setBaudRate(BAUD9600);
        com->setDataBits(DATA_8);
        com->setParity(PAR_NONE);
        com->setStopBits(STOP_1);
        com->setFlowControl(FLOW_OFF);
        com->setTimeout(10);
    }

    return isOpen;
}

void SendMsgThread::init()
{
    if (getIslive()) {
        //设置短信接收模式
        sendAT("AT+CNMI=2,1", 200);

        //设置短信收发处理模式
        int errorCount = 0;
        while (errorCount < 5) {
            if (setMode(0)) {
                break;
            }
            errorCount++;
        }

        if (errorCount == 5) {
            emit receiveResult("设置短信模式失败");
        }
    }
}

bool SendMsgThread::getIslive()
{
    if (isOpen) {
        disconnect(com, SIGNAL(readyRead()), this, SLOT(readData()));
        writeData(QString("%1\r").arg("AT"));
        msleep(300);

        QString data = com->readAll();
        data.replace(" ", "");
        data.replace("\r", "");
        data.replace("\n", "");
        if (data.length() > 0) {
            emit receiveData(data);
        }

        //收到返回ATOK说明短信猫连接成功
        if (data == "ATOK") {
            isLive = true;
        } else {
            isLive = false;
        }
        connect(com, SIGNAL(readyRead()), this, SLOT(readData()), Qt::DirectConnection);
    } else {
        isLive = false;
    }

    //2018-4-11 增加永远为true
    isLive = true;
    return isLive;
}

void SendMsgThread::close()
{
    if (com != NULL && isOpen) {
        disconnect(com, SIGNAL(readyRead()), this, SLOT(readData()));
        com->close();
        com = NULL;
        isLive = false;
        isOpen = false;
    }
}

void SendMsgThread::clear()
{
    tasks.clear();
}

void SendMsgThread::setAutoDelete(bool autoDelete)
{
    this->autoDelete = autoDelete;
}

bool SendMsgThread::setMode(quint8 mode)
{
    QString result = sendAT(QString("AT+CMGF=%1").arg(mode), 200);
    if (result.right(2) == "OK") {
        this->mode = mode;
        emit receiveResult(QString("设置%1成功").arg(mode == 0 ? "Pdu模式" : "Text模式"));
        return true;
    }

    return false;
}

bool SendMsgThread::setDTMF()
{
    QString result = sendAT(QString("AT+DDET=%1").arg(1), 200);
    if (result.right(2) == "OK") {
        emit receiveResult(QString("开启DTMF检测成功"));
        return true;
    }

    return false;
}

void SendMsgThread::readData()
{
    if (com->bytesAvailable() <= 2) {
        return;
    }

    isRead = true;
}

void SendMsgThread::writeData(const QString &data)
{
    if (isOpen) {
        com->write(data.toLatin1());
        emit sendData(data);
    }
}

void SendMsgThread::readMsg()
{
    isRead = false;
    QString data = com->readAll();
    data.replace(" ", "");
    data.replace("\r", "");
    data.replace("\n", "");
    if (data.length() <= 0) {
        return;
    }

    //从SIM卡上读取短信回复  "AT+CMGR=5+CMGR:"RECREAD","+8618001797656",,"16/05/28,18:12:52+32"006:WD+05.1,ZL139.6,YL1.2,AUTO,YSJ-ONOK"
    //主动收到短信          "AT+CMGR=6+CMGR:"RECUNREAD","+8618001797656",,"16/05/28,18:20:51+32"006:WD+05.1,ZL139.6,YL1.2,AUTO,YSJ-ONOK"
    qDebug() << TIMEMS << "Receive:" << data;
    emit receiveData(data);

    if (data.endsWith(",,0OK")) {
        clear();
        emit readMsgFinish();
        return;
    }

    //收到行短信后会收到 +CMTI:"SM",2 ,其中2为短信索引
    //接着根据新短信的索引读取短信,读取短信成功返回 AT+CMGR=5+CMGR:0,,230891683110304105F0240D91688110978310F000004121519115322303E13919
    if (data.contains("CMTI")) {
        //获取新短信的索引号
        msgIndex = data.split(",").at(1).toInt();
        writeData(QString("AT+CMGR=%1\r").arg(msgIndex));
    } else if (data.contains("CMGR")) {
        //一步步解析服务中心号码,发件人号码,发送时间,短信内容
        QStringList list = data.split(",");
        if (list.count() < 3) {
            return;
        }

        if (mode == 0) {
            QString unicode = list.at(2);
            int len = unicode.length();

            //去除末尾ok两个字符
            unicode = unicode.mid(0, len - 2);
            if (unicode == "0") {
                clear();
                emit readMsgFinish();
                return;
            }

            int index = unicode.indexOf("08");
            QString centerNum = unicodeToTel(unicode.mid(index + 6, 12));
            QString msgTel = unicodeToTel(unicode.mid(index + 26, 12));
            QString msgType = unicode.mid(index + 40, 2);
            QString msgTime = unicodeToTime(unicode.mid(index + 42, 12));
            QString msgContent = unicode.mid(index + 58);

            //类型为 00 则采用的是 7bit 编码,需要用 7bit 解码
            //类型为 08 则采用的是 unicode 编码
            if (msgType == "00") {
                msgContent = unicodeToMsg7Bit(msgContent);
            } else if (msgType == "08") {
                msgContent = unicodeToMsg(msgContent);
            }

            //发送收到新短信信号
            emit receiveMsg(centerNum, msgTel, msgTime, msgContent);
        } else {
            QString centerNum = "13800000000";
            QString msgTel = list.at(1).mid(4, 11);
            QString msgTime = list.at(3);
            msgTime = msgTime.mid(1, msgTime.length());
            msgTime.replace("/", "-");
            msgTime = "20" + msgTime + " " + list.at(4).mid(0, 8);

            QStringList str;
            for (int i = 4; i < list.count(); i++) {
                str.append(list.at(i));
            }

            QString msgContent = str.join(",");
            msgContent = msgContent.split("\"").at(1);
            msgContent = msgContent.mid(0, msgContent.length() - 2);

            //发送收到新短信信号
            emit receiveMsg(centerNum, msgTel, msgTime, msgContent);
        }
    } else if (data.contains("NOCARRIER")) {
        emit callFinsh();
    } else if (data.contains("NOANSWER")) {
        emit callError();
    } else if (data.contains("RING")) {
        QString tel;
        QStringList list = data.split("\"");
        if (list.count() > 1) {
            tel = list.at(1);
        }
        emit receiveCall(tel);
    } else if (data.contains("DTMF")) {
        int index = data.indexOf("DTMF");
        QString key = data.mid(index + 5, 1);
        emit receiveKey(key);
    }
}

void SendMsgThread::deleteMsg()
{
    //自动删除SIM卡上的短信
    if (autoDelete) {
        deleteMsg(msgIndex);
    }
}

QString SendMsgThread::sendAT(const QString &cmd, int sleep)
{
    if (!isOpen || !isLive) {
        return "";
    }

    //主动执行命令期间,断开收到数据信号槽连接
    disconnect(com, SIGNAL(readyRead()), this, SLOT(readData()));
    writeData(QString("%1\r").arg(cmd));
    msleep(sleep);

    QString data = com->readAll();
    data.replace(" ", "");
    data.replace("\r", "");
    data.replace("\n", "");
    if (data.length() > 0) {
        qDebug() << TIMEMS << "Result:" << data;
        emit receiveData(data);
    }

    //完成处理后,重新绑定收到数据信号槽连接
    connect(com, SIGNAL(readyRead()), this, SLOT(readData()), Qt::DirectConnection);
    return data;
}

bool SendMsgThread::sendMsg(const QString &tel, const QString &msg)
{
    bool ok = false;
    char ctrlz  = 26;
    int sleepTime;
    QString msgContent;
    QString sendMsg;

    if (mode == 0) {
        sleepTime = 6000;
        msgContent = QString("%1%2%3%4%5").arg("0011000D91").arg(getMsgTel(tel))
                     .arg("000801").arg(getMsgHex(msg)).arg(ctrlz);
        sendMsg = QString::number(getMsgLen(msg));
    } else {
        sleepTime = 1000;
        msgContent = QString("%1%2").arg(msg).arg(ctrlz);
        sendMsg = tel;
    }

    //首先进入发送短信状态
    QString result = sendAT(QString("AT+CMGS=%1\r").arg(sendMsg), 300);
    //返回结果最末一位为 > 后输入短信内容编码,发送短信正文
    if (result.right(1) == ">") {
        result = sendAT(msgContent, sleepTime);
        if (mode == 0) {
            if (result.right(2) == "OK") {
                ok = true;
            }
        } else {
            if (result.right(2) == "OK" || result.startsWith(msg)) {
                ok = true;
            }
        }
    }

    return ok;
}

void SendMsgThread::startCall(const QString &tel)
{
    sendAT(QString("ATD%1;").arg(tel), 100);
}

void SendMsgThread::stopCall()
{
    sendAT(QString("ATH"), 100);
}

void SendMsgThread::answerCall()
{
    sendAT(QString("ATA"), 100);
}

QString SendMsgThread::getGSMType()
{
    QString result = sendAT("AT+CGMI", 500);
    QString type = "";
    //如果成功会返回 AT+CGMISIEMENSOK,对应类型要减去开头7位和末尾2位
    if (result.right(2) == "OK") {
        type = result.mid(7, result.length() - 7 - 2);
    }

    return type;
}

QString SendMsgThread::getGSMCenterNum()
{
    QString result = sendAT("AT+CSCA?", 500);
    QString num = "";
    //如果成功会返回 AT+CSCA?+CSCA:"+8613010314500",145OK
    if (result.right(2) == "OK") {
        num = result.split("\"").at(1);
    }

    return num;
}

void SendMsgThread::readMsg(int msgIndex)
{
    writeData(QString("AT+CMGR=%1\r").arg(msgIndex));
}

void SendMsgThread::deleteMsg(int msgIndex)
{
    msleep(500);
    sendAT(QString("AT+CMGD=%1\r").arg(msgIndex), 200);
}

void SendMsgThread::readAllMsg()
{
    //SIM卡最多可以存储50条短信
    for (int i = 1; i <= 50; i++) {
        tasks.append(QString("ReadMsg|AT+CMGR=%1\r|").arg(i));
    }
}

void SendMsgThread::deleteAllMsg()
{
    //SIM卡最多可以存储50条短信
    for (int i = 1; i <= 50; i++) {
        tasks.append(QString("DeleteMsg|AT+CMGD=%1|").arg(i));
    }
}

int SendMsgThread::getMsgLen(const QString &msg)
{
    int len = msg.length() * 2;
    len += 15;
    return len;
}

QString SendMsgThread::getMsgTel(const QString &tel)
{
    QString temp;
    //检查手机号码是否按照标准格式写,如果不是则补上
    if (tel.mid(0, 2) != "86") {
        temp = QString("86%1F").arg(tel);
    }

    QByteArray ba = temp.toLatin1();
    QString str = "";
    //按照内存编码格式,将每两位的顺序调换
    for (int i = 0; i < 13; i = i + 2) {
        str += QString("%1%2").arg(ba.at(i + 1)).arg(ba.at(i));
    }

    return str;
}

QString SendMsgThread::getMsgHex(const QString &msg)
{
    int len = msg.length();
    wchar_t *buffer = new wchar_t[len];
    msg.toWCharArray((wchar_t *)buffer);
    QStringList data;
    for (int i = 0; i < len; i++) {
        QString temp = QString::number(buffer[i], 16);
        if (temp.length() == 2) {
            temp = "00" + temp;
        }
        data.append(temp);
    }

    //将长度及短信内容的unicode编码格式内容拼接
    QString lenHex = QString::number(len * 2, 16);
    QString msgHex = data.join("");
    QString str = QString("%1%2").arg(lenHex).arg(msgHex);
    delete [] buffer;
    return str.toUpper();
}

QString SendMsgThread::unicodeToTel(const QString &unicode)
{
    //按照内存编码格式,将每两位的顺序调换
    QString tel;
    for (int i = 0; i < 12; i = i + 2) {
        tel += QString("%1%2").arg(unicode.at(i + 1)).arg(unicode.at(i));
    }

    //去除最后一位 F
    tel = tel.remove("F");
    return tel;
}

QString SendMsgThread::unicodeToTime(const QString &unicode)
{
    //按照内存编码格式,将每两位的顺序调换
    QString time;
    for (int i = 0; i < 12; i = i + 2) {
        time += QString("%1%2").arg(unicode.at(i + 1)).arg(unicode.at(i));
    }

    time = QString("20%1-%2-%3 %4:%5:%6")
           .arg(time.mid(0, 2))
           .arg(time.mid(2, 2))
           .arg(time.mid(4, 2))
           .arg(time.mid(6, 2))
           .arg(time.mid(8, 2))
           .arg(time.mid(10, 2));

    return time;
}

QString SendMsgThread::unicodeToMsg(const QString &unicode)
{
    QString msg;
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QStringList str;
    for (int i = 0; i < unicode.length(); i = i + 4) {
        str.append(unicode.mid(i, 4));
    }

    QString s;
    foreach (const QString &unicode, str) {
        s.append(unicode.toUShort(0, 16));
    }

    msg = codec->fromUnicode(s);
    return msg;
}

QString SendMsgThread::unicodeToMsg7Bit(const QString &unicode)
{
    //参考网页 http://www.51testing.com/html/51/515351-816463.html
    //先将16进制字符串,从倒数第二位开始逆着转
    //例如 E6F3198D3603 需转为 03 36 8D 19 F3 E6
    QString msg;
    int len = unicode.length();
    QStringList listHex;
    for (int i = len - 2; i >= 0; i = i - 2) {
        listHex.append(unicode.mid(i, 2));
    }

    //将16进制数据转为2进制字符串形式拼起来
    QStringList listBin;
    foreach (QString hex, listHex) {
        listBin.append(strHexToStrBin(hex));
    }

    //从尾开始,一次取7位,作为一个新的8位字节(第8位补0)
    //N表示 7bit 的字节数, 次数 = N+[N/7]
    QString strBin = listBin.join("");
    int count = listHex.count();
    int n = count + (count / 7);
    int startIndex = strBin.length() - 7;
    for (int i = 0; i < n; i++) {
        QString bin = QString("0%1").arg(strBin.mid(startIndex, 7));
        QByteArray ba;
        ba.append(strBinToDecimal(bin));
        msg.append(QString(ba));
        startIndex = startIndex - 7;
    }

    return msg;
}

QString SendMsgThread::strHexToStrBin(const QString &strHex)
{
    uchar decimal = strHexToDecimal(strHex);
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();
    if (len < 8) {
        for (int i = 0; i < 8 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

int SendMsgThread::strHexToDecimal(const QString &strHex)
{
    bool ok;
    return strHex.toInt(&ok, 16);
}

int SendMsgThread::strBinToDecimal(const QString &strBin)
{
    bool ok;
    return strBin.toInt(&ok, 2);
}

void SendMsgThread::append(const QString &tels, const QString &msgs)
{
    lock.lockForWrite();

    //支持长短信发送,判断短信内容长度,求出短信条数
    int len = msgs.length();
    double d = (double)len / 70;
    int i = (int)d;
    int count = (d == i ? i : i + 1);
    QStringList listMsg;
    int index = 0;
    for (int i = 0; i < count; i++) {
        if (i == (count - 1)) {
            listMsg.append(msgs.mid(index, len - index));
        } else {
            listMsg.append(msgs.mid(index, 70));
        }

        index = index + 70;
    }

    QStringList listTel = tels.split(";");
    foreach (QString tel, listTel) {
        foreach (QString msg, listMsg) {
            tasks.append(QString("SendMsg|%1|%2").arg(tel).arg(msg));
        }
    }

    lock.unlock();
}
