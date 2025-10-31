#include "dbdata.h"

DbConnThread *DbData::DbLocal = 0;
DbConnThread *DbData::DbNet = 0;

int DbData::UserLogID = 0;
int DbData::AlarmLogID = 0;
int DbData::NodeLogID = 0;

int DbData::UserInfo_Count = 0;
QList<QString> DbData::UserInfo_UserName = QList<QString>();
QList<QString> DbData::UserInfo_UserPwd = QList<QString>();
QList<QString> DbData::UserInfo_UserType = QList<QString>();
QList<QString> DbData::UserInfo_Permission1 = QList<QString>();
QList<QString> DbData::UserInfo_Permission2 = QList<QString>();
QList<QString> DbData::UserInfo_Permission3 = QList<QString>();
QList<QString> DbData::UserInfo_Permission4 = QList<QString>();
QList<QString> DbData::UserInfo_Permission5 = QList<QString>();
QList<QString> DbData::UserInfo_Permission6 = QList<QString>();
QList<QString> DbData::UserInfo_Permission7 = QList<QString>();

int DbData::PortInfo_Count = 0;
QList<int> DbData::PortInfo_PortID = QList<int>();
QList<QString> DbData::PortInfo_PortName = QList<QString>();
QList<QString> DbData::PortInfo_PortType = QList<QString>();
QList<QString> DbData::PortInfo_Protocol = QList<QString>();
QList<QString> DbData::PortInfo_ComName = QList<QString>();
QList<int> DbData::PortInfo_BaudRate = QList<int>();
QList<QString> DbData::PortInfo_TcpIP = QList<QString>();
QList<int> DbData::PortInfo_TcpPort = QList<int>();
QList<float> DbData::PortInfo_ReadInterval = QList<float>();
QList<int> DbData::PortInfo_ReadTimeout = QList<int>();
QList<int> DbData::PortInfo_ReadMaxtime = QList<int>();

int DbData::DeviceInfo_Count = 0;
QList<int> DbData::DeviceInfo_DeviceID = QList<int>();
QList<QString> DbData::DeviceInfo_PortName = QList<QString>();
QList<QString> DbData::DeviceInfo_DeviceName = QList<QString>();
QList<quint8> DbData::DeviceInfo_DeviceAddr = QList<quint8>();
QList<QString> DbData::DeviceInfo_DeviceType = QList<QString>();
QList<int> DbData::DeviceInfo_NodeNumber = QList<int>();

int DbData::NodeInfo_Count = 0;
QList<int> DbData::NodeInfo_NodeID = QList<int>();
QList<QString> DbData::NodeInfo_PositionID = QList<QString>();
QList<QString> DbData::NodeInfo_DeviceName = QList<QString>();
QList<QString> DbData::NodeInfo_NodeName = QList<QString>();
QList<QString> DbData::NodeInfo_NodeDescription = QList<QString>();
QList<QString> DbData::NodeInfo_PermissionType = QList<QString>();
QList<QString> DbData::NodeInfo_DataType = QList<QString>();
QList<QString> DbData::NodeInfo_RegisterArea = QList<QString>();
QList<quint16> DbData::NodeInfo_NodeAddr = QList<quint16>();
QList<QString> DbData::NodeInfo_NodeType = QList<QString>();
QList<QString> DbData::NodeInfo_NodeClass = QList<QString>();
QList<QString> DbData::NodeInfo_NodeSign = QList<QString>();
QList<float> DbData::NodeInfo_NodeUpper = QList<float>();
QList<float> DbData::NodeInfo_NodeLimit = QList<float>();
QList<float> DbData::NodeInfo_NodeMax = QList<float>();
QList<float> DbData::NodeInfo_NodeMin = QList<float>();
QList<float> DbData::NodeInfo_NodeRange = QList<float>();
QList<QString> DbData::NodeInfo_NodeEnable = QList<QString>();
QList<QString> DbData::NodeInfo_NodeSound = QList<QString>();
QList<QString> DbData::NodeInfo_NodeImage = QList<QString>();
QList<int> DbData::NodeInfo_SaveInterval = QList<int>();
QList<int> DbData::NodeInfo_DotCount = QList<int>();
QList<QString> DbData::NodeInfo_AlarmEnable = QList<QString>();
QList<int> DbData::NodeInfo_AlarmDelay = QList<int>();
QList<QString> DbData::NodeInfo_AlarmType = QList<QString>();
QList<float> DbData::NodeInfo_AlarmH = QList<float>();
QList<float> DbData::NodeInfo_AlarmL = QList<float>();
QList<int> DbData::NodeInfo_NodeX = QList<int>();
QList<int> DbData::NodeInfo_NodeY = QList<int>();

QList<bool> DbData::NodeInfo_Online = QList<bool>();
QList<bool> DbData::NodeInfo_AlarmUpper = QList<bool>();
QList<bool> DbData::NodeInfo_AlarmLimit = QList<bool>();
QList<bool> DbData::NodeInfo_AlarmOther = QList<bool>();
QList<float> DbData::NodeInfo_NodeValue = QList<float>();
QList<QDateTime> DbData::NodeInfo_SaveTime = QList<QDateTime>();

int DbData::AlarmLink_Count = 0;
QList<QString> DbData::AlarmLink_PositionID = QList<QString>();
QList<QString> DbData::AlarmLink_ComName = QList<QString>();
QList<int> DbData::AlarmLink_BaudRate = QList<int>();
QList<quint8> DbData::AlarmLink_ModelAddr = QList<quint8>();
QList<QString> DbData::AlarmLink_LinkAddr = QList<QString>();
QList<QString> DbData::AlarmLink_LinkEnable = QList<QString>();

int DbData::TypeInfo_Count = 0;
QList<int> DbData::TypeInfo_TypeID = QList<int>();
QList<QString> DbData::TypeInfo_DeviceType = QList<QString>();
QList<int> DbData::TypeInfo_NodeNumber = QList<int>();
QList<QString> DbData::TypeInfo_NodeType = QList<QString>();
QList<QString> DbData::TypeInfo_NodeClass = QList<QString>();
QList<QString> DbData::TypeInfo_NodeSign = QList<QString>();
