#ifndef DBDATA_H
#define DBDATA_H

#include "dbconnthread.h"
#include "public.h"

class DbData
{
public:
    //本地数据库连接
    static DbConnThread *DbLocal;
    //远程数据库连接
    static DbConnThread *DbNet;

    //记录数编号用于递增字段
    static int UserLogID;
    static int AlarmLogID;
    static int NodeLogID;

    //用户信息链表
    static int UserInfo_Count;
    static QList<QString> UserInfo_UserName;
    static QList<QString> UserInfo_UserPwd;
    static QList<QString> UserInfo_UserType;
    static QList<QString> UserInfo_Permission1;
    static QList<QString> UserInfo_Permission2;
    static QList<QString> UserInfo_Permission3;
    static QList<QString> UserInfo_Permission4;
    static QList<QString> UserInfo_Permission5;
    static QList<QString> UserInfo_Permission6;
    static QList<QString> UserInfo_Permission7;

    //端口信息表
    static int PortInfo_Count;
    static QList<int> PortInfo_PortID;
    static QList<QString> PortInfo_PortName;
    static QList<QString> PortInfo_PortType;
    static QList<QString> PortInfo_Protocol;
    static QList<QString> PortInfo_ComName;
    static QList<int> PortInfo_BaudRate;
    static QList<QString> PortInfo_TcpIP;
    static QList<int> PortInfo_TcpPort;
    static QList<float> PortInfo_ReadInterval;
    static QList<int> PortInfo_ReadTimeout;
    static QList<int> PortInfo_ReadMaxtime;

    //设备信息表
    static int DeviceInfo_Count;
    static QList<int> DeviceInfo_DeviceID;
    static QList<QString> DeviceInfo_PortName;
    static QList<QString> DeviceInfo_DeviceName;
    static QList<quint8> DeviceInfo_DeviceAddr;
    static QList<QString> DeviceInfo_DeviceType;
    static QList<int> DeviceInfo_NodeNumber;

    //变量信息表
    static int NodeInfo_Count;
    static QList<int> NodeInfo_NodeID;
    static QList<QString> NodeInfo_PositionID;// 变量位号
    static QList<QString> NodeInfo_DeviceName;// 设备名称
    static QList<QString> NodeInfo_NodeName;// 变量名称
    static QList<QString> NodeInfo_NodeDescription;// 变量描述
    static QList<QString> NodeInfo_PermissionType;// 读写类型
    static QList<QString> NodeInfo_DataType;// 数据类型
    static QList<QString> NodeInfo_RegisterArea;// 寄存器区
    static QList<quint16> NodeInfo_NodeAddr;// 设备地址
    static QList<QString> NodeInfo_NodeType;//变量类型
    static QList<QString> NodeInfo_NodeClass;//所属归类
    static QList<QString> NodeInfo_NodeSign;//数据单位
    static QList<float> NodeInfo_NodeUpper;//上限值
    static QList<float> NodeInfo_NodeLimit;//下限值
    static QList<float> NodeInfo_NodeMax;// 最大值
    static QList<float> NodeInfo_NodeMin;// 最小值
    static QList<float> NodeInfo_NodeRange;//工程转换
    static QList<QString> NodeInfo_NodeEnable;// 是否IO连接
    static QList<QString> NodeInfo_NodeSound;
    static QList<QString> NodeInfo_NodeImage;
    static QList<int> NodeInfo_SaveInterval;//存储的周期，单位分钟
    static QList<int> NodeInfo_DotCount;//小数点位数
    static QList<QString> NodeInfo_AlarmEnable;// 是否报警
    static QList<int> NodeInfo_AlarmDelay;//报警延时
    static QList<QString> NodeInfo_AlarmType;//报警类型
    static QList<float> NodeInfo_AlarmH;//报警上限值
    static QList<float> NodeInfo_AlarmL;//报警下限值
    static QList<int> NodeInfo_NodeX;//地图上的X坐标
    static QList<int> NodeInfo_NodeY;//地图上的Y坐标



    //变量状态报警信息
    static QList<bool> NodeInfo_Online;
    static QList<bool> NodeInfo_AlarmUpper;
    static QList<bool> NodeInfo_AlarmLimit;
    static QList<bool> NodeInfo_AlarmOther;
    static QList<float> NodeInfo_NodeValue;
    static QList<QDateTime> NodeInfo_SaveTime;

    //联动信息表
    static int AlarmLink_Count;
    static QList<QString> AlarmLink_PositionID;
    static QList<QString> AlarmLink_ComName;
    static QList<int> AlarmLink_BaudRate;
    static QList<quint8> AlarmLink_ModelAddr;
    static QList<QString> AlarmLink_LinkAddr;
    static QList<QString> AlarmLink_LinkEnable;

    //类型信息表 控制器型号+变量数量+变量型号+气体种类+气体符号
    static int TypeInfo_Count;
    static QList<int> TypeInfo_TypeID;
    static QList<QString> TypeInfo_DeviceType;
    static QList<int> TypeInfo_NodeNumber;
    static QList<QString> TypeInfo_NodeType;
    static QList<QString> TypeInfo_NodeClass;
    static QList<QString> TypeInfo_NodeSign;
};

#endif // DBDATA_H
