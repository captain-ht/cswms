#ifndef PROJECTDATASQLITEBASE_H
#define PROJECTDATASQLITEBASE_H
#include "SQLiteDatabase.h"

class ProjectDataSQLiteDatabase : public SQLiteDatabase
{
public:
    explicit ProjectDataSQLiteDatabase(const QString &dbnamePrj = "DefaultProject.pdt",
                                       const QString &dbnameHis = "HisData.db",
                                       const QString &user = "root",
                                       const QString &pwd = "725431",
                                       const QString &hostname = "127.0.0.1",
                                       int port = 3306,
                                       QObject *parent = 0);
    ~ProjectDataSQLiteDatabase();

    // 创建系统参数表
    bool createTableSystemParameters();
    // 创建组网设置表
    bool createTableNetSetting();
    // 创建数据库设置表
    bool createTableDatabaseSetting();
    // 创建用户权限表
    bool createTableUserAuthority();
    // 创建设备信息表
    bool createTableDeviceInfo();
    // 图片资源信息表
    bool createTablePictureResourceInfo();

    // 创建数据库表
    bool createTables() override;

    // 创建历史数据表
    bool createTableHisData(QString currentDate, QStringList keyList,QStringList valueList);
    // 存储历史数据
    bool insterHisData(QStringList fieldList,QStringList valueList);
    // 存储数据同步
    bool insterDataSynchro(QStringList fieldList,QStringList valueList);
    // 存储上传失败数据
    bool insterTraFailData(QString timeStamp);
    //上传完成后删除上传失败数据
    bool removeTransferFailedData(QString timeStamp);
    //请求数据同步查询
    QList<QStringList> reqDataSynchro(QStringList keyList, int num);

    QString hdataTableName;
};

#endif // PROJECTDATASQLITEBASE_H
