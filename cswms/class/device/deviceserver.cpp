#include "deviceserver.h"
#include "quihelper.h"
#include "dbquery.h"
#include "devicehelper.h"
#include "sendserver.h"
#include "udpsend.h"
#include "udpreceive.h"
#include "dbreceive.h"
#include "alarmlink.h"

#include "public.h"
#include "DBTagObject.h"
#include "RealTimeDB.h"
#include "Tag.h"
#include "IOTag.h"
#include "VendorPluginManager.h"
#include "ComPort.h"
#include "NetPort.h"
#include "SysRuntimeEvent.h"
#include "3rd_log4qt/logger.h"
#include "log.h"
#include "ProjectInfoManager.h"
#include "ProjectData.h"

QScopedPointer<DeviceServer> DeviceServer::self;
DeviceServer *DeviceServer::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new DeviceServer);
        }
    }

    return self.data();
}

DeviceServer::DeviceServer(QObject *parent) : QObject(parent)
{
    //启动定时器处理数据的读取
    timerReceive = new QTimer(this);
    connect(timerReceive, SIGNAL(timeout()), this, SLOT(doReceiveValue()));
    timerReceive->start(500);
    //启动定时器处理记录的存储
    timerSave = new QTimer(this);
    connect(timerSave, SIGNAL(timeout()), this, SLOT(saveData()));
    timerSave->start(1000);

    //启动定时器处理报警
    timerCheck = new QTimer(this);
    connect(timerCheck, SIGNAL(timeout()), this, SLOT(checkAlarm()));
    timerCheck->start(1000);
}

DeviceServer::~DeviceServer()
{
    timerSave->stop();
    timerCheck->stop();
    Unload();
}

void DeviceServer::saveData()
{
    //如果是数据库采集则不存储数据
    if (AppConfig::WorkMode == 1) {
        return;
    }

    QMutexLocker lock(&mutex);
    QDateTime now = QDateTime::currentDateTime();

    //启动数据库事务,加快存储
    QSqlDatabase::database().transaction();

    //优先处理上线后的记录
    for (int i = 0; i < positionIDs.count(); i++) {
        QString positionID = positionIDs.at(i);
        int index = DbData::NodeInfo_PositionID.indexOf(positionID);
        QString deviceName = DbData::NodeInfo_DeviceName.at(index);
        QString nodeName = DbData::NodeInfo_NodeName.at(index);
        QString nodeSign = DbData::NodeInfo_NodeSign.at(index);
        float nodeValue = DbData::NodeInfo_NodeValue.at(index);
        DbQuery::addNodeLog(positionID, deviceName, nodeName, nodeValue, nodeSign);
        DbData::NodeInfo_SaveTime[index] = now;
    }

    positionIDs.clear();

    //处理是否到了存储时间进行记录的存储
    for (int i = 0; i < DbData::NodeInfo_Count; i++) {
        //离线的就不用处理
        if (!DbData::NodeInfo_Online.at(i)) {
            continue;
        }

        int saveInterval = DbData::NodeInfo_SaveInterval.at(i);
        QDateTime saveTime = DbData::NodeInfo_SaveTime.at(i);
        int timeout = saveTime.secsTo(now);
        int offset = saveInterval * 1;
        if (timeout >= offset) {
            QString positionID = DbData::NodeInfo_PositionID.at(i);
            QString deviceName = DbData::NodeInfo_DeviceName.at(i);
            QString nodeName = DbData::NodeInfo_NodeName.at(i);
            QString nodeSign = DbData::NodeInfo_NodeSign.at(i);
            float nodeValue = DbData::NodeInfo_NodeValue.at(i);
            DbQuery::addNodeLog(positionID, deviceName, nodeName, nodeValue, nodeSign);
            DbData::NodeInfo_SaveTime[i] = now;
        }
    }

    if (!QSqlDatabase::database().commit()) {
        QSqlDatabase::database().rollback();
    }
}

void DeviceServer::checkAlarm()
{
    //逐个判断队列中的报警类型+报警时间,与到期时间是否到了,到了还处于报警则算真正的报警
    //在恢复的地方移除对应类型的报警信息
    QDateTime now = QDateTime::currentDateTime();
    int count = alarmIDs.count();
    for (int i = 0; i < count; i++) {
        QString positionID = alarmIDs.at(i);
        quint8 type = alarmTypes.at(i);
        QDateTime time = alarmTimes.at(i);
        float nodeValue = alarmValues.at(i);
        qint64 timeout = time.secsTo(now);

        quint8 nodeStatus = 4;
        int index = DbData::NodeInfo_PositionID.indexOf(positionID);
        int alarmDelay = DbData::NodeInfo_AlarmDelay.at(index);
        if (type == 0 && timeout >= alarmDelay) {
            nodeStatus = 0;
            DbData::NodeInfo_AlarmLimit[index] = true;
        }  else if (type == 1 && timeout >= alarmDelay) {
            nodeStatus = 2;
            DbData::NodeInfo_AlarmUpper[index] = true;
        }

        //处理报警
        if (nodeStatus == 0 || nodeStatus == 2) {
            doAlarm(nodeStatus, positionID, nodeValue, index);
            removeAlarm(positionID, type);
            break;
        }
    }
}

//type 0表示低低报 1表示低报 2表示高报 3表示高高报
void DeviceServer::appendAlarm(const QString &id, quint8 type, float value)
{
    //加入队列延时处理报警
    QMutexLocker lock(&mutex);
    if (!(alarmIDs.contains(id) && alarmTypes.contains(type))) {
        alarmIDs << id;
        alarmTypes << type;
        alarmTimes << QDateTime::currentDateTime();
        alarmValues << value;
        QString info = QString("%1 位号: %2 类型: %3 报警值: %4").arg("加入报警队列").arg(id).arg(type).arg(value);
        emit receiveInfo("报警端口", 255, info);
    }
}

//type 0表示低低报 1表示低报 2表示高报 3表示高高报
void DeviceServer::removeAlarm(const QString &id, quint8 type)
{
    //找到队列中的报警移除掉
    QMutexLocker lock(&mutex);
    int count = alarmIDs.count();
    for (int i = 0; i < count; i++) {
        if (alarmIDs.at(i) == id && alarmTypes.at(i) == type) {
            float value = alarmValues.at(i);
            alarmIDs.removeAt(i);
            alarmTypes.removeAt(i);
            alarmTimes.removeAt(i);
            alarmValues.removeAt(i);
            QString info = QString("%1 位号: %2 类型: %3 报警值: %4").arg("移除报警队列").arg(id).arg(type).arg(value);
            emit receiveInfo("报警端口", 255, info);
            break;
        }
    }
}

void DeviceServer::doReceiveOnline(const QString &portName, quint8 addr, bool online)
{
    //设备采集2由下位机上报变量的在线状态
    if (AppConfig::WorkMode == 3) {
        return;
    }

    //上线立即更新状态并存储数据,下线则清除报警状态
    bool doOnline = false;
    QString deviceName = DbQuery::getDeviceName(portName, addr);
    for (int i = 0; i < DbData::NodeInfo_Count; i++) {
        if (DbData::NodeInfo_DeviceName.at(i) == deviceName) {
            //如果当前是在线状态,突然又来了在线状态,则不处理
            //只有离线转为在线,在线转为离线才需要处理
            if (online && !DbData::NodeInfo_Online.at(i)) {
                QMutexLocker lock(&mutex);
                QString positionID = DbData::NodeInfo_PositionID.at(i);
                positionIDs << positionID;

                if (!doOnline) {
                    DeviceHelper::deviceOnline(deviceName, online);
                    emit receiveOnline(deviceName, online);
                    doOnline = true;
                }

                //这里后期还要考虑一个问题,如果在报警状态又离线了则是否要结束这次报警
                //否则会有一条记录永远存在未结束状态
            } else if (!online && DbData::NodeInfo_Online.at(i)) {
                DbData::NodeInfo_NodeValue[i] = -1;
                DbData::NodeInfo_AlarmUpper[i] = false;
                DbData::NodeInfo_AlarmLimit[i] = false;

                if (!doOnline) {
                    DeviceHelper::deviceOnline(deviceName, online);
                    emit receiveOnline(deviceName, online);
                    doOnline = true;
                }
            }

            //更新在线状态
            DbData::NodeInfo_Online[i] = online;
        }
    }
}

void DeviceServer::doReceiveValue()
{
    for (int i = 0; i < DbData::NodeInfo_Count; i++) {
        QString positionID = DbData::NodeInfo_PositionID.at(i);
        float nodeUpper = DbData::NodeInfo_NodeUpper.at(i);
        float nodeLimit = DbData::NodeInfo_NodeLimit.at(i);
        float nodeMax = DbData::NodeInfo_NodeMax.at(i);
        float nodeMin = DbData::NodeInfo_NodeMin.at(i);
        float nodeRange = DbData::NodeInfo_NodeRange.at(i);
        int dotCount = DbData::NodeInfo_DotCount.at(i);

        //目前收到的值需要经过几层过滤计算才是真实的值
        //第一层是小数点(默认0),比如收到的值是1000,如果小数点设定的1则真实的是100
        //第二层是量程(默认0),比如收到998,如果设定的量程0.25则运算后 998/4000*0.25=0.062375
        //第三层是最大值(默认1000),假设设定的最大值1000,如果收到的值>1000则取1000,因为某些设备损坏或者误报采集到的是一个很大的不准确的值
        //第四层是清零值(默认0),假设设定的是50,则低于50都认为是0,因为某些设备损坏或者误报采集到的是一个很小的不准确的值

        //根据设定的小数点来重新计算真实的值
        float nodeValue = RealTimeDB::instance()->GetDataString(positionID).toFloat();
        //例如温度传感器和变送器检测的是-10－60℃，对应数值是6400－32000,用上述的方程表达为X=70*(Z-6400)/25600-10
        nodeValue = (nodeUpper - nodeLimit) * (nodeValue - nodeMin) / (nodeMax - nodeMin) + nodeLimit;

        if (nodeRange > 0) {
            nodeValue = nodeValue * nodeRange;
        }

        if (dotCount > 0) {
            nodeValue = nodeValue / qPow(10, dotCount);
        }

        //如果收到的值大于最大值则取最大值作为当前的值
        nodeValue = nodeValue > nodeUpper ? nodeUpper : nodeValue;
        //如果收到的值小于最小值则取最小值作为当前的值
        nodeValue = nodeValue < nodeLimit ? nodeLimit : nodeValue;

        //精度过滤,避免精度过大显示太长
        nodeValue = QString::number(nodeValue, 'f', AppConfig::Precision).toFloat();

        //找到当前变量处理变量报警
        //如果当前值小于最小值而且当前不处于下限报警则触发报警
        //如果当前值大于最大值而且当前不处于上限报警则触发报警
        //0-低报 1-低报恢复 2-高报 3-高报恢复
        quint8 nodeStatus = 100;

        //根据设定的不同的报警类型处理,假定上限值100,下限值25
        //HH表示超过25是低报,超过100是高报
        //HL表示低于25是低报,超过100是高报
        //LL表示低于25是高报,低于100是低报
        if(DbData::NodeInfo_AlarmEnable.at(i) == "启用"){
            QString alarmType = DbData::NodeInfo_AlarmType.at(i);
            if (alarmType == "HH") {
                doAlarmHH(nodeStatus, positionID, nodeValue, i);
            } else if (alarmType == "HL") {
                doAlarmHL(nodeStatus, positionID, nodeValue, i);
            } else if (alarmType == "LL") {
                doAlarmLL(nodeStatus, positionID, nodeValue, i);
            }

            //处理报警
            //qDebug() << TIMEMS << positionID << nodeStatus << nodeValue;
            doAlarm(nodeStatus, positionID, nodeValue, i);
        }

        //没有变动的数据则无需处理,不然界面上一下子这么多数据刷新可能导致高CPU,一样的数据也没有必要
        if (DbData::NodeInfo_NodeValue.at(i) != nodeValue) {
            DeviceHelper::deviceValue(positionID, nodeValue);
            DbData::NodeInfo_NodeValue[i] = nodeValue;
        }

        //云端数据同步
        if (AppConfig::UseNetDb) {
            QString sql = QString("update NodeData set NodeValue='%1',SaveTime='%2' where PositionID='%3'").arg(nodeValue).arg(DATETIME).arg(positionID);
            DbData::DbNet->append(sql);
        }
    }
}

void DeviceServer::doAlarmHH(quint8 &nodeStatus, const QString &positionID, float nodeValue, int index)
{
    int alarmDelay = DbData::NodeInfo_AlarmDelay.at(index);

    //未报警情况下的正常值都移除报警队列
    if (nodeValue <= DbData::NodeInfo_AlarmL.at(index)) {
        removeAlarm(positionID, 0);
        removeAlarm(positionID, 1);
    }

    if (nodeValue >= DbData::NodeInfo_AlarmH.at(index)) {
        //大于最大值而且当前不为高报则变成高报
        if (!DbData::NodeInfo_AlarmUpper.at(index)) {
            if (alarmDelay > 0) {
                appendAlarm(positionID, 1, nodeValue);
            } else {
                nodeStatus = 2;
                DbData::NodeInfo_AlarmUpper[index] = true;
                DbData::NodeInfo_AlarmLimit[index] = false;
            }
        }
    } else if (nodeValue < DbData::NodeInfo_AlarmH.at(index)) {
        //先恢复高报
        if (DbData::NodeInfo_AlarmUpper.at(index)) {
            if (alarmDelay > 0) {
                removeAlarm(positionID, 1);
            } else {
                nodeStatus = 3;
                DbData::NodeInfo_AlarmUpper[index] = false;
            }
        }

        //再判断是否在低报范围内
        if (nodeValue >= DbData::NodeInfo_AlarmL.at(index)) {
            //低报范围内而且不为低报状态则变成低报
            if (!DbData::NodeInfo_AlarmLimit.at(index)) {
                if (alarmDelay > 0) {
                    appendAlarm(positionID, 0, nodeValue);
                } else {
                    nodeStatus = 0;
                    DbData::NodeInfo_AlarmLimit[index] = true;
                    DbData::NodeInfo_AlarmUpper[index] = false;
                }
            }
        } else {
            if (DbData::NodeInfo_AlarmLimit.at(index)) {
                if (alarmDelay > 0) {
                    removeAlarm(positionID, 0);
                } else {
                    nodeStatus = 1;
                    DbData::NodeInfo_AlarmLimit[index] = false;
                }
            }
        }
    }
}

void DeviceServer::doAlarmHL(quint8 &nodeStatus, const QString &positionID, float nodeValue, int index)
{
    int alarmDelay = DbData::NodeInfo_AlarmDelay.at(index);

    //未报警情况下的正常值都移除报警队列
    if (nodeValue >= DbData::NodeInfo_AlarmL.at(index) && nodeValue <= DbData::NodeInfo_AlarmH.at(index)) {
        removeAlarm(positionID, 0);
        removeAlarm(positionID, 1);
    }

    if (nodeValue <= DbData::NodeInfo_AlarmL.at(index)) {
        if (!DbData::NodeInfo_AlarmLimit.at(index)) {
            if (alarmDelay > 0) {
                appendAlarm(positionID, 0, nodeValue);
            } else {
                nodeStatus = 0;
                DbData::NodeInfo_AlarmLimit[index] = true;
                DbData::NodeInfo_AlarmUpper[index] = false;
            }
        }
    } else if (nodeValue > DbData::NodeInfo_AlarmL.at(index)) {
        if (DbData::NodeInfo_AlarmLimit.at(index)) {
            if (alarmDelay > 0) {
                removeAlarm(positionID, 0);
            } else {
                nodeStatus = 1;
                DbData::NodeInfo_AlarmLimit[index] = false;
            }
        }
    }

    if (nodeValue >= DbData::NodeInfo_AlarmH.at(index)) {
        if (!DbData::NodeInfo_AlarmUpper.at(index)) {
            if (alarmDelay > 0) {
                appendAlarm(positionID, 1, nodeValue);
            } else {
                nodeStatus = 2;
                DbData::NodeInfo_AlarmUpper[index] = true;
                DbData::NodeInfo_AlarmLimit[index] = false;
            }
        }
    } else if (nodeValue < DbData::NodeInfo_AlarmH.at(index)) {
        if (DbData::NodeInfo_AlarmUpper.at(index)) {
            if (alarmDelay > 0) {
                removeAlarm(positionID, 1);
            } else {
                nodeStatus = 3;
                DbData::NodeInfo_AlarmUpper[index] = false;
            }
        }
    }
}

void DeviceServer::doAlarmLL(quint8 &nodeStatus, const QString &positionID, float nodeValue, int index)
{
    int alarmDelay = DbData::NodeInfo_AlarmDelay.at(index);

    //未报警情况下的正常值都移除报警队列
    if (nodeValue >= DbData::NodeInfo_AlarmH.at(index)) {
        removeAlarm(positionID, 0);
        removeAlarm(positionID, 1);
    }

    //2019-3-4小宋那家伙又说改为:小于下限值是高报,小于上限值是低报
    if (nodeValue <= DbData::NodeInfo_AlarmL.at(index)) {
        if (!DbData::NodeInfo_AlarmUpper.at(index)) {
            if (alarmDelay > 0) {
                appendAlarm(positionID, 1, nodeValue);
            } else {
                nodeStatus = 2;
                DbData::NodeInfo_AlarmUpper[index] = true;
                DbData::NodeInfo_AlarmLimit[index] = false;
            }
        }
    } else if (nodeValue > DbData::NodeInfo_AlarmL.at(index)) {
        //先恢复高报
        if (DbData::NodeInfo_AlarmUpper.at(index)) {
            if (alarmDelay > 0) {
                removeAlarm(positionID, 1);
            } else {
                nodeStatus = 3;
                DbData::NodeInfo_AlarmUpper[index] = false;
            }
        }

        //再判断是否在低报范围内
        if (nodeValue <= DbData::NodeInfo_AlarmH.at(index)) {
            //低报范围内而且不为低报状态则变成低报
            if (!DbData::NodeInfo_AlarmLimit.at(index)) {
                if (alarmDelay > 0) {
                    appendAlarm(positionID, 0, nodeValue);
                } else {
                    nodeStatus = 0;
                    DbData::NodeInfo_AlarmLimit[index] = true;
                    DbData::NodeInfo_AlarmUpper[index] = false;
                }
            }
        } else if (nodeValue > DbData::NodeInfo_AlarmH.at(index)) {
            if (DbData::NodeInfo_AlarmLimit.at(index)) {
                if (alarmDelay > 0) {
                    removeAlarm(positionID, 0);
                } else {
                    nodeStatus = 1;
                    DbData::NodeInfo_AlarmLimit[index] = false;
                }
            }
        }
    }
}

void DeviceServer::doAlarm2(quint8 nodeStatus, const QString &positionID, float nodeValue, int index, const QString &nodeSign)
{
    bool online, alarmLimit, alarmUpper, alarmOther;
    if (AppConfig::WorkMode == 3) {
        //0预热中 1工作中 2低限报警 3高限报警 4传感器故障 7变量离线
        online = (nodeStatus != 7);
        alarmLimit = (nodeStatus == 2);
        alarmUpper = (nodeStatus == 3);
        alarmOther = (nodeStatus == 4);
    } else {
        //数据库采集模式 nodeStatus 0表示离线  1表示在线  2表示低报  3表示高报
        //数据库读取模式 nodeStatus 0表示离线  1表示正常  2表示报警  3表示高报  4表示失效
        online = (nodeStatus >= 1);
        alarmLimit = (nodeStatus == 2);
        alarmUpper = (nodeStatus == 3);
        alarmOther = (nodeStatus == 4);
    }

    //处理上下线
    if (DbData::NodeInfo_Online.at(index) != online) {
        DeviceHelper::deviceOnline2(positionID, online);
        DbData::NodeInfo_Online[index] = online;

        //离线后恢复状态
        if (!online) {
            DbData::NodeInfo_NodeValue[index] = -1;
            DbData::NodeInfo_AlarmUpper[index] = false;
            DbData::NodeInfo_AlarmLimit[index] = false;
        }
    }

    //只有在线的设备才需要继续处理
    if (online) {
        //处理低报
        if (DbData::NodeInfo_AlarmLimit.at(index) != alarmLimit) {
            DbData::NodeInfo_AlarmLimit[index] = alarmLimit;
            DbData::NodeInfo_AlarmUpper[index] = false;
            DbData::NodeInfo_AlarmOther[index] = false;
            doAlarm(alarmLimit ? 0 : 1, positionID, nodeValue, index);
        }

        //处理高报
        if (DbData::NodeInfo_AlarmUpper.at(index) != alarmUpper) {
            DbData::NodeInfo_AlarmUpper[index] = alarmUpper;
            DbData::NodeInfo_AlarmLimit[index] = false;
            DbData::NodeInfo_AlarmOther[index] = false;
            doAlarm(alarmUpper ? 2 : 3, positionID, nodeValue, index);
        }

        //处理其他报警比如失效报警
        if (DbData::NodeInfo_AlarmOther.at(index) != alarmOther) {
            DbData::NodeInfo_AlarmOther[index] = alarmOther;
            DbData::NodeInfo_AlarmUpper[index] = false;
            DbData::NodeInfo_AlarmLimit[index] = false;
            doAlarm(alarmOther ? 5 : 6, positionID, nodeValue, index);
        }
    }

    //没有变动的数据则无需处理,不然界面上一下子这么多数据刷新可能导致高CPU,一样的数据也没有必要
    if (DbData::NodeInfo_NodeValue.at(index) != nodeValue) {
        DeviceHelper::deviceValue(positionID, nodeValue, nodeSign);
        DbData::NodeInfo_NodeValue[index] = nodeValue;
    }

    //云端数据同步
    if (AppConfig::UseNetDb) {
        QString sql = QString("update NodeData set NodeValue='%1',SaveTime='%2' where PositionID='%3'").arg(nodeValue).arg(DATETIME).arg(positionID);
        DbData::DbNet->append(sql);
    }
}

void DeviceServer::doAlarm(quint8 nodeStatus, const QString &positionID, float nodeValue, int index)
{
    QString deviceName = DbData::NodeInfo_DeviceName.at(index);
    QString nodeName = DbData::NodeInfo_NodeName.at(index);
    QString nodeSign = DbData::NodeInfo_NodeSign.at(index);
    QString nodeSound = DbData::NodeInfo_NodeSound.at(index);

    //找出报警对应的字符串
    QString content;
    if (nodeStatus == 0 || nodeStatus == 1) {
        content = AppData::AlarmTypes.at(0);
    } else if (nodeStatus == 2 || nodeStatus == 3) {
        content = AppData::AlarmTypes.at(1);
    } else if (nodeStatus == 5 || nodeStatus == 6) {
        content = AppData::AlarmTypes.at(2);
    }

    if (nodeStatus == 0 || nodeStatus == 2 || nodeStatus == 5) {
        //先恢复上一次的报警 当前是什么报警则恢复的是其余两种报警
        if (nodeStatus == 0) {
            DbQuery::updateAlarmLog(positionID, AppData::AlarmTypes.at(1), DATETIME);
            DbQuery::updateAlarmLog(positionID, AppData::AlarmTypes.at(2), DATETIME);
        } else if (nodeStatus == 2) {
            DbQuery::updateAlarmLog(positionID, AppData::AlarmTypes.at(0), DATETIME);
            DbQuery::updateAlarmLog(positionID, AppData::AlarmTypes.at(2), DATETIME);
        } else if (nodeStatus == 5) {
            DbQuery::updateAlarmLog(positionID, AppData::AlarmTypes.at(0), DATETIME);
            DbQuery::updateAlarmLog(positionID, AppData::AlarmTypes.at(1), DATETIME);
        }

        DeviceHelper::addMsg(positionID, nodeValue);
        DeviceHelper::deviceAlarm(positionID, nodeStatus);
        DbQuery::addAlarmLog(positionID, deviceName, nodeName, nodeValue, nodeSign, content, DATETIME);
        emit receiveAlarm(deviceName, positionID, nodeStatus);

        //加入到报警联动队列
        AlarmLink::Instance()->append(positionID);

        //构建短信转发和邮件转发内容
        QString data = QString("位号: %1  控制器: %2  变量: %3  触发值: %4 %5  类型: %6  时间: %7")
                       .arg(positionID).arg(deviceName).arg(index).arg(nodeValue).arg(nodeSign).arg(content).arg(DATETIME);
        SendServer::Instance()->append(data);

        //启动报警声音
        DeviceHelper::playAlarm(nodeSound);

        //右下角弹框 10000-表示禁用
        if (AppConfig::TipInterval != 10000) {
            QString msg = QString("位    号: %1\n控制器: %2\n变量: %3\n触发值: %4 %5\n类    型: %6")
                          .arg(positionID).arg(deviceName).arg(index).arg(nodeValue).arg(nodeSign).arg(content);
            QUIHelper::showTipBox("报警信息", msg, false, false, AppConfig::TipInterval);
        }
    } else if (nodeStatus == 1 || nodeStatus == 3 || nodeStatus == 6) {
        DeviceHelper::deviceAlarm(positionID, nodeStatus);
        DbQuery::updateAlarmLog(positionID, content, DATETIME);
        emit receiveAlarm(deviceName, positionID, nodeStatus);
        //从报警联动队列中移除
        AlarmLink::Instance()->remove(positionID);
        //停止报警声音
        DeviceHelper::stopSound();
    }
}

DeviceClient *DeviceServer::getDeviceClient(const QString &portName)
{
    DeviceClient *device = 0;
    foreach (DeviceClient *deviceClient, deviceClients) {
        if (deviceClient->getPortName() == portName) {
            device = deviceClient;
            break;
        }
    }

    return device;
}

void DeviceServer::init()
{
    //判断是从数据库采集数据还是设备采集数据
    if (AppConfig::WorkMode == 1 || AppConfig::WorkMode == 2) {
        DbReceive::Instance()->start();
        return;
    }
    //加载设备采集数据
    Load();
    //初始化报警联动
    connect(AlarmLink::Instance(), SIGNAL(sendData(QString, quint8, QByteArray)),
            this, SIGNAL(sendData(QString, quint8, QByteArray)));
    connect(AlarmLink::Instance(), SIGNAL(receiveData(QString, quint8, QByteArray)),
            this, SIGNAL(receiveData(QString, quint8, QByteArray)));
    connect(AlarmLink::Instance(), SIGNAL(receiveInfo(QString, quint8, QString)),
            this, SIGNAL(receiveInfo(QString, quint8, QString)));
    connect(AlarmLink::Instance(), SIGNAL(receiveError(QString, quint8, QString)),
            this, SIGNAL(receiveError(QString, quint8, QString)));
    AlarmLink::Instance()->init();
}

bool DeviceServer::Load()
{
    LogInfo(QString("load devices!"));
    m_VendorList.clear();

    for (int i = 0; i < DbData::PortInfo_Count; i++) {
        QString portName = DbData::PortInfo_PortName.at(i);
        //取出端口名称,从设备表中找到该端口对应的地址即可
        QList<quint8> addrs;
        QString portType = DbData::PortInfo_PortType.at(i);
        QString protocol = DbData::PortInfo_Protocol.at(i);
        QString comName = DbData::PortInfo_ComName.at(i);
        LogInfo(QString("PortType is %1").arg(portType.toUpper()));

        Vendor *pVendorObj = new Vendor(this);
        connect(pVendorObj, SIGNAL(receiveOnline(QString, quint8, bool)),
                this, SLOT(doReceiveOnline(QString, quint8, bool)));

        IVendorPlugin *pVendorPluginObj = VendorPluginManager::getInstance()->getPlugin(protocol);
        if(pVendorPluginObj != Q_NULLPTR) {
            pVendorObj->m_pVendorPluginObj = pVendorPluginObj;
        }
        m_VendorList.append(pVendorObj);

        if(portType == "COM") {
            ComPort* pComPortObj = new ComPort();
            pVendorObj->m_pPortObj = pComPortObj;
            ComDevicePrivate *pComDevicePrivateObj = new ComDevicePrivate();
            if (pComDevicePrivateObj->LoadData(i)) {
                QStringList comArgs;
                comArgs << QString().number(pComDevicePrivateObj->m_iBaudrate);
                comArgs << QString().number(pComDevicePrivateObj->m_iDatabit);
                comArgs << pComDevicePrivateObj->m_sVerifybit;
                comArgs << QString().number(pComDevicePrivateObj->m_fStopbit);

                if(!pComPortObj->open(pComDevicePrivateObj->m_sPortNumber, comArgs)) {
                    LogError("ComPort open fail!");
                }
            }
            pVendorObj->m_pVendorPrivateObj = pComDevicePrivateObj;
        } else if(portType == "NET") {
            NetDevicePrivate* pNetDevicePrivateObj = new NetDevicePrivate();
            if (pNetDevicePrivateObj->LoadData(i)) {
                QStringList netArgs;
                netArgs << pNetDevicePrivateObj->m_sIpAddress;
                netArgs << QString().number(pNetDevicePrivateObj->m_iPort);
                NetPort* pNetPortObj = new NetPort(pNetDevicePrivateObj->m_sIpAddress, pNetDevicePrivateObj->m_iPort);
                pVendorObj->m_pPortObj = pNetPortObj;
                if(!pNetPortObj->open("Net", netArgs)) {
                    Log4Qt::Logger::logger("Run_Logger")->error("NetPort open fail!");
                    LogError("NetPort open fail!");
                }
            }
            pVendorObj->m_pVendorPrivateObj = pNetDevicePrivateObj;
        }
        else{
            continue;
        }
    }


    for (int j = 0; j < DbData::DeviceInfo_Count; j++) {
        //-----------------设备变量------------------//
        for (int k = 0; k < DbData::NodeInfo_Count; k++) {
            if (DbData::DeviceInfo_DeviceName.at(j) == DbData::NodeInfo_DeviceName.at(k)) {
                QString devVarTabName = DbData::DeviceInfo_DeviceName.at(j);
                QString logStr = QString("设备变量-DevVarList(devVarTabName:%1, size:%2)")
                        .arg(devVarTabName)
                        .arg(DbData::NodeInfo_Count);
                LogInfo(logStr);

                IoDataTag *pIoDataTag = new IoDataTag();

                pIoDataTag->mId = DbData::NodeInfo_PositionID.at(k);
                pIoDataTag->mName = DbData::NodeInfo_NodeName.at(k);
                pIoDataTag->mDescription = DbData::NodeInfo_PositionID.at(k);
                pIoDataTag->mUnit = DbData::NodeInfo_NodeSign.at(k);
                pIoDataTag->mProjectConverter = "";
                pIoDataTag->mComments = "";

                //-------------IO连接-----------------//
                pIoDataTag->mDeviceName = devVarTabName;// 设备名称
                pIoDataTag->mDescription = DbData::NodeInfo_NodeDescription.at(k);

                if(DbData::NodeInfo_PermissionType.at(k) == "只读")
                {
                    pIoDataTag->mPermissionType = READ;
                }
                else if(DbData::NodeInfo_PermissionType.at(k) == "只写")
                {
                    pIoDataTag->mPermissionType = WRIE;
                }
                else if(DbData::NodeInfo_PermissionType.at(k) == "读写")
                {
                    pIoDataTag->mPermissionType = READ_WRIE;
                }
                else
                {
                    pIoDataTag->mPermissionType = READ;
                }

                pIoDataTag->mDeviceAddress = DbData::DeviceInfo_DeviceAddr.at(j);

                pIoDataTag->mMaxValue = DbData::NodeInfo_NodeUpper.at(k);

                pIoDataTag->mRegisterArea = DbData::NodeInfo_RegisterArea.at(k);

                pIoDataTag->mMinValue = DbData::NodeInfo_NodeLimit.at(k);

                pIoDataTag->mRegisterAddress = DbData::NodeInfo_NodeAddr.at(k);

                pIoDataTag->mInitializeValue = 0;

                QString tmp = DbData::NodeInfo_DataType.at(k);
                if(tmp == "Bit1开关量") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_BOOL;
                    pIoDataTag->mLength = 1;
                } else if(tmp == "Char8位有符号数") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_INT8;
                    pIoDataTag->mLength = 1;
                } else if(tmp == "Byte8位无符号数") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_UINT8;
                    pIoDataTag->mLength = 1;
                } else if(tmp == "Short16位有符号数") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_INT16;
                    pIoDataTag->mLength = 2;
                } else if(tmp == "Word16位无符号数") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_UINT16;
                    pIoDataTag->mLength = 2;
                } else if(tmp == "ASCII2个字符") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_ASCII2CHAR;
                    pIoDataTag->mLength = 2;
                } else if(tmp == "Long32位有符号数") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_INT32;
                    pIoDataTag->mLength = 4;
                } else if(tmp == "Dword32位无符号数") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_UINT32;
                    pIoDataTag->mLength = 4;
                } else if(tmp == "Float单精度浮点数") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_FLOAT;
                    pIoDataTag->mLength = 4;
                } else if(tmp == "String字符串") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_STRING;
                    pIoDataTag->mLength = 256;
                } else if(tmp == "Double双精度浮点数") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_DOUBLE;
                    pIoDataTag->mLength = 8;
                } else if(tmp == "BCD") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_BCD;
                    pIoDataTag->mLength = 8;
                } else if(tmp == "LongLong64位有符号数") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_INT64;
                    pIoDataTag->mLength = 8;
                } else if(tmp == "DwordDword64位无符号数") {
                    pIoDataTag->mType = pIoDataTag->mDataType = TYPE_UINT64;
                    pIoDataTag->mLength = 8;
                }

                pIoDataTag->mScale = 1;
                pIoDataTag->mOffset = 0;
                pIoDataTag->m_Function.LoadFuncObjects(pIoDataTag->mProjectConverter);

                PDBTagObject pEmptyDBTagObj = RealTimeDB::instance()->getEmptyDBTagObject();
                DBTagObject* pDBIoTagObject = new DBTagObject(pEmptyDBTagObj,
                                                              pIoDataTag->mId,
                                                              pIoDataTag->mType,
                                                              READ_WRIE,
                                                              pIoDataTag->mLength,
                                                              TYPE_IO,
                                                              pIoDataTag);

                RealTimeDB::instance()->rtdb.insert(pIoDataTag->mId, pDBIoTagObject);
                RealTimeDB::instance()->varNameMapId.insert((QObject::tr("设备变量.") + pIoDataTag->mName + "[" + pIoDataTag->mId + "]"), pIoDataTag->mId);

                IOTag *pIOTag = new IOTag();
                pIOTag->SetTagID(pIoDataTag->mId);
                pIOTag->SetDeviceName(pIoDataTag->mDeviceName);
                pIOTag->SetPermissionType(pIoDataTag->mPermissionType);
                pIOTag->SetDeviceAddress(pIoDataTag->mDeviceAddress);
                pIOTag->SetRegisterArea(pIoDataTag->mRegisterArea);
                pIOTag->SetRegisterAddress(pIoDataTag->mRegisterAddress);
                pIOTag->SetDataType(pIoDataTag->mDataType);
                pIOTag->SetOffset(pIoDataTag->mOffset);
                pIOTag->SetMaxValue(pIoDataTag->mMaxValue);
                pIOTag->SetMinValue(pIoDataTag->mMinValue);
                pIOTag->SetInitializeValue(pIoDataTag->mInitializeValue);
                pIOTag->SetScale(pIoDataTag->mScale);
                int bufLen = (pIoDataTag->mLength > 8) ? pIoDataTag->mLength : 8;
                pIOTag->SetTagBufferLength(bufLen);
                pIOTag->SetDBTagObject(pDBIoTagObject);

                Vendor *pVendor = FindVendor(pIoDataTag->mDeviceName);
                if(pVendor != Q_NULLPTR) {
                    pVendor->addIOTagToDeviceTagList(pIOTag);
                    pDBIoTagObject->m_pVendor = pVendor;
                }

                pDBIoTagObject->addListener(new DBTagObject_Event_Listener());
            }
        }
    }

    /////////////////////////////////////////////
    // 查找已使用的端口名称并添加至列表
    foreach (Vendor *pVendor, m_VendorList) {
        AddPortName(pVendor->getPortName());
    }

    RealTimeDB::instance()->debugShowNameMapId();
    return true;
}

bool DeviceServer::Unload()
{
    qDeleteAll(m_VendorList);
    m_VendorList.clear();

    RealTimeDB::instance()->rtdb.clear();

    qDeleteAll(m_listPortThread);
    m_listPortThread.clear();

    return true;
}

void DeviceServer::AddPortName(const QString name)
{
    foreach (QString port, m_listPortName) {
        if(name == port)
            return;
    }
    LogInfo(QString("PortName:%1").arg(name));
    m_listPortName.append(name);
}

Vendor *DeviceServer::FindVendor(const QString name)
{
    Vendor *ret = Q_NULLPTR;
    for (int i = 0; i < m_VendorList.size(); ++i) {
        ret = m_VendorList.at(i);
        if (ret->getDeviceName() == name)
            break;
    }
    return ret;
}

void DeviceServer::start()
{
    foreach (DeviceClient *deviceClient, deviceClients) {
        deviceClient->start();
    }

    DbQuery::addUserLog("设备上报", "端口服务启动");
    AlarmLink::Instance()->start();

    foreach (QString name, m_listPortName) {
        PortThread *pPortThread = new PortThread(name);
        foreach (Vendor *pVendor, m_VendorList) {
            if(name == pVendor->getPortName())
                pPortThread->AddVendor(pVendor);
        }
        m_listPortThread.append(pPortThread);
    }

    foreach (PortThread *pPortThread, m_listPortThread) {
        pPortThread->Start();
    }
}

void DeviceServer::stop()
{
    foreach (DeviceClient *deviceClient, deviceClients) {
        deviceClient->stop();
    }

    DbQuery::addUserLog("设备上报", "端口服务停止");
    AlarmLink::Instance()->stop();

    foreach (PortThread *pPortThread, m_listPortThread)
    {
        pPortThread->Stop();
    }
}

void DeviceServer::readValueAll()
{
    foreach (DeviceClient *deviceClient, deviceClients) {
        deviceClient->readValueAll();
    }
}

void DeviceServer::readValue(const QString &portName, quint8 addr, bool all)
{
    //设备采集模式才有此功能,如果没有添加端口查不到端口号也不需要处理
    if (AppConfig::WorkMode == 1 || AppConfig::WorkMode == 2 || portName.isEmpty()) {
        return;
    }

    if (all) {
        getDeviceClient(portName)->readValueAll();
    } else {
        getDeviceClient(portName)->readValue(addr);
    }
}

void DeviceServer::writeData(const QString &portName, const QString &type, quint8 addr, const QByteArray &body)
{
    //这里需要过滤下,在设备采集以外的模式可能调用此函数
    DeviceClient *client = getDeviceClient(portName);
    if (client != 0) {
        //改成了追加命令,这样防止冲突,比如正在回复数据的时候触发报警联动
        client->append(type, addr, body);
    }
}
