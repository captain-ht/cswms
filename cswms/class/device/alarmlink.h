#ifndef ALARMLINK_H
#define ALARMLINK_H

#include <QtGui>
#include "qextserialport.h"

class AlarmLink : public QObject
{
    Q_OBJECT
public:
    static AlarmLink *Instance();
    explicit AlarmLink(QObject *parent = 0);

private:
    static QScopedPointer<AlarmLink> self;
    QMutex mutex;

    //位号报警队列
    QStringList alarmPositionIDs;
    //定时器排队处理报警
    QTimer *timerAlarm;

    //串口号+串口类列表
    QList<QString> comNames;
    QList<QextSerialPort *> coms;

    //存储当前已报警的模块地址及位集合
    //格式 COM1|1|0000000000000000  依次表示串口号|模块地址|报警位
    QList<QString> alarmIDs;

private slots:
    void readData();

signals:
    void sendData(const QString &portName, quint8 addr, const QByteArray &data);
    void receiveData(const QString &portName, quint8 addr, const QByteArray &data);
    void receiveInfo(const QString &portName, quint8 addr, const QString &data);
    void receiveError(const QString &portName, quint8 addr, const QString &data);

public slots:
    //初始化+启动+停止服务
    void init();
    void start();
    void stop();

    //处理一次报警
    void checkAlarm();
    void checkAlarm(const QString &positionID, bool alarm);

    //追加报警位号
    void append(const QString &positionID);
    //消音报警位号
    void remove(const QString &positionID);
    //复位所有,软件启动后执行一次
    void reset();

    //发送报警数据,联动串口
    void writeData(int index, const QString &type, quint8 addr, const QByteArray &body);
    //发送报警数据,联动串口+通信串口
    void writeData(const QString &comName, const QString &type, quint8 modelAddr, const QByteArray &body);
};

#endif // ALARMLINK_H
