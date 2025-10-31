#ifndef UDPRECEIVE_H
#define UDPRECEIVE_H

#include <QtGui>
#include <QtNetwork>

class UdpReceive : public QObject
{
    Q_OBJECT
public:
    static UdpReceive *Instance();
    explicit UdpReceive(QObject *parent = 0);
    ~UdpReceive();

private:
    static QScopedPointer<UdpReceive> self;

    QString portName;           //端口名称
    QString currentType;        //当前命令类型

    QUdpSocket *udpSocket;      //udp通信对象
    QList<quint16> addrs;       //地址集合
    QList<QDateTime> times;     //设备最后的消息时间,用于判断单个设备是否离线
    QList<bool> onlines;        //设备是否在线
    QList<bool> lives;          //设备是否活过

private slots:
    void readData();
    void savePortInfo(const QStringList &data);
    void saveDeviceInfo(const QStringList &data);
    void saveNodeInfo(const QStringList &data);
    void checkData(const QString &data);

signals:
    void receiveData(const QString &portName, quint8 addr, const QByteArray &data);
    void receiveInfo(const QString &portName, quint8 addr, const QString &data);
    void receiveError(const QString &portName, quint8 addr, const QString &data);
    void receiveOnline(const QString &portName, quint8 addr, bool online);
    void receiveValue(const QString &portName, quint8 addr, const QList<quint16> &values);

public slots:
    void start();
    void stop();
    void reboot();
    void setAddrs(const QList<quint16> &addrs);
};

#endif // UDPRECEIVE_H
