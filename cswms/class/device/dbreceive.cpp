#include "dbreceive.h"
#include "quihelper.h"
#include "devicehelper.h"
#include "deviceserver.h"

QScopedPointer<DbReceive> DbReceive::self;
DbReceive *DbReceive::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new DbReceive);
        }
    }

    return self.data();
}

DbReceive::DbReceive(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkData()));
    timer->setInterval(1000);
}

void DbReceive::start()
{
    timer->start();
}

void DbReceive::stop()
{
    timer->stop();
}

void DbReceive::checkData()
{
    QMutexLocker locker(&mutex);

    //实时读取临时表数据,另外的程序负责实时更新该表数据
    //PositionID    位置编号,也可以当做设备编号唯一标识一个设备
    //NodeValue     设备节点的值
    //NodeStatus    设备节点的状态,比如1,2,3,4 四个数字分别代表正常/报警/高报/失效
    //SaveTime      设备的记录时间
    QString sql = "select PositionID,NodeValue,NodeStatus,SaveTime from NodeData";
    QSqlQuery query;
    if (!query.exec(sql)) {
        return;
    }

    QStringList list1, list2, list3, list4;
    while (query.next()) {
        QString value1 = query.value(0).toString();
        QString value2 = query.value(1).toString();
        QString value3 = query.value(2).toString();
        QString value4 = query.value(3).toString();

        list1 << value1;
        list2 << value2;
        list3 << value3;
        list4 << value4;

        //过滤不存在的数据
        if (value1.isEmpty() || value2.isEmpty() || value3.isEmpty() || value4.isEmpty()) {
            emit debug("返回数据不正确");
            continue;
        }

        QString positionID = value1;
        float nodeValue = value2.toFloat();
        int nodeStatus = value3.toInt();
        QString dateTime = value4;
        QDateTime saveTime = QDateTime::fromString(dateTime, "yyyy-MM-dd HH:mm:ss");
        emit receiveValue(positionID, nodeValue);

        //判断状态是否变化,发生变化则做对应的处理
        int index = DbData::NodeInfo_PositionID.indexOf(positionID);
        if (index >= 0) {
            DeviceServer::Instance()->doAlarm2(nodeStatus, positionID, nodeValue, index);
        }

        //云端数据同步
        if (AppConfig::UseNetDb) {
            QString sql = QString("update NodeData set NodeValue='%1',NodeStatus='%2',SaveTime='%3' where PositionID='%4'")
                          .arg(nodeValue).arg(nodeStatus).arg(dateTime).arg(positionID);
            DbData::DbNet->append(sql);
        }
    }

    //合并成一个信号发送减少压力
    QString msg = QString("位号: %1  值: %2  状态: %3  时间: %4")
                  .arg(list1.join(",")).arg(list2.join(",")).arg(list3.join(",")).arg(list4.join(","));
    emit debug(msg);
}

void DbReceive::setInterval(int interval)
{
    if (interval >= 1000) {
        timer->setInterval(interval);
    }
}
