#ifndef DEVICEMAP_H
#define DEVICEMAP_H

#include <QtGui>
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

class DeviceMap : public QObject
{
    Q_OBJECT
public:
    static DeviceMap *Instance();
    explicit DeviceMap(QObject *parent = 0);
    ~DeviceMap();

private:
    static QScopedPointer<DeviceMap> self;
    QList<QString> mapNames;
    QList<QPixmap> mapPixs;

public slots:
    //载入地图
    void loadMap();

    //添加到图片队列
    void appendMapPix(const QString &mapName, const QPixmap &pix);
    //添加地图数据
    void appendMapPix(const QString &mapName);
    //移除地图数据
    void removeMapPix(const QString &mapName);

    //是否已经存在地图数据
    bool existMapPix(const QString &mapName);
    //获取地图数据
    QPixmap getMapPix(const QString &mapName);

};

#endif // DEVICEMAP_H
