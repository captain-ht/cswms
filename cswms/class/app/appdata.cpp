#include "appdata.h"
#include "head.h"

QString AppData::Version = "V20210925";
int AppData::MapWidth = 800;
int AppData::MapHeight = 600;
bool AppData::IsMove = false;
bool AppData::IsReboot = false;

QString AppData::CurrentImage = "bg_alarm.jpg";
QString AppData::CurrentUserName = "admin";
QString AppData::CurrentUserPwd = "admin";
QString AppData::CurrentUserType = QString::fromUtf8("管理员");
QList<bool> AppData::UserPermission = QList<bool>() << true << true << true << true << true << true << true;
QStringList AppData::PermissionName = QStringList() << QString::fromUtf8("系统设置|删除记录|模块A|模块B|模块C|模块D|模块E").split("|");

int AppData::RowHeight = 25;
int AppData::LeftWidth = 200;
int AppData::RightWidth = 210;
int AppData::TopHeight = 80;
int AppData::BottomHeight = 25;
int AppData::DeviceWidth = 75;
int AppData::DeviceHeight = 35;

int AppData::BtnMinWidth = 70;
int AppData::BtnMinHeight = 55;
int AppData::BtnIconSize = 18;
int AppData::BorderWidth = 4;
int AppData::IconSize = 20;
int AppData::IconWidth = 40;
int AppData::IconHeight = 30;
int AppData::SwitchBtnWidth = 65;
int AppData::SwitchBtnHeight = 26;

QString AppData::DefaultPositionID = "AT-40001";
QStringList AppData::AlarmTypes = QString::fromUtf8("下限报警|上限报警|其他报警").split("|");
QString AppData::NodeDataColumns = QString::fromUtf8("编号|位号|控制器名称|变量名称|数值|数据单位|保存时间");
QString AppData::TableDataColumns = QString::fromUtf8("序号|位号|控制器名称|控制器型号|变量名称|变量型号|数据类型|实时数据|单位|状态");
QString AppData::TypeInfoColumns = QString::fromUtf8("编号|控制器型号|变量数量|变量型号|数据类型|数据单位");
QString AppData::NodeInfoColumns = QString::fromUtf8("编号|位号|控制器名称|变量名称|变量描述|读写类型|数据类型|寄存器区|寄存器|型号|设备类型|数据单位|工程值上限|工程值下限|最大值|最小值|系数|启用|声音|地图|存储|小数点|报警启用|报警延时|报警类型|高报值|低报值|X坐标|Y坐标");

QString AppData::MapPath = QString();
QList<QString> AppData::MapNames = QList<QString>();
QString AppData::SoundPath = QString();
QList<QString> AppData::SoundNames = QList<QString>();
QList<QString> AppData::PortNames = QList<QString>();
