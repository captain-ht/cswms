#ifndef SENDMSGTHREAD_H
#define SENDMSGTHREAD_H

/**
 * 多线程收发短信类 GSM短信猫操作 2016-12-22
 * 1:可设置收发短信模式
 * 2:可批量发送短信以及支持长短信
 * 3:可读取指定指定序号短信
 * 4:可删除所有短信
 * 5:可检测短信猫设备是否正常
 * 6:支持中文短信发送
 * 7:支持拨打电话+挂断电话+接听来电+识别用户按键反馈
 * 8:支持批量发送给多个号码
 */

#include <QtGui>

class QextSerialPort;

class SendMsgThread : public QThread
{
    Q_OBJECT
public:
    static SendMsgThread *Instance();
    explicit SendMsgThread(QObject *parent = 0);
    ~SendMsgThread();

protected:
    void run();

private:
    static QScopedPointer<SendMsgThread> self;
    QReadWriteLock lock;
    volatile bool stopped;

    QStringList tasks;      //读取短信队列和删除短信队列
    QextSerialPort *com;    //短信猫通信串口对象
    bool isOpen;            //串口是否打开
    bool isLive;            //短信猫是否活着
    bool isRead;            //是否触发读取短信
    quint8 mode;            //短信猫收发模式
    bool autoDelete;        //是否自动删除短信
    int msgIndex;           //短信序号

public:
    //停止线程
    void stop();
    //打开串口
    bool open(const QString &portName);
    //初始化短信猫
    void init();
    //检测短信猫是否活着
    bool getIslive();
    //关闭串口
    void close();
    //清空串口数据及任务
    void clear();

    //设置接收短信后自动删除短信
    void setAutoDelete(bool autoDelete);
    //设置短信猫收发模式 text模式为1 pdu模式为0
    bool setMode(quint8 mode);
    //开启DTMF检测
    bool setDTMF();

    //执行AT命令
    QString sendAT(const QString &cmd, int sleep = 200);
    //单个号码发送短信
    bool sendMsg(const QString &tel, const QString &msg);
    //拨打电话
    void startCall(const QString &tel);
    //挂断电话
    void stopCall();
    //接听来电
    void answerCall();

    //获取短信猫厂商标识
    QString getGSMType();
    //获取短信中心号码
    QString getGSMCenterNum();

    //读取指定序号短信
    void readMsg(int msgIndex);
    //删除指定序号短信
    void deleteMsg(int msgIndex);
    //读取所有短信
    void readAllMsg();
    //删除所有短信
    void deleteAllMsg();

public:
    bool getIsOpen()        const;
    quint8 getMode()        const;
    bool getAutoDelete()    const;

private:    
    //获取手机号码(内存编码)
    QString getMsgTel(const QString &tel);
    //获取短信内容长度
    int getMsgLen(const QString &msg);
    //获取汉字16进制格式表示
    QString getMsgHex(const QString &msg);

    //unicode编码转为号码字符串
    QString unicodeToTel(const QString &unicode);
    //unicode编码转为时间字符串
    QString unicodeToTime(const QString &unicode);
    //unicode编码转为中文字符串
    QString unicodeToMsg(const QString &unicode);
    //7bit 编码转换为字符串
    QString unicodeToMsg7Bit(const QString &unicode);

    //16进制字符串转2进制字符串
    QString strHexToStrBin(const QString &strHex);
    //16进制字符串转10进制
    int strHexToDecimal(const QString &strHex);
    //2进制字符串转10进制
    int strBinToDecimal(const QString &strBin);

signals:
    //发送数据
    void sendData(const QString &data);
    //接收数据
    void receiveData(const QString &data);

    //收到短信
    void receiveMsg(const QString &centerNum, const QString &msgTel, const QString &msgTime, const QString &msgContent);
    //收到处理结果
    void receiveResult(const QString &result);
    //读取短信完成
    void readMsgFinish();
    //删除短信完成
    void deleteMsgFinish();

    //通话结束
    void callFinsh();
    //无人应答
    void callError();

    //新电话打入
    void receiveCall(const QString &tel);
    //用户反馈按键
    void receiveKey(const QString &key);

private slots:
    //读取数据
    void readData();
    //发送串口数据
    void writeData(const QString &data);

    //读取短信
    void readMsg();
    //删除短信
    void deleteMsg();

public slots:
    //追加短信到发送列表
    void append(const QString &tels, const QString &msgs);
};

#endif // SENDMSGTHREAD_H
