#ifndef DEVICECLIENT_H
#define DEVICECLIENT_H

#include <QtGui>
#include <QtNetwork>
#include "qextserialport.h"

class DeviceClient : public QObject
{
    Q_OBJECT
public:
    static DeviceClient *Instance();
    explicit DeviceClient(QObject *parent = 0);

private:
    static QScopedPointer<DeviceClient> self;
    QMutex mutex;               //锁对象
    int currentIndex;           //当前地址索引

    QString portType;           //端口类型 COM TCP
    QString portName;           //端口名称
    QString comName;            //端口号
    int baudRate;               //波特率
    QString tcpIP;              //服务器IP
    int tcpPort;                //服务器端口
    int readInterval;           //采集间隔
    int readTimeout;            //超时次数
    int readMaxtime;            //重新轮询所有设备时间
    QList<quint8> addrs;        //地址集合

    QList<QDateTime> times;     //设备最后的消息时间,用于判断单个设备是否离线
    QList<bool> onlines;        //设备是否在线

    bool isOk;                  //通信是否正常
    bool pause;                 //暂停轮询
    QDateTime lastTime;         //最后的消息时间,用于判断整个通信是否离线
    QByteArray buffer;          //数据队列

    QextSerialPort *com;        //串口通信类对象
    QTcpSocket *tcpSocket;      //网络通信类对象

    QTimer *timerConn;          //定时器处理重连
    QTimer *timerOnline;        //定时器处理离线
    QTimer *timerRead;          //定时器采集
    QTimer *timerCheck;         //定时器处理数据
    QTimer *timerReadAll;       //重新读取所有设备地址

    QString currentType;        //当前命令类型
    QList<QString> doTypes;     //优先执行命令的解释说明
    QList<quint8> doAddrs;      //优先执行的地址
    QList<QByteArray> doBodys;  //优先执行的命令

public:
    QString getPortName()       const;
    QList<quint8> getAddrs()    const;

private slots:
    void connected();           //设备连上
    void disconnected();        //设备断开
    void checkConn();           //校验连接,自动重连
    void checkOnline();         //处理掉线
    void readData();            //读取数据
    void readValue();           //查询值
    void checkValue();          //处理值

signals:
    void sendData(const QString &portName, quint8 addr, const QByteArray &data);
    void receiveData(const QString &portName, quint8 addr, const QByteArray &data);
    void receiveInfo(const QString &portName, quint8 addr, const QString &data);
    void receiveError(const QString &portName, quint8 addr, const QString &data);
    void receiveOnline(const QString &portName, quint8 addr, bool online);
    void receiveValue(const QString &portName, quint8 addr, const QList<quint16> &values);

public slots:
    void setPortType(const QString &portType);
    void setPortName(const QString &portName);
    void setComName(const QString &comName);
    void setBaudRate(int baudRate);
    void setTcpIP(const QString &tcpIP);
    void setTcpPort(int tcpPort);
    void setReadInterval(int readInterval);
    void setReadTimeout(int readTimeout);
    void setReadMaxtime(int readMaxtime);
    void setAddrs(const QList<quint8> &addrs);

    //初始化+启动+停止+暂停 采集
    void init();
    void start();
    void stop();
    void setPause(bool pause);

    //插入优先执行的命令
    void append(const QString &type, quint8 addr, const QByteArray &body);
    //清空优先执行的指令
    void clear();

    //手动触发重新读取所有离线设备,单个设备直接readValue(地址)即可
    void readValueAll();
    //广播指定的地址集合
    void readValueAll(quint8 start, quint8 end);
    //查询数值
    void readValue(quint8 addr, bool append = false);

    //通用命令+发送数据
    void writeData(quint8 addr, quint8 cmd, quint8 data1, quint8 data2, const QString &type);
    void writeData(const QString &type, quint8 addr, const QByteArray &body);
};

#endif // DEVICECLIENT_H
