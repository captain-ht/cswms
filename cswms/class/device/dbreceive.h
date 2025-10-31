#ifndef DBRECEIVE_H
#define DBRECEIVE_H

#include <QObject>
#include <QMutex>

class QTimer;

class DbReceive : public QObject
{
    Q_OBJECT
public:
    static DbReceive *Instance();
    explicit DbReceive(QObject *parent = 0);

private:
    static QScopedPointer<DbReceive> self;
    QMutex mutex;
    QTimer *timer;

signals:
    void debug(const QString &msg);
    void receiveValue(const QString &positionID, float value);

public slots:
    //启动+停止服务
    void start();
    void stop();

    //采集一次数据
    void checkData();

    //设置采集间隔
    void setInterval(int interval);
};

#endif // DBRECEIVE_H
