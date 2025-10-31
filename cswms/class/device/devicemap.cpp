#include "devicemap.h"
#include "quihelper.h"

QScopedPointer<DeviceMap> DeviceMap::self;
DeviceMap *DeviceMap::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new DeviceMap);
        }
    }

    return self.data();
}

DeviceMap::DeviceMap(QObject *parent) : QObject(parent)
{
}

DeviceMap::~DeviceMap()
{
}

void DeviceMap::loadMap()
{
    foreach (QString mapName, AppData::MapNames) {        
        QPixmap pix = QPixmap(QString("%1/%2").arg(AppData::MapPath).arg(mapName));
        pix = pix.scaled(AppData::MapWidth, AppData::MapHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        mapNames.append(mapName);
        mapPixs.append(pix);
    }
}

void DeviceMap::appendMapPix(const QString &mapName, const QPixmap &pix)
{
    int index = mapNames.indexOf(mapName);
    if (index >= 0) {
        mapPixs[index] = pix;
    } else {
        mapNames.append(mapName);
        mapPixs.append(pix);
    }
}

void DeviceMap::appendMapPix(const QString &mapName)
{
    QPixmap pix = QPixmap(QString("%1/%2").arg(AppData::MapPath).arg(mapName));
    pix = pix.scaled(AppData::MapWidth, AppData::MapHeight);
    mapNames.append(mapName);
    mapPixs.append(pix);
}

void DeviceMap::removeMapPix(const QString &mapName)
{
    int index = mapNames.indexOf(mapName);
    if (index >= 0) {
        mapNames.removeAt(index);
        mapPixs.removeAt(index);
    }
}

bool DeviceMap::existMapPix(const QString &mapName)
{
    return mapNames.contains(mapName);
}

QPixmap DeviceMap::getMapPix(const QString &mapName)
{
    QPixmap pix;
    int index = mapNames.indexOf(mapName);
    if (index >= 0) {
        pix = mapPixs.at(index);
    }

    return pix;
}
