#ifndef UDPSEND_H
#define UDPSEND_H

#include <QtGui>
#include <QtNetwork>

class UdpSend : public QObject
{
    Q_OBJECT
public:
    static UdpSend *Instance();
    explicit UdpSend(QObject *parent = 0);
    ~UdpSend();

private:
    static QScopedPointer<UdpSend> self;
    QUdpSocket *udpSocket;

private slots:
    void sendData(const QString &portName, quint8 addr, const QByteArray &data);

public slots:
    void start();
    void stop();
    void sendInfo();
};

#endif // UDPSEND_H
