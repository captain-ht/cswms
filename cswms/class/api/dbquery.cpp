#include "dbquery.h"
#include "quihelper.h"

bool DbQuery::loadfromdb = true;

void DbQuery::loadUserInfo()
{
    DbData::UserInfo_Count = 0;
    DbData::UserInfo_UserName.clear();
    DbData::UserInfo_UserPwd.clear();
    DbData::UserInfo_UserType.clear();
    DbData::UserInfo_Permission1.clear();
    DbData::UserInfo_Permission2.clear();
    DbData::UserInfo_Permission3.clear();
    DbData::UserInfo_Permission4.clear();
    DbData::UserInfo_Permission5.clear();
    DbData::UserInfo_Permission6.clear();
    DbData::UserInfo_Permission7.clear();

    QString sql = "select UserName,UserPwd,UserType,Permission1,Permission2,Permission3,Permission4,Permission5,Permission6,Permission7 from UserInfo";
    QSqlQuery query;
    if (!query.exec(sql)) {
        return;
    }

    while (query.next()) {
        QString userName = query.value(0).toString();
        QString userPwd = query.value(1).toString();
        QString userType = query.value(2).toString();
        QString permission1 = query.value(3).toString();
        QString permission2 = query.value(4).toString();
        QString permission3 = query.value(5).toString();
        QString permission4 = query.value(6).toString();
        QString permission5 = query.value(7).toString();
        QString permission6 = query.value(8).toString();
        QString permission7 = query.value(9).toString();

        DbData::UserInfo_Count++;
        DbData::UserInfo_UserName << userName;
        DbData::UserInfo_UserPwd << userPwd;
        DbData::UserInfo_UserType << userType;
        DbData::UserInfo_Permission1 << permission1;
        DbData::UserInfo_Permission2 << permission2;
        DbData::UserInfo_Permission3 << permission3;
        DbData::UserInfo_Permission4 << permission4;
        DbData::UserInfo_Permission5 << permission5;
        DbData::UserInfo_Permission6 << permission6;
        DbData::UserInfo_Permission7 << permission7;
    }
}

void DbQuery::clearUserInfo()
{
    QString sql = "delete from UserInfo where UserName != 'admin'";
    DbHelper::execSql(sql);
}

void DbQuery::updateUserInfo(const QString &userName, const QString &userPwd)
{
    QString sql = QString("update UserInfo set UserPwd='%1' where UserName='%2'").arg(userPwd).arg(userName);
    DbHelper::execSql(sql);
}

void DbQuery::getUserInfo(const QString &userName, QString &userPwd, QString &userType, QList<bool> &permission)
{
    QString temp = "Permission1,Permission2,Permission3,Permission4,Permission5,Permission6,Permission7";
    QString sql = QString("select UserPwd,UserType,%1 from UserInfo where UserName='%2'").arg(temp).arg(userName);
    QSqlQuery query;
    if (!query.exec(sql)) {
        return;
    }

    if (query.next()) {
        userPwd = query.value(0).toString();
        userType = query.value(1).toString();
        for (int i = 0; i < 7; ++i) {
            permission[i] = (query.value(i + 2).toString() == "启用");
        }
    }

    //qDebug() << TIMEMS << AppData::CurrentUserName << AppData::CurrentUserPwd << AppData::CurrentUserType << AppData::UserPermission;
}

void DbQuery::loadPortInfo()
{
    DbData::PortInfo_Count = 0;
    DbData::PortInfo_PortID.clear();
    DbData::PortInfo_PortName.clear();
    DbData::PortInfo_PortType.clear();
    DbData::PortInfo_Protocol.clear();
    DbData::PortInfo_ComName.clear();
    DbData::PortInfo_BaudRate.clear();
    DbData::PortInfo_TcpIP.clear();
    DbData::PortInfo_TcpPort.clear();
    DbData::PortInfo_ReadInterval.clear();
    DbData::PortInfo_ReadTimeout.clear();
    DbData::PortInfo_ReadMaxtime.clear();

    QString sql = "select PortID,PortName,PortType,Protocol,ComName,BaudRate,TcpIP,TcpPort,ReadInterval,ReadTimeout,ReadMaxtime from PortInfo order by PortID asc";
    QSqlQuery query;
    if (!query.exec(sql)) {
        return;
    }

    while (query.next()) {
        int portID = query.value(0).toInt();
        QString portName = query.value(1).toString();
        QString portType = query.value(2).toString();
        QString protocol = query.value(3).toString();
        QString comName = query.value(4).toString();
        int baudRate = query.value(5).toInt();
        QString tcpIP = query.value(6).toString();
        int tcpPort = query.value(7).toInt();
        float readInterval = query.value(8).toFloat();
        int readTimeout = query.value(9).toInt();
        int readMaxtime = query.value(10).toInt();

        //限定超时次数不能小于1
        readTimeout = readTimeout <= 1 ? 1 : readTimeout;

        DbData::PortInfo_Count++;
        DbData::PortInfo_PortID.append(portID);
        DbData::PortInfo_PortName.append(portName);
        DbData::PortInfo_PortType.append(portType);
        DbData::PortInfo_Protocol.append(protocol);
        DbData::PortInfo_ComName.append(comName);
        DbData::PortInfo_BaudRate.append(baudRate);
        DbData::PortInfo_TcpIP.append(tcpIP);
        DbData::PortInfo_TcpPort.append(tcpPort);
        DbData::PortInfo_ReadInterval.append(readInterval);
        DbData::PortInfo_ReadTimeout.append(readTimeout);
        DbData::PortInfo_ReadMaxtime.append(readMaxtime);
    }
}

void DbQuery::clearPortInfo()
{
    QString sql = "delete from PortInfo";
    DbHelper::execSql(sql);
}

void DbQuery::loadDeviceInfo()
{
    DbData::DeviceInfo_Count = 0;
    DbData::DeviceInfo_DeviceID.clear();
    DbData::DeviceInfo_PortName.clear();
    DbData::DeviceInfo_DeviceName.clear();
    DbData::DeviceInfo_DeviceAddr.clear();
    DbData::DeviceInfo_DeviceType.clear();
    DbData::DeviceInfo_NodeNumber.clear();

    QString sql = "select DeviceID,PortName,DeviceName,DeviceAddr,DeviceType,NodeNumber from DeviceInfo order by DeviceID asc";
    QSqlQuery query;
    if (!query.exec(sql)) {
        return;
    }

    while (query.next()) {
        int deviceID = query.value(0).toInt();
        QString portName = query.value(1).toString();
        QString deviceName = query.value(2).toString();
        quint8 deviceAddr = query.value(3).toInt();
        QString deviceType = query.value(4).toString();
        int nodeNumber = query.value(5).toInt();

        DbData::DeviceInfo_Count++;
        DbData::DeviceInfo_DeviceID.append(deviceID);
        DbData::DeviceInfo_PortName.append(portName);
        DbData::DeviceInfo_DeviceName.append(deviceName);
        DbData::DeviceInfo_DeviceAddr.append(deviceAddr);
        DbData::DeviceInfo_DeviceType.append(deviceType);
        DbData::DeviceInfo_NodeNumber.append(nodeNumber);
    }
}

void DbQuery::clearDeviceInfo()
{
    QString sql = "delete from DeviceInfo";
    DbHelper::execSql(sql);
}

void DbQuery::deleteDeviceInfo(const QString &portName)
{
    QString sql = QString("delete from DeviceInfo where PortName='%1'").arg(portName);
    DbHelper::execSql(sql);
}

QString DbQuery::getPortName(const QString &deviceName)
{
    QString portName;
    for (int i = 0; i < DbData::DeviceInfo_Count; i++) {
        if (DbData::DeviceInfo_DeviceName.at(i) == deviceName) {
            portName = DbData::DeviceInfo_PortName.at(i);
            break;
        }
    }

    return portName;
}

QString DbQuery::getDeviceName(const QString &portName, quint8 addr)
{
    QString deviceName;
    for (int i = 0; i < DbData::DeviceInfo_Count; i++) {
        if (DbData::DeviceInfo_PortName.at(i) == portName) {
            if (DbData::DeviceInfo_DeviceAddr.at(i) == addr) {
                deviceName = DbData::DeviceInfo_DeviceName.at(i);
                break;
            }
        }
    }

    return deviceName;
}

void DbQuery::getDeviceInfo(const QString &portName, quint8 addr, QString &deviceName, QString &deviceType)
{
    for (int i = 0; i < DbData::DeviceInfo_Count; i++) {
        if (DbData::DeviceInfo_PortName.at(i) == portName) {
            if (DbData::DeviceInfo_DeviceAddr.at(i) == addr) {
                deviceName = DbData::DeviceInfo_DeviceName.at(i);
                deviceType = DbData::DeviceInfo_DeviceType.at(i);
                break;
            }
        }
    }
}

int DbQuery::getNodeNumber(const QString &portName, quint8 addr)
{
    int nodeNumber = 0;
    for (int i = 0; i < DbData::DeviceInfo_Count; i++) {
        if (DbData::DeviceInfo_PortName.at(i) == portName) {
            if (DbData::DeviceInfo_DeviceAddr.at(i) == addr) {
                nodeNumber = DbData::DeviceInfo_NodeNumber.at(i);
                break;
            }
        }
    }

    return nodeNumber;
}

quint16 DbQuery::getNodeMinAddr(const QString &portName, quint8 addr)
{
    quint16 nodeMinAddr = 65535;
    for (int i = 0; i < DbData::DeviceInfo_Count; i++) {
        if (DbData::DeviceInfo_PortName.at(i) == portName) {
            if (DbData::DeviceInfo_DeviceAddr.at(i) == addr) {
                QString deviceName = DbData::DeviceInfo_DeviceName.at(i);
                //再从变量表中找该控制器对应的变量的最小的地址
                for (int j = 0; j < DbData::NodeInfo_Count; j++) {
                    if (deviceName == DbData::NodeInfo_DeviceName.at(j)) {
                        quint16 nodeAddr = DbData::NodeInfo_NodeAddr.at(j);
                        if (nodeAddr < nodeMinAddr) {
                            nodeMinAddr = nodeAddr;
                        }
                    }
                }

                break;
            }
        }
    }

    return nodeMinAddr - 1;
}

void DbQuery::loadNodeInfo()
{
    //先记住之前的值 Online NodeValue AlarmUpper AlarmLimit AlarmOther SaveTime
    //防止在动态改变变量信息以后误报,例如以前处于报警,重新保存的时候不报警,此时重新采集数据又是报警,会认为是新的报警
    QList<QString> NodeInfo_PositionID;
    QList<bool> NodeInfo_Online;
    QList<bool> NodeInfo_AlarmUpper;
    QList<bool> NodeInfo_AlarmLimit;
    QList<bool> NodeInfo_AlarmOther;
    QList<float> NodeInfo_NodeValue;
    QList<QDateTime> NodeInfo_SaveTime;

    NodeInfo_PositionID = DbData::NodeInfo_PositionID;
    NodeInfo_Online = DbData::NodeInfo_Online;
    NodeInfo_AlarmUpper = DbData::NodeInfo_AlarmUpper;
    NodeInfo_AlarmLimit = DbData::NodeInfo_AlarmLimit;
    NodeInfo_AlarmOther = DbData::NodeInfo_AlarmOther;
    NodeInfo_NodeValue = DbData::NodeInfo_NodeValue;
    NodeInfo_SaveTime = DbData::NodeInfo_SaveTime;

    DbData::NodeInfo_Count = 0;
    DbData::NodeInfo_NodeID.clear();
    DbData::NodeInfo_PositionID.clear();
    DbData::NodeInfo_DeviceName.clear();
    DbData::NodeInfo_NodeName.clear();
    DbData::NodeInfo_NodeDescription.clear();
    DbData::NodeInfo_PermissionType.clear();
    DbData::NodeInfo_RegisterArea.clear();
    DbData::NodeInfo_DataType.clear();
    DbData::NodeInfo_NodeAddr.clear();
    DbData::NodeInfo_NodeType.clear();
    DbData::NodeInfo_NodeClass.clear();
    DbData::NodeInfo_NodeSign.clear();
    DbData::NodeInfo_NodeUpper.clear();
    DbData::NodeInfo_NodeLimit.clear();
    DbData::NodeInfo_NodeMax.clear();
    DbData::NodeInfo_NodeMin.clear();
    DbData::NodeInfo_NodeRange.clear();
    DbData::NodeInfo_NodeEnable.clear();
    DbData::NodeInfo_NodeSound.clear();
    DbData::NodeInfo_NodeImage.clear();
    DbData::NodeInfo_SaveInterval.clear();
    DbData::NodeInfo_DotCount.clear();
    DbData::NodeInfo_AlarmDelay.clear();
    DbData::NodeInfo_AlarmType.clear();
    DbData::NodeInfo_AlarmH.clear();
    DbData::NodeInfo_AlarmL.clear();
    DbData::NodeInfo_NodeX.clear();
    DbData::NodeInfo_NodeY.clear();

    DbData::NodeInfo_Online.clear();
    DbData::NodeInfo_AlarmUpper.clear();
    DbData::NodeInfo_AlarmLimit.clear();
    DbData::NodeInfo_AlarmOther.clear();
    DbData::NodeInfo_NodeValue.clear();
    DbData::NodeInfo_SaveTime.clear();

    //根据气种升序+位号升序排列
    QString sql = "select NodeID,PositionID,DeviceName,NodeName,NodeDescription,PermissionType,DataType,RegisterArea,NodeAddr,NodeType,NodeClass,NodeSign,NodeUpper,NodeLimit,NodeMax,NodeMin,NodeRange,"
                  "NodeEnable,NodeSound,NodeImage,SaveInterval,DotCount,AlarmEnable,AlarmDelay,AlarmType,AlarmH,AlarmL,NodeX,NodeY from NodeInfo order by NodeClass asc,NodeID asc";
    QSqlQuery query;
    if (!query.exec(sql)) {
        return;
    }

    while (query.next()) {
        int nodeID = query.value(0).toInt();
        QString positionID = query.value(1).toString();
        QString deviceName = query.value(2).toString();
        QString nodeName = query.value(3).toString();
        QString nodeDescription = query.value(4).toString();
        QString permissionType = query.value(5).toString();
        QString dataType = query.value(6).toString();
        QString registerArea = query.value(7).toString();
        quint16 nodeAddr = query.value(8).toInt();
        QString nodeType = query.value(9).toString();
        QString nodeClass = query.value(10).toString();
        QString nodeSign = query.value(11).toString();
        float nodeUpper = query.value(12).toFloat();
        float nodeLimit = query.value(13).toFloat();
        float nodeMax = query.value(14).toFloat();
        float nodeMin = query.value(15).toFloat();
        float nodeRange = query.value(16).toFloat();
        QString nodeEnable = query.value(17).toString();
        QString nodeSound = query.value(18).toString();
        QString nodeImage = query.value(19).toString();
        int saveInterval = query.value(20).toInt();
        int dotCount = query.value(21).toInt();
        QString alarmEnable = query.value(22).toString();
        int alarmDelay = query.value(23).toInt();
        QString alarmType = query.value(24).toString();
        float alarmH = query.value(25).toFloat();
        float alarmL = query.value(26).toFloat();
        int nodeX = query.value(27).toInt();
        int nodeY = query.value(28).toInt();


        //过滤禁用的变量
        if (nodeEnable == "禁用") {
            continue;
        }

        DbData::NodeInfo_Count++;
        DbData::NodeInfo_NodeID.append(nodeID);
        DbData::NodeInfo_PositionID.append(positionID);
        DbData::NodeInfo_DeviceName.append(deviceName);
        DbData::NodeInfo_NodeName.append(nodeName);
        DbData::NodeInfo_NodeDescription.append(nodeDescription);
        DbData::NodeInfo_PermissionType.append(permissionType);
        DbData::NodeInfo_DataType.append(dataType);
        DbData::NodeInfo_RegisterArea.append(registerArea);
        DbData::NodeInfo_NodeAddr.append(nodeAddr);
        DbData::NodeInfo_NodeType.append(nodeType);
        DbData::NodeInfo_NodeClass.append(nodeClass);
        DbData::NodeInfo_NodeSign.append(nodeSign);
        DbData::NodeInfo_NodeUpper.append(nodeUpper);
        DbData::NodeInfo_NodeLimit.append(nodeLimit);
        DbData::NodeInfo_NodeMax.append(nodeMax);
        DbData::NodeInfo_NodeMin.append(nodeMin);
        DbData::NodeInfo_NodeRange.append(nodeRange);
        DbData::NodeInfo_NodeEnable.append(nodeEnable);
        DbData::NodeInfo_NodeSound.append(nodeSound);
        DbData::NodeInfo_NodeImage.append(nodeImage);
        DbData::NodeInfo_SaveInterval.append(saveInterval);
        DbData::NodeInfo_DotCount.append(dotCount);
        DbData::NodeInfo_AlarmEnable.append(alarmEnable);
        DbData::NodeInfo_AlarmDelay.append(alarmDelay);
        DbData::NodeInfo_AlarmType.append(alarmType);
        DbData::NodeInfo_AlarmH.append(alarmH);
        DbData::NodeInfo_AlarmL.append(alarmL);
        DbData::NodeInfo_NodeX.append(nodeX);
        DbData::NodeInfo_NodeY.append(nodeY);

        DbData::NodeInfo_Online.append(false);
        DbData::NodeInfo_AlarmUpper.append(false);
        DbData::NodeInfo_AlarmLimit.append(false);
        DbData::NodeInfo_AlarmOther.append(false);
        DbData::NodeInfo_NodeValue.append(-1);
        DbData::NodeInfo_SaveTime.append(QDateTime::currentDateTime());
    }

    //重新更新值和状态等
    for (int i = 0; i < DbData::NodeInfo_Count; i++) {
        QString positionID = DbData::NodeInfo_PositionID.at(i);
        int index = NodeInfo_PositionID.indexOf(positionID);
        if (index >= 0) {
            DbData::NodeInfo_Online[i] = NodeInfo_Online.at(index);
            DbData::NodeInfo_AlarmUpper[i] = NodeInfo_AlarmUpper.at(index);
            DbData::NodeInfo_AlarmLimit[i] = NodeInfo_AlarmLimit.at(index);
            DbData::NodeInfo_AlarmOther[i] = NodeInfo_AlarmOther.at(index);
            DbData::NodeInfo_NodeValue[i] = NodeInfo_NodeValue.at(index);
            DbData::NodeInfo_SaveTime[i] = NodeInfo_SaveTime.at(index);
        }
    }
}

void DbQuery::clearNodeInfo()
{
    QString sql = "delete from NodeInfo";
    DbHelper::execSql(sql);
}

void DbQuery::deleteNodeInfo(const QString &deviceName)
{
    QString sql = QString("delete from NodeInfo where DeviceName='%1'").arg(deviceName);
    DbHelper::execSql(sql);
}

void DbQuery::addNodeLog(const QString &positionID, const QString &deviceName,
                         const QString &nodeName, float nodeValue,
                         const QString &nodeSign, const QString &saveTime)
{
    DbData::NodeLogID++;
    QString sql = "insert into NodeLog(LogID,PositionID,DeviceName,NodeName,NodeValue,NodeSign,SaveTime) values('";
    sql += QString::number(DbData::NodeLogID) + "','";
    sql += positionID + "','";
    sql += deviceName + "','";
    sql += nodeName + "','";
    sql += QString::number(nodeValue) + "','";
    sql += nodeSign + "','";
    sql += saveTime + "')";
    DbHelper::execSql(sql);
}

void DbQuery::deleteNodeLog(const QString &timeStart, const QString &timeEnd)
{
    QString sql = "delete from NodeLog where 1=1";
    if (AppConfig::LocalDbType.toUpper() == "SQLITE") {
        sql += " and datetime(SaveTime)>='" + timeStart + "'";
        sql += " and datetime(SaveTime)<='" + timeEnd + "'";
    } else if (AppConfig::LocalDbType.toUpper() == "MYSQL") {
        sql += " and unix_timestamp(SaveTime)>=unix_timestamp('" + timeStart + "')";
        sql += " and unix_timestamp(SaveTime)<=unix_timestamp('" + timeEnd + "')";
    }

    DbHelper::execSql(sql);
}

void DbQuery::clearNodeLog()
{
    DbData::NodeLogID = 0;
    DbHelper::clearTable("NodeLog", AppConfig::LocalDbType);
}

void DbQuery::loadAlarmLink()
{
    DbData::AlarmLink_Count = 0;
    DbData::AlarmLink_PositionID.clear();
    DbData::AlarmLink_ComName.clear();
    DbData::AlarmLink_BaudRate.clear();
    DbData::AlarmLink_ModelAddr.clear();
    DbData::AlarmLink_LinkAddr.clear();
    DbData::AlarmLink_LinkEnable.clear();

    QSqlQuery query;
    QString sql = "select PositionID,ComName,BaudRate,ModelAddr,LinkAddr,LinkEnable from AlarmLink order by PositionID asc";
    if (!query.exec(sql)) {
        return;
    }

    while (query.next()) {
        QString positionID = query.value(0).toString();
        QString comName = query.value(1).toString();
        int baudRate = query.value(2).toInt();
        quint8 modelAddr = query.value(3).toInt();
        QString linkAddr = query.value(4).toString();
        QString linkEnable = query.value(5).toString();

        //过滤禁用的报警联动
        if (linkEnable == "禁用") {
            continue;
        }

        DbData::AlarmLink_Count++;
        DbData::AlarmLink_PositionID.append(positionID);
        DbData::AlarmLink_ComName.append(comName);
        DbData::AlarmLink_BaudRate.append(baudRate);
        DbData::AlarmLink_ModelAddr.append(modelAddr);
        DbData::AlarmLink_LinkAddr.append(linkAddr);
        DbData::AlarmLink_LinkEnable.append(linkEnable);
    }
}

void DbQuery::clearAlarmLink()
{
    QString sql = "delete from AlarmLink";
    DbHelper::execSql(sql);
}

void DbQuery::loadTypeInfo()
{
    DbData::TypeInfo_Count = 0;
    DbData::TypeInfo_TypeID.clear();
    DbData::TypeInfo_DeviceType.clear();
    DbData::TypeInfo_NodeNumber.clear();
    DbData::TypeInfo_NodeType.clear();
    DbData::TypeInfo_NodeClass.clear();
    DbData::TypeInfo_NodeSign.clear();

    QSqlQuery query;
    QString sql = "select TypeID,DeviceType,NodeNumber,NodeType,NodeClass,NodeSign from TypeInfo";
    if (!query.exec(sql)) {
        return;
    }

    while (query.next()) {
        int typeID = query.value(0).toInt();
        QString deviceType = query.value(1).toString();
        int nodeNumber = query.value(2).toInt();
        QString nodeType = query.value(3).toString();
        QString nodeClass = query.value(4).toString();
        QString nodeSign = query.value(5).toString();

        DbData::TypeInfo_Count++;
        DbData::TypeInfo_TypeID.append(typeID);

        //需要过滤空的
        if (!deviceType.isEmpty()) {
            DbData::TypeInfo_DeviceType.append(deviceType);
            DbData::TypeInfo_NodeNumber.append(nodeNumber);
        }

        if (!nodeType.isEmpty()) {
            DbData::TypeInfo_NodeType.append(nodeType);
        }

        if (!nodeClass.isEmpty()) {
            DbData::TypeInfo_NodeClass.append(nodeClass);
        }

        if (!nodeSign.isEmpty()) {
            DbData::TypeInfo_NodeSign.append(nodeSign);
        }
    }
}

void DbQuery::clearTypeInfo()
{
    QString sql = "delete from TypeInfo";
    DbHelper::execSql(sql);
}

void DbQuery::addAlarmLog(const QString &positionID, const QString &deviceName,
                          const QString &nodeName, float nodeValue, const QString &nodeSign,
                          const QString &content, const QString &startTime)
{
    DbData::AlarmLogID++;
    QString sql = "insert into AlarmLog(LogID,PositionID,DeviceName,NodeName,NodeValue,NodeSign,"
                  "Content,StartTime,ConfirmUser,ConfirmTime,ConfirmContent) values('";
    sql += QString::number(DbData::AlarmLogID) + "','";
    sql += positionID + "','";
    sql += deviceName + "','";
    sql += nodeName + "','";
    sql += QString::number(nodeValue) + "','";
    sql += nodeSign + "','";
    sql += content + "','";
    sql += startTime + "','";
    sql += AppData::CurrentUserName + "','";
    sql += QString(DATETIME) + "','";

    //如果开启了自动确认则自动添加确认信息
    if (AppConfig::AutoConfirm) {
        sql += QString("已确认并解决") + "')";
    } else {
        sql += QString("") + "')";
    }

    DbHelper::execSql(sql);
}

void DbQuery::confirmAlarmLog(const QString &logID, const QString &confirmContent)
{
    QString sql = QString("update AlarmLog set ConfirmUser='%1',ConfirmTime='%2',ConfirmContent='%3' where LogID='%4'")
                  .arg(AppData::CurrentUserName).arg(DATETIME).arg(confirmContent).arg(logID);
    DbHelper::execSql(sql);
}

void DbQuery::updateAlarmLog(const QString &positionID, const QString &content, const QString &endTime)
{
    //找到最近的该设备该种类型报警的LogID
    QString sql = QString("select LogID,EndTime from AlarmLog where PositionID='%1' and Content='%2' order by LogID desc limit 1")
                  .arg(positionID).arg(content);
    QSqlQuery query;
    if (!query.exec(sql)) {
        return;
    }

    if (query.next()) {
        QString logID = query.value(0).toString();
        //如果存在结束时间说明已经恢复过不需要恢复
        if (query.value(1).toString().isEmpty()) {
            sql = QString("update AlarmLog set EndTime='%1' where LogID='%2'").arg(endTime).arg(logID);
            query.clear();
            query.exec(sql);
        }
    }
}

void DbQuery::deleteAlarmLog(const QString &timeStart, const QString &timeEnd)
{
    QString sql = "delete from AlarmLog where 1=1";
    if (AppConfig::LocalDbType.toUpper() == "SQLITE") {
        sql += " and datetime(StartTime)>='" + timeStart + "'";
        sql += " and datetime(StartTime)<='" + timeEnd + "'";
    } else if (AppConfig::LocalDbType.toUpper() == "MYSQL") {
        sql += " and unix_timestamp(StartTime)>=unix_timestamp('" + timeStart + "')";
        sql += " and unix_timestamp(StartTime)<=unix_timestamp('" + timeEnd + "')";
    }

    DbHelper::execSql(sql);
}

void DbQuery::deleteAlarmLog(const QString &ids)
{
    //合并成一条语句删除
    QString sql = QString("delete from AlarmLog where LogID in (%1)").arg(ids);
    QSqlQuery query;
    query.exec(sql);
}

void DbQuery::clearAlarmLog()
{
    DbData::AlarmLogID = 0;
    DbHelper::clearTable("AlarmLog", AppConfig::LocalDbType);
}

void DbQuery::addUserLog(const QString &triggerType,
                         const QString &triggerContent)
{
    DbData::UserLogID++;
    QString sql = "insert into UserLog(LogID,TriggerTime,UserName,UserType,TriggerType,TriggerContent) values('";
    sql += QString::number(DbData::UserLogID) + "','";
    sql += QString("%1").arg(DATETIME) + "','";
    sql += AppData::CurrentUserName + "','";
    sql += AppData::CurrentUserType + "','";
    sql += triggerType + "','";
    sql += triggerContent + "')";
    DbHelper::execSql(sql);
}

void DbQuery::addUserLog(const QString &triggerContent)
{
    addUserLog("用户操作", triggerContent);
}

void DbQuery::addUserLog(int count, const QString &triggerContent)
{
    QSqlDatabase::database().transaction();
    for (int i = 0; i < count; i++) {
        DbQuery::addUserLog(triggerContent);
    }
    QSqlDatabase::database().commit();
}

void DbQuery::deleteUserLog(const QString &timeStart, const QString &timeEnd)
{
    QString sql = "delete from UserLog where 1=1";
    if (AppConfig::LocalDbType.toUpper() == "SQLITE") {
        sql += " and datetime(TriggerTime)>='" + timeStart + "'";
        sql += " and datetime(TriggerTime)<='" + timeEnd + "'";
    } else if (AppConfig::LocalDbType.toUpper() == "MYSQL") {
        sql += " and unix_timestamp(TriggerTime)>=unix_timestamp('" + timeStart + "')";
        sql += " and unix_timestamp(TriggerTime)<=unix_timestamp('" + timeEnd + "')";
    }

    DbHelper::execSql(sql);
}

void DbQuery::clearUserLog()
{
    DbData::UserLogID = 0;
    DbHelper::clearTable("UserLog", AppConfig::LocalDbType);
}

void DbQuery::clearNodeData()
{
    DbHelper::clearTable("NodeData", AppConfig::LocalDbType);
}

void DbQuery::updatePosition(DeviceButton *btn)
{
    QString sql = QString("update NodeInfo set NodeX='%1',NodeY='%2' where NodeID='%3'")
                  .arg(btn->geometry().x()).arg(btn->geometry().y()).arg(btn->property("nodeID").toString());
    DbHelper::execSql(sql);
}
