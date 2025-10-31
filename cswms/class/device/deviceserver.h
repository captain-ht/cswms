#ifndef DEVICESERVER_H
#define DEVICESERVER_H

#include "deviceclient.h"
#include "Vendor.h"
#include "RealTimeDB.h"
#include "PortThread.h"

class DeviceServer : public QObject
{
    Q_OBJECT
public:
    static DeviceServer *Instance();
    explicit DeviceServer(QObject *parent = 0);
    ~DeviceServer();

private:
    static QScopedPointer<DeviceServer> self;
    QMutex mutex;

    //端口通信集合
    QList<DeviceClient *> deviceClients;

    //读取数据
    QTimer *timerReceive;
    //等待存储记录的列表
    QStringList positionIDs;
    QTimer *timerSave;

    //链表存储延时处理报警信息
    QList<QString> alarmIDs;
    QList<quint8> alarmTypes;
    QList<QDateTime> alarmTimes;
    QList<float> alarmValues;
    QTimer *timerCheck;

    // 获取工程名称
    QStringList m_listPortName;
    QList<Vendor *> m_VendorList;
    QList<PortThread *> m_listPortThread;

private slots:
    void saveData();
    void checkAlarm();
    void appendAlarm(const QString &id, quint8 type, float value);
    void removeAlarm(const QString &id, quint8 type);
    void doReceiveOnline(const QString &portName, quint8 addr, bool online);

    //设备采集模式
    void doReceiveValue();
    void doAlarmHH(quint8 &nodeStatus, const QString &positionID, float nodeValue, int index);
    void doAlarmHL(quint8 &nodeStatus, const QString &positionID, float nodeValue, int index);
    void doAlarmLL(quint8 &nodeStatus, const QString &positionID, float nodeValue, int index);

    void AddPortName(const QString name);
    Vendor *FindVendor(const QString name);

public slots:
    void doAlarm2(quint8 nodeStatus, const QString &positionID, float nodeValue, int index, const QString &nodeSign = "");
    void doAlarm(quint8 nodeStatus, const QString &positionID, float nodeValue, int index);

public:
    DeviceClient *getDeviceClient(const QString &portName);

signals:
    void sendData(const QString &portName, quint8 addr, const QByteArray &data);
    void receiveData(const QString &portName, quint8 addr, const QByteArray &data);
    void receiveInfo(const QString &portName, quint8 addr, const QString &data);
    void receiveError(const QString &portName, quint8 addr, const QString &data);
    void receiveValue(const QString &portName, quint8 addr, const QList<float> &values);
    void receiveOnline(const QString &deviceName, bool online);
    void receiveAlarm(const QString &deviceName, const QString &positionID, quint8 alarmType);

public slots:
    //初始化+启动+停止服务
    void init();
    bool Load();
    bool Unload();
    void start();
    void stop();

    //重新读取所有的值
    void readValueAll();
    //查询数值
    void readValue(const QString &portName, quint8 addr, bool all = false);   

    //发送数据
    void writeData(const QString &portName, const QString &type, quint8 addr, const QByteArray &body);
};

#endif // DEVICESERVER_H
