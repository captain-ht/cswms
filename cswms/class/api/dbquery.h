#ifndef DBQUERY_H
#define DBQUERY_H

#include "dbhelper.h"
#include "devicebutton.h"
#include "datacsv.h"

class DbQuery
{
public:
    static bool loadfromdb;

    //载入用户信息
    static void loadUserInfo();
    //清空用户信息
    static void clearUserInfo();
    //更新密码
    static void updateUserInfo(const QString &userName, const QString &userPwd);
    //取出用户的密码及类型和权限
    static void getUserInfo(const QString &userName, QString &userPwd,
                            QString &userType, QList<bool> &permission);

    //载入端口信息
    static void loadPortInfo();
    //清空端口信息
    static void clearPortInfo();

    //载入设备信息
    static void loadDeviceInfo();
    //清空设备信息
    static void clearDeviceInfo();
    //删除指定端口设备
    static void deleteDeviceInfo(const QString &portName);

    //根据设备名称查询端口名称
    static QString getPortName(const QString &deviceName);
    //根据端口名称+设备地址获取对应的设备的名称
    static QString getDeviceName(const QString &portName, quint8 addr);
    //根据端口名称+设备地址获取对应的设备的名称+型号
    static void getDeviceInfo(const QString &portName, quint8 addr, QString &deviceName, QString &deviceType);
    //根据端口名称+设备地址获取对应的设备的变量个数
    static int getNodeNumber(const QString &portName, quint8 addr);
    //根据端口名称+设备地址获取寄存器其实地址
    static quint16 getNodeMinAddr(const QString &portName, quint8 addr);

    //载入变量信息
    static void loadNodeInfo();
    //清空变量信息
    static void clearNodeInfo();
    //删除指定设备名称的变量
    static void deleteNodeInfo(const QString &deviceName);

    //插入运行记录
    static void addNodeLog(const QString &positionID, const QString &deviceName,
                           const QString &nodeName, float nodeValue,
                           const QString &nodeSign, const QString &saveTime = DATETIME);
    //删除运行记录
    static void deleteNodeLog(const QString &timeStart, const QString &timeEnd);
    static void clearNodeLog();

    //载入报警联动信息
    static void loadAlarmLink();
    //清空报警联动信息
    static void clearAlarmLink();

    //载入类型信息
    static void loadTypeInfo();
    //清空类型信息
    static void clearTypeInfo();

    //插入报警记录
    static void addAlarmLog(const QString &positionID, const QString &deviceName,
                            const QString &nodeName, float nodeValue, const QString &nodeSign,
                            const QString &content, const QString &startTime);
    //确认报警记录
    static void confirmAlarmLog(const QString &positionID, const QString &confirmContent);
    //更新报警记录
    static void updateAlarmLog(const QString &positionID, const QString &content, const QString &endTime);
    //删除报警记录
    static void deleteAlarmLog(const QString &timeStart, const QString &timeEnd);
    //删除报警记录指定id号集合
    static void deleteAlarmLog(const QString &ids);
    //清空报警记录
    static void clearAlarmLog();

    //插入用户记录
    static void addUserLog(const QString &triggerType,
                           const QString &triggerContent);
    static void addUserLog(const QString &triggerContent);
    //随机生成数据
    static void addUserLog(int count, const QString &triggerContent);

    //删除用户记录
    static void deleteUserLog(const QString &timeStart, const QString &timeEnd);
    static void clearUserLog();

    //删除设备临时记录表
    static void clearNodeData();
    //更新按钮位置
    static void updatePosition(DeviceButton *btn);
};

#endif // DBQUERY_H
