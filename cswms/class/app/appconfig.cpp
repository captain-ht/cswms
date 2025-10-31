#include "appconfig.h"
#include "quihelper.h"

QString AppConfig::ConfigFile = "config.ini";

bool AppConfig::AutoRun = false;
bool AppConfig::AutoPwd = false;
bool AppConfig::AutoLogin = false;
bool AppConfig::AutoConfirm = true;
bool AppConfig::SaveLog = false;

QString AppConfig::Copyright = QString::fromUtf8("启阳工控物联网技术研究中心");
QString AppConfig::TitleCn = QString::fromUtf8("CSWMS起重机安全预警管理系统");
QString AppConfig::TitleEn = "crane security warning management system";
QString AppConfig::LogoImage = "config/logo_black.png";
QString AppConfig::HttpUrl = "http://www.ipctech.com.cn";
QString AppConfig::StyleName = ":/qss/lightblue.css";
QString AppConfig::LastLoginer = "admin";
QString AppConfig::LastFormMain = "btnViewData";
QString AppConfig::LastFormData = "btnDataNode";
QString AppConfig::LastFormConfig = "btnConfigSystem";

int AppConfig::WorkMode = 0;
int AppConfig::PanelMode = 0;
bool AppConfig::PlayAlarmSound = true;
int AppConfig::PlayCount = 1;
int AppConfig::MsgCount = 26;
int AppConfig::DeviceColumn = 6;
int AppConfig::RecordsPerpage = 28;
int AppConfig::TipInterval = 0;
int AppConfig::Precision = 3;

int AppConfig::AlarmLogMaxCount = 10;
int AppConfig::NodeLogMaxCount = 10;
int AppConfig::UserLogMaxCount = 10;
QString AppConfig::AlarmLogOrder = "desc";
QString AppConfig::NodeLogOrder = "desc";
QString AppConfig::UserLogOrder = "desc";

QString AppConfig::ColorZero = "blueviolet";
QString AppConfig::ColorUpper = "crimson";
QString AppConfig::ColorLimit = "darkorange";
QString AppConfig::ColorNormal = "lightseagreen";
QString AppConfig::ColorPlotBg = "darkcyan";
QString AppConfig::ColorPlotText = "white";
QString AppConfig::ColorPlotLine = "lightskyblue";
QString AppConfig::ColorOther = "firebrick";

bool AppConfig::UseNetSend = false;
QString AppConfig::NetSendInfo = "39.107.90.218:6060";
bool AppConfig::UseNetReceive = false;
int AppConfig::NetReceivePort = 6060;

QString AppConfig::MsgPortName = "COM8";
int AppConfig::MsgBaudRate = 9600;
int AppConfig::MsgInterval = 10000;
QString AppConfig::MsgTel = "13888888888";

QString AppConfig::SendEmailAddr = "liuyang_tekkens@163.com";
QString AppConfig::SendEmailPwd = "12345678";
int AppConfig::EmailInterval = 10000;
QString AppConfig::ReceiveEmailAddr = "liuyang_tekkens@163.com;297000733@qq.com";

QString AppConfig::LocalDbType = "Sqlite";
QString AppConfig::LocalDbName = "cswms";
QString AppConfig::LocalHostName = "127.0.0.1";
int AppConfig::LocalHostPort = 3306;
QString AppConfig::LocalUserName = "root";
QString AppConfig::LocalUserPwd = "root";

bool AppConfig::UseNetDb = false;
QString AppConfig::NetDbType = "MySql";
QString AppConfig::NetDbName = "cswms";
QString AppConfig::NetHostName = "127.0.0.1";
int AppConfig::NetHostPort = 3306;
QString AppConfig::NetUserName = "root";
QString AppConfig::NetUserPwd = "root";

int AppConfig::PortNameIndex = 0;
int AppConfig::DeviceNameIndex = 0;
QString AppConfig::KeyValue1 = "|";
QString AppConfig::KeyColor1 = "darkred";
QString AppConfig::KeyValue2 = "|";
QString AppConfig::KeyColor2 = "darkgreen";
QString AppConfig::SelectDirName = ".";

QString AppConfig::DeviceType = "FC-1003-8";
int AppConfig::DeviceAddr = 1;
QString AppConfig::PortName = "COM2";
int AppConfig::BaudRate = 9600;
int AppConfig::ListenPort = 502;
QString AppConfig::NodeValues = "20|785|60|88|75|62|32|99|20|785|60|88|75|62|32|99";
bool AppConfig::AutoData = false;
int AppConfig::TabIndex = 0;

void AppConfig::readConfig()
{
    //配置文件不存在或者有问题则重新生成
    if (!QUIHelper::checkIniFile(ConfigFile)) {
        if (QUIHelper::deskWidth() <= 1440) {
            MsgCount = 23;
            RecordsPerpage = 23;
        }
        writeConfig();
        return;
    }

    QSettings set(ConfigFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    set.setIniCodec("utf-8");
#endif

    set.beginGroup("AppConfig1");
    AutoRun = set.value("AutoRun", AutoRun).toBool();
    AutoPwd = set.value("AutoPwd", AutoPwd).toBool();
    AutoLogin = set.value("AutoLogin", AutoLogin).toBool();
    AutoConfirm = set.value("AutoConfirm", AutoConfirm).toBool();
    SaveLog = set.value("SaveLog", SaveLog).toBool();
    set.endGroup();

    set.beginGroup("AppConfig2");
    Copyright = set.value("Copyright", Copyright).toString();
    TitleCn = set.value("TitleCn", TitleCn).toString();
    TitleEn = set.value("TitleEn", TitleEn).toString();
    LogoImage = set.value("LogoImage", LogoImage).toString();
    HttpUrl = set.value("HttpUrl", HttpUrl).toString();
    StyleName = set.value("StyleName", StyleName).toString();
    LastLoginer = set.value("LastLoginer", LastLoginer).toString();
    LastFormMain = set.value("LastFormMain", LastFormMain).toString();
    LastFormData = set.value("LastFormData", LastFormData).toString();
    LastFormConfig = set.value("LastFormConfig", LastFormConfig).toString();
    set.endGroup();

    set.beginGroup("AppConfig3");
    WorkMode = set.value("WorkMode", WorkMode).toInt();
    PanelMode = set.value("PanelMode", PanelMode).toInt();
    PlayAlarmSound = set.value("PlayAlarmSound", PlayAlarmSound).toBool();
    PlayCount = set.value("PlayCount", PlayCount).toInt();
    MsgCount = set.value("MsgCount", MsgCount).toInt();
    DeviceColumn = set.value("DeviceColumn", DeviceColumn).toInt();
    RecordsPerpage = set.value("RecordsPerpage", RecordsPerpage).toInt();
    TipInterval = set.value("TipInterval", TipInterval).toInt();
    Precision = set.value("Precision", Precision).toInt();
    set.endGroup();

    set.beginGroup("LogConfig");
    AlarmLogMaxCount = set.value("AlarmLogMaxCount", AlarmLogMaxCount).toInt();
    NodeLogMaxCount = set.value("NodeLogMaxCount", NodeLogMaxCount).toInt();
    UserLogMaxCount = set.value("UserLogMaxCount", UserLogMaxCount).toInt();
    AlarmLogOrder = set.value("AlarmLogOrder", AlarmLogOrder).toString();
    NodeLogOrder = set.value("NodeLogOrder", NodeLogOrder).toString();
    UserLogOrder = set.value("UserLogOrder", UserLogOrder).toString();
    set.endGroup();

    set.beginGroup("ColorConfig");
    ColorZero = set.value("ColorZero", ColorZero).toString();
    ColorUpper = set.value("ColorUpper", ColorUpper).toString();
    ColorLimit = set.value("ColorLimit", ColorLimit).toString();
    ColorNormal = set.value("ColorNormal", ColorNormal).toString();
    ColorPlotBg = set.value("ColorPlotBg", ColorPlotBg).toString();
    ColorPlotText = set.value("ColorPlotText", ColorPlotText).toString();
    ColorPlotLine = set.value("ColorPlotLine", ColorPlotLine).toString();
    ColorOther = set.value("ColorOther", ColorOther).toString();
    set.endGroup();

    set.beginGroup("NetLink");
    UseNetSend = set.value("UseNetSend", UseNetSend).toBool();
    NetSendInfo = set.value("NetSendInfo", NetSendInfo).toString();
    UseNetReceive = set.value("UseNetReceive", UseNetReceive).toBool();
    NetReceivePort = set.value("NetReceivePort", NetReceivePort).toInt();
    set.endGroup();

    set.beginGroup("MsgLink");
    MsgPortName = set.value("MsgPortName", MsgPortName).toString();
    MsgBaudRate = set.value("MsgBaudRate", MsgBaudRate).toInt();
    MsgInterval = set.value("MsgInterval", MsgInterval).toInt();
    MsgTel = set.value("MsgTel", MsgTel).toString();
    set.endGroup();

    set.beginGroup("EmailLink");
    SendEmailAddr = set.value("SendEmailAddr", SendEmailAddr).toString();
    SendEmailPwd = set.value("SendEmailPwd", SendEmailPwd).toString();
    SendEmailPwd = QUIHelper::getXorEncryptDecrypt(SendEmailPwd, 100);
    EmailInterval = set.value("EmailInterval", EmailInterval).toInt();
    ReceiveEmailAddr = set.value("ReceiveEmailAddr", ReceiveEmailAddr).toString();
    set.endGroup();

    set.beginGroup("LocalDbConfig");
    LocalDbType = set.value("LocalDbType", LocalDbType).toString();
    LocalDbName = set.value("LocalDbName", LocalDbName).toString();
    LocalHostName = set.value("LocalHostName", LocalHostName).toString();
    LocalHostPort = set.value("LocalHostPort", LocalHostPort).toInt();
    LocalUserName = set.value("LocalUserName", LocalUserName).toString();
    LocalUserPwd = set.value("LocalUserPwd", LocalUserPwd).toString();
    LocalUserPwd = QUIHelper::getXorEncryptDecrypt(LocalUserPwd, 100);
    set.endGroup();

    set.beginGroup("NetDbConfig");
    UseNetDb = set.value("UseNetDb", UseNetDb).toBool();
    NetDbType = set.value("NetDbType", NetDbType).toString();
    NetDbName = set.value("NetDbName", NetDbName).toString();
    NetHostName = set.value("NetHostName", NetHostName).toString();
    NetHostPort = set.value("NetHostPort", NetHostPort).toInt();
    NetUserName = set.value("NetUserName", NetUserName).toString();
    NetUserPwd = set.value("NetUserPwd", NetUserPwd).toString();
    NetUserPwd = QUIHelper::getXorEncryptDecrypt(NetUserPwd, 100);
    set.endGroup();

    set.beginGroup("DebugConfig");
    PortNameIndex = set.value("PortNameIndex", PortNameIndex).toInt();
    DeviceNameIndex = set.value("DeviceNameIndex", DeviceNameIndex).toInt();
    KeyValue1 = set.value("KeyValue1", KeyValue1).toString();
    KeyColor1 = set.value("KeyColor1", KeyColor1).toString();
    KeyValue2 = set.value("KeyValue2", KeyValue2).toString();
    KeyColor2 = set.value("KeyColor2", KeyColor2).toString();
    SelectDirName = set.value("SelectDirName", SelectDirName).toString();
    set.endGroup();

    set.beginGroup("ToolConfig");
    DeviceType = set.value("DeviceType", DeviceType).toString();
    DeviceAddr = set.value("DeviceAddr", DeviceAddr).toInt();
    PortName = set.value("PortName", PortName).toString();
    BaudRate = set.value("BaudRate", BaudRate).toInt();
    ListenPort = set.value("ListenPort", ListenPort).toInt();
    NodeValues = set.value("NodeValues", NodeValues).toString();
    AutoData = set.value("AutoData", AutoData).toBool();
    TabIndex = set.value("TabIndex", TabIndex).toInt();
    set.endGroup();

    checkConfig();
}

void AppConfig::checkConfig()
{
    //过滤不符合要求的配置
    AlarmLogMaxCount = AlarmLogMaxCount > 1000 ? 1000 : AlarmLogMaxCount;
    NodeLogMaxCount = NodeLogMaxCount > 1000 ? 1000 : NodeLogMaxCount;
    UserLogMaxCount = UserLogMaxCount > 1000 ? 1000 : UserLogMaxCount;
}

void AppConfig::writeConfig()
{
    QSettings set(ConfigFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    set.setIniCodec("utf-8");
#endif

    set.beginGroup("AppConfig1");
    set.setValue("AutoRun", AutoRun);
    set.setValue("AutoPwd", AutoPwd);
    set.setValue("AutoLogin", AutoLogin);
    set.setValue("AutoConfirm", AutoConfirm);
    set.setValue("SaveLog", SaveLog);
    set.endGroup();

    set.beginGroup("AppConfig2");
    set.setValue("Copyright", Copyright);
    set.setValue("TitleCn", TitleCn);
    set.setValue("TitleEn", TitleEn);
    set.setValue("LogoImage", LogoImage);
    set.setValue("HttpUrl", HttpUrl);
    set.setValue("StyleName", StyleName);
    set.setValue("LastLoginer", LastLoginer);
    set.setValue("LastFormMain", LastFormMain);
    set.setValue("LastFormData", LastFormData);
    set.setValue("LastFormConfig", LastFormConfig);
    set.endGroup();

    set.beginGroup("AppConfig3");
    set.setValue("WorkMode", WorkMode);
    set.setValue("PanelMode", PanelMode);
    set.setValue("PlayAlarmSound", PlayAlarmSound);
    set.setValue("PlayCount", PlayCount);
    set.setValue("MsgCount", MsgCount);
    set.setValue("DeviceColumn", DeviceColumn);
    set.setValue("RecordsPerpage", RecordsPerpage);
    set.setValue("TipInterval", TipInterval);
    set.setValue("Precision", Precision);
    set.endGroup();

    set.beginGroup("LogConfig");
    set.setValue("AlarmLogMaxCount", AlarmLogMaxCount);
    set.setValue("NodeLogMaxCount", NodeLogMaxCount);
    set.setValue("UserLogMaxCount", UserLogMaxCount);
    set.setValue("AlarmLogOrder", AlarmLogOrder);
    set.setValue("NodeLogOrder", NodeLogOrder);
    set.setValue("UserLogOrder", UserLogOrder);
    set.endGroup();

    set.beginGroup("ColorConfig");
    set.setValue("ColorZero", ColorZero);
    set.setValue("ColorUpper", ColorUpper);
    set.setValue("ColorLimit", ColorLimit);
    set.setValue("ColorNormal", ColorNormal);
    set.setValue("ColorPlotBg", ColorPlotBg);
    set.setValue("ColorPlotText", ColorPlotText);
    set.setValue("ColorPlotLine", ColorPlotLine);
    set.setValue("ColorOther", ColorOther);
    set.endGroup();

    set.beginGroup("NetLink");
    set.setValue("UseNetSend", UseNetSend);
    set.setValue("NetSendInfo", NetSendInfo);
    set.setValue("UseNetReceive", UseNetReceive);
    set.setValue("NetReceivePort", NetReceivePort);
    set.endGroup();

    set.beginGroup("MsgLink");
    set.setValue("MsgPortName", MsgPortName);
    set.setValue("MsgBaudRate", MsgBaudRate);
    set.setValue("MsgInterval", MsgInterval);
    set.setValue("MsgTel", MsgTel);
    set.endGroup();

    set.beginGroup("EmailLink");
    set.setValue("SendEmailAddr", SendEmailAddr);
    set.setValue("SendEmailPwd", QUIHelper::getXorEncryptDecrypt(SendEmailPwd, 100));
    set.setValue("EmailInterval", EmailInterval);
    set.setValue("ReceiveEmailAddr", ReceiveEmailAddr);
    set.endGroup();

    set.beginGroup("LocalDbConfig");
    set.setValue("LocalDbType", LocalDbType);
    set.setValue("LocalDbName", LocalDbName);
    set.setValue("LocalHostName", LocalHostName);
    set.setValue("LocalHostPort", LocalHostPort);
    set.setValue("LocalUserName", LocalUserName);
    set.setValue("LocalUserPwd", QUIHelper::getXorEncryptDecrypt(LocalUserPwd, 100));
    set.endGroup();

    set.beginGroup("NetDbConfig");
    set.setValue("UseNetDb", UseNetDb);
    set.setValue("NetDbType", NetDbType);
    set.setValue("NetDbName", NetDbName);
    set.setValue("NetHostName", NetHostName);
    set.setValue("NetHostPort", NetHostPort);
    set.setValue("NetUserName", NetUserName);
    set.setValue("NetUserPwd", QUIHelper::getXorEncryptDecrypt(NetUserPwd, 100));
    set.endGroup();

    set.beginGroup("DebugConfig");
    set.setValue("PortNameIndex", PortNameIndex);
    set.setValue("DeviceNameIndex", DeviceNameIndex);
    set.setValue("KeyValue1", KeyValue1);
    set.setValue("KeyColor1", KeyColor1);
    set.setValue("KeyValue2", KeyValue2);
    set.setValue("KeyColor2", KeyColor2);
    set.setValue("SelectDirName", SelectDirName);
    set.endGroup();

    set.beginGroup("ToolConfig");
    set.setValue("DeviceType", DeviceType);
    set.setValue("DeviceAddr", DeviceAddr);
    set.setValue("PortName", PortName);
    set.setValue("BaudRate", BaudRate);
    set.setValue("ListenPort", ListenPort);
    set.setValue("NodeValues", NodeValues);
    set.setValue("AutoData", AutoData);
    set.setValue("TabIndex", TabIndex);
    set.endGroup();
}
