#include "appinit.h"
#include "appkey.h"
#include "quiwidget.h"
#include "dbquery.h"
#include "dbcleanthread.h"
#include "devicemap.h"
#include "frmconfigdb.h"
#include "frmconfigdbnet.h"

QScopedPointer<AppInit> AppInit::self;
AppInit *AppInit::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new AppInit);
        }
    }

    return self.data();
}

AppInit::AppInit(QObject *parent) : QObject(parent)
{
}

bool AppInit::eventFilter(QObject *watched, QEvent *event)
{
    QWidget *w = (QWidget *)watched;
    if (!w->property("canMove").toBool()) {
        return QObject::eventFilter(watched, event);
    }

    static QPoint mousePoint;
    static bool mousePressed = false;

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (event->type() == QEvent::MouseButtonPress) {
        if (mouseEvent->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = mouseEvent->globalPos() - w->pos();
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
    } else if (event->type() == QEvent::MouseMove) {
        if (mousePressed) {
            w->move(mouseEvent->globalPos() - mousePoint);
            return true;
        }
    }

    return QObject::eventFilter(watched, event);
}

void AppInit::start()
{
    qApp->installEventFilter(this);
#ifdef cswmstool
    qApp->setWindowIcon(QIcon(":/tool.ico"));
#else
    qApp->setWindowIcon(QIcon(":/main.ico"));
#endif

    //设置全局字体
    QUIConfig::IconMain = 0xf108;
    QUIConfig::FontSize = 12;
    QFont font;
    font.setFamily(QUIConfig::FontName);
    font.setPixelSize(QUIConfig::FontSize);
    qApp->setFont(font);

    //设置编码
    QUIHelper::setCode();
    //设置翻译文件
    QUIHelper::setTranslator(":/image/qt_zh_CN.qm");
    QUIHelper::setTranslator(":/image/widgets.qm");
    QUIHelper::setTranslator(":/image/designer_zh_CN.qm");
    //初始化随机数种子
    QUIHelper::initRand();

    //新建不存在的目录
    QUIHelper::newDir("config");
    QUIHelper::newDir("log");
    QUIHelper::newDir("db");
    QUIHelper::newDir("map");

    //载入配置文件
    AppConfig::ConfigFile = QString("%1/config/%2.ini").arg(QUIHelper::appPath()).arg("cswms");
    AppConfig::readConfig();

    //校验秘钥或机器码
#ifndef cswmstool
    bool ok = AppKey::Instance()->checkKey(qApp->applicationDirPath() + "/db/key.db", 110);
    if (!ok) {
        exit(0);
    }
    //AppKey::Instance()->checkLicense(qApp->applicationDirPath() + "/db/key.lic");
#endif

    //计算地图宽高
    AppData::MapWidth = QUIHelper::deskWidth() - AppData::LeftWidth - AppData::RightWidth - 25;
    AppData::MapHeight = QUIHelper::deskHeight() - AppData::TopHeight - AppData::BottomHeight - 15;

    //载入地图文件名称集合,按照名称升序排序
    AppData::MapPath = QUIHelper::appPath() + "/map";
    QStringList filter;
    filter << "*.jpg" << "*.bmp" << "*.png";
    QDir mapPath(AppData::MapPath);
    AppData::MapNames.append(mapPath.entryList(filter, QDir::Files, QDir::Name));
    DeviceMap::Instance()->loadMap();

    //载入声音文件名称集合
    AppData::SoundPath = QUIHelper::appPath() + "/sound";
    filter.clear();
    filter << "*.wav" << "*.mp3" << "*.mdi";
    QDir soundPath(AppData::SoundPath);
    AppData::SoundNames.append(soundPath.entryList(filter));

    //载入串口号
    for (int i = 1; i < 20; i++) {
        AppData::PortNames << QString("COM%1").arg(i);
    }

    AppData::PortNames << "ttyUSB0" << "ttyS0" << "ttyS1" << "ttyS2" << "ttyS3" << "ttyS4" << "ttyS5";

    //设置开机自动运行
    QString strPath = QApplication::applicationFilePath();
    strPath = QDir::toNativeSeparators(strPath);
    QUIHelper::runWithSystem(QUIHelper::appName(), strPath, AppConfig::AutoRun);

    //初始化数据库
    this->initDb();
    //初始化不同工作模式的全局设置
    this->initWorkMode();
    //校验自动登录
    this->autoLogin();
}

void AppInit::initDb()
{
    //设置数据库连接信息
    DbInfo dbInfo;
    dbInfo.connName = "qt_sql_default_connection";
    dbInfo.dbName = AppConfig::LocalDbName;
    dbInfo.hostName = AppConfig::LocalHostName;
    dbInfo.hostPort = AppConfig::LocalHostPort;    
    dbInfo.userName = AppConfig::LocalUserName;
    dbInfo.userPwd = AppConfig::LocalUserPwd;

    bool ok = true;
    QString dbType = AppConfig::LocalDbType.toUpper();
    if (dbType == "SQLITE") {
        dbInfo.dbName = DbHelper::getDbDefaultFile("cswms");
        if (QFile(dbInfo.dbName).size() <= 4) {
            ok = false;
        }
    }

    //如果打开失败则弹出数据库配置界面进行设置
    DbType type = DbHelper::getDbType(dbType);
    DbData::DbLocal = new DbConnThread;
    DbData::DbLocal->setConnInfo(type, dbInfo);
    if (!ok || !DbData::DbLocal->openDb()) {
        QUIWidget qui;
        qui.setOnlyCloseBtn();
        qui.setTitle("本地数据库配置");
        qui.setMainWidget(new frmConfigDb);
        QUIHelper::setFormInCenter(&qui);
        qui.exec();
        exit(0);
    } else {
        DbData::DbLocal->start();
    }

    //提前载入数据库数据
    DbQuery::loadUserInfo();
    DbQuery::loadPortInfo();
    DbQuery::loadDeviceInfo();
    DbQuery::loadNodeInfo();
    DbQuery::loadAlarmLink();
    DbQuery::loadTypeInfo();

    //找到记录数最大编号
    DbData::UserLogID = DbHelper::getMaxID("UserLog", "LogID");
    DbData::AlarmLogID = DbHelper::getMaxID("AlarmLog", "LogID");
    DbData::NodeLogID = DbHelper::getMaxID("NodeLog", "LogID");

    //测试数据
    DbQuery::addUserLog(50, "测试数据");

    //启动线程自动清除早期数据
    cleanAlarmLog = new DbCleanThread(this);
    cleanAlarmLog->setObjectName("cleanAlarmLog");
    cleanAlarmLog->setCleanInfo("AlarmLog", "LogID", "LogID", "LogID asc", AppConfig::AlarmLogMaxCount * 10000, 1 * 60);
    dbInfo.connName = "cleanAlarmLog";
    cleanAlarmLog->setConnInfo(type, dbInfo);

    cleanNodeLog = new DbCleanThread(this);
    cleanNodeLog->setObjectName("cleanNodeLog");
    cleanNodeLog->setCleanInfo("NodeLog", "LogID", "LogID", "LogID asc", AppConfig::NodeLogMaxCount * 10000, 2 * 60);
    dbInfo.connName = "cleanNodeLog";
    cleanNodeLog->setConnInfo(type, dbInfo);

    cleanUserLog = new DbCleanThread(this);
    cleanUserLog->setObjectName("cleanUserLog");
    cleanUserLog->setCleanInfo("UserLog", "LogID", "LogID", "LogID asc", AppConfig::UserLogMaxCount * 10000, 3 * 60);
    dbInfo.connName = "cleanUserLog";
    cleanUserLog->setConnInfo(type, dbInfo);

    //统一启动线程
    cleanAlarmLog->start();
    cleanNodeLog->start();
    cleanUserLog->start();

    //如果启用了云端远程数据库同步
    if (AppConfig::UseNetDb) {
        DbInfo dbInfo;
        dbInfo.connName = "qt_sql_tcp_connection";
        dbInfo.dbName = AppConfig::NetDbName;
        dbInfo.hostName = AppConfig::NetHostName;
        dbInfo.hostPort = AppConfig::NetHostPort;        
        dbInfo.userName = AppConfig::NetUserName;
        dbInfo.userPwd = AppConfig::NetUserPwd;

        DbType type = DbHelper::getDbType(AppConfig::NetDbType.toUpper());
        DbData::DbNet = new DbConnThread;
        DbData::DbNet->setDbFlag("远程");
        DbData::DbNet->setConnInfo(type, dbInfo);
        if (!DbData::DbNet->openDb()) {
            QUIWidget qui;
            qui.setOnlyCloseBtn();
            qui.setTitle("远程数据库配置");
            qui.setMainWidget(new frmConfigDbNet);
            QUIHelper::setFormInCenter(&qui);
            qui.exec();
            exit(0);
        } else {
            //根据本地数据库的信息生成初始化远程数据库临时表的数据
            //填充默认设备的初始数据
            QStringList listSql;
            //首先插入删除表语句
            listSql << "delete from NodeData";
            //再插入新建语句
            for (int i = 0; i < DbData::NodeInfo_Count; i++) {
                QString positionID = DbData::NodeInfo_PositionID.at(i);
                QString sql = QString("insert into NodeData(PositionID,NodeValue,NodeStatus,SaveTime) values('%1','0',0,'%2')").arg(positionID).arg(DATETIME);
                listSql << sql;
            }

            DbData::DbNet->initDb(listSql);
            DbData::DbNet->start();
        }
    }
}

void AppInit::initWorkMode()
{
    if (AppConfig::WorkMode == 2) {
        AppData::DefaultPositionID = "AT-10001";
        AppData::AlarmTypes = QString::fromUtf8("报警|高报|失效").split("|");
        AppData::NodeDataColumns = QString::fromUtf8("编号|位号|控制器名称|变量名称|数值|符号|保存时间");
        AppData::TableDataColumns = QString::fromUtf8("序号|位号|控制器名称|控制器型号|变量名称|变量型号|种类|实时数值|单位|状态");
        AppData::TypeInfoColumns = QString::fromUtf8("编号|控制器型号|变量数量|变量型号|种类|符号");
        AppData::NodeInfoColumns = QString::fromUtf8("编号|位号|控制器名称|变量名称|变量描述|读写类型|数据类型|寄存器区|寄存器|型号|种类|单位|报警值|高报值|失效值|消零|量程|状态|声音|地图|存储|小数点|报警启用|报警延时|报警类型|高报值|低报值|X坐标|Y坐标");
    } else if (AppConfig::WorkMode == 3) {
        AppData::AlarmTypes = QString::fromUtf8("低报|高报|故障").split("|");
    }

    AppData::PermissionName.clear();
    AppData::PermissionName << "系统设置" << "删除记录" << "曲线监控" << "数据查询" << "模块A" << "模块B" << "模块C";
}

void AppInit::autoLogin()
{
    if (AppConfig::AutoLogin) {
        AppData::CurrentUserName = AppConfig::LastLoginer;
        DbQuery::getUserInfo(AppData::CurrentUserName, AppData::CurrentUserPwd, AppData::CurrentUserType, AppData::UserPermission);
        DbQuery::addUserLog("用户自动登录");
    }
}

void AppInit::changeDbMaxCount()
{
    cleanAlarmLog->setMaxCount(AppConfig::AlarmLogMaxCount * 10000);
    cleanNodeLog->setMaxCount(AppConfig::NodeLogMaxCount * 10000);
    cleanUserLog->setMaxCount(AppConfig::UserLogMaxCount * 10000);
}
