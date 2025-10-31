#ifndef _PROJECTDATA_H
#define _PROJECTDATA_H

#include <QMutex>
#include <QObject>
#include <QString>
#include "ProjectInfoManager.h"
#include "NetSetting.h"
#include "DatabaseSetting.h"
#include "UserAuthority.h"
#include "DeviceInfo.h"
#include "PictureResourceManager.h"

class ProjectDataSQLiteDatabase;

class ProjectData
{

public:
    static ProjectData* getInstance();
    static void releaseInstance();
    // 创建或打开工程数据
    bool createOrOpenProjectData(const QString &projPath,const QString &projName);
    //创建或打开历史数据库
    bool createOrOpenHisData(QString currentDate, QStringList fieldList,QStringList valueList);
    // 获取工程数据文件路径
    QString getDBPath() const;
    // 存储历史数据库数据
    bool insterHisData(QStringList fieldList,QStringList valueList);
    // 存储数据同步
    bool insterDataSynchro(QStringList fieldList,QStringList valueList);
    // 存储上传失败数据
    bool insterTraFailData(QString timeStamp);
    // 每日创建一个历史分表
    bool createDayTable(QString currentDate, QStringList fieldList,QStringList valueList);
    //上传完成后删除上传失败数据
    bool removeTransferFailedData(QString timeStamp);
    //请求数据同步查询
    QList<QStringList> reqDataSynchro(QStringList keyList, int num);

public:
    static ProjectDataSQLiteDatabase *dbData_;
    static QString szProjPath_;
    static QString szProjName_;
    ProjectInfoManager projInfoMgr_; // 工程信息管理
    NetSetting netSetting_; // 网络配置
    DatabaseSetting dbSetting_; // 数据库配置
    UserAuthority userAuthority_; // 用户权限
    DeviceInfo deviceInfo_; // 设备配置信息
    PictureResourceManager pictureResourceMgr_; // 图片资源管理

private:
    explicit ProjectData();
    ~ProjectData();

private:  
    static ProjectData *instance_;
    static QMutex mutex_;
    QString dbPath_;
    Q_DISABLE_COPY(ProjectData)
};


#endif // _PROJECTDATA_H
