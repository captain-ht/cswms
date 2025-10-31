#include "ProjectDataSQLiteDatabase.h"
#include <QDateTime>
#include <QSqlQuery>
#include "ulog.h"

ProjectDataSQLiteDatabase::ProjectDataSQLiteDatabase(const QString &dbnamePrj,
                                                     const QString &dbnameHis,
                                                     const QString &user,
                                                     const QString &pwd,
                                                     const QString &hostname,
                                                     int port,
                                                     QObject *parent)
    : SQLiteDatabase(dbnamePrj, dbnameHis, user, pwd, hostname, port, parent)
{

}

ProjectDataSQLiteDatabase::~ProjectDataSQLiteDatabase()
{

}


/**
 * @brief ProjectDataSQLiteDatabase::createTableSystemParameters
 * @details 创建系统参数表
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::createTableSystemParameters()
{
    QString autoincrement = "integer not null primary key autoincrement";
    QStringList keyList,valueList;
    int ret = 0;

    keyList.clear();
    valueList.clear();

    keyList << "id" << "project_encrypt" << "data_scan_period" << "device_type"
            << "page_scan_period" << "project_description" << "project_name"
            << "project_path" << "start_page" << "station_address" << "station_number";

    valueList << autoincrement << "int" << "int" << "varchar(32)"
              << "int" << "varchar(1024)" << "varchar(128)"
              << "varchar(512)" << "varchar(32)" << "varchar(32)"<< "int";

    QSqlQuery query(dbPrj_);
    ret = createTable(query, "t_system_parameters", keyList, valueList, "");

    if(ret == 1) {
        excSQL("delete from t_system_parameters");
        excSQL("insert into t_system_parameters values(1, 0, 500, 'TPC(800*600)', 500, 'demo1 project', '', '', 'NONE', '127.0.0.1', 0);");
    }

    return ret;
}


/**
 * @brief ProjectDataSQLiteDatabase::createTableNetSetting
 * @details 创建组网设置表
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::createTableNetSetting()
{
    QString autoincrement = "integer not null primary key autoincrement";
    QStringList keyList,valueList;
    int ret = 0;

    keyList.clear();
    valueList.clear();

    keyList << "id" << "hot_standby_mode" << "client_mode" << "server_station"
            << "client_station" << "client_address" << "server_address"
            << "heartbeat_time" << "database_sync_time";

    valueList << autoincrement << "int" << "int" << "int"
              << "int" << "varchar(32)" << "varchar(32)"
              << "int" << "int";

    QSqlQuery query(dbPrj_);
    ret = createTable(query, "t_net_setting", keyList, valueList, "");
    if(ret == 1) {
        excSQL("delete from t_net_setting");
        excSQL("insert into t_net_setting values(1, 0, 0, 0, 0, '', '', 0, 0);");
    }

    return ret;
}


/**
 * @brief ProjectDataSQLiteDatabase::createTableDatabaseSetting
 * @details 创建数据库设置表
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::createTableDatabaseSetting()
{
    QString autoincrement = "integer not null primary key autoincrement";
    QStringList keyList,valueList;
    int ret = 0;

    keyList.clear();
    valueList.clear();
    keyList << "id" << "alarm_size" << "special_db" << "auto_delete"
            << "db_type" << "data_keep_days" << "ip_address" << "use_sd"
            << "user" << "save_period" << "password" << "send_period"
            << "db_name" << "start_time" << "port";

    valueList << autoincrement << "int" << "int" << "int"
              << "varchar(32)" << "int" << "varchar(32)" << "int"
              << "varchar(32)" << "int" << "varchar(32)" << "int"
              << "varchar(32)" << "int" << "int";

    QSqlQuery query(dbPrj_);
    ret = createTable(query,"t_database_setting", keyList, valueList, "");
    if(ret == 1) {
        excSQL("delete from t_database_setting");
        excSQL("insert into t_database_setting values(1, 50, 0, 0, 'SQLITE', 10, '127.0.0.1', 0, 'root', 60, '123456', 500, 'HisData.db', 1, 3306)");
    }

    return ret;
}


/**
 * @brief ProjectDataSQLiteDatabase::createTableUserAuthority
 * @details 创建用户权限表
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::createTableUserAuthority()
{
    QString autoincrement = "integer not null primary key autoincrement";
    QStringList keyList,valueList;
    int ret = 0;

    keyList.clear();
    valueList.clear();

    keyList << "id" << "number" << "name" << "password"
            << "authority" << "comments" << "name2" << "name3"
            << "name4" << "name5" << "login" << "logout";

    valueList << autoincrement << "varchar(4)" << "varchar(32)" << "varchar(16)"
              << "varchar(2)" << "varchar(512)" << "varchar(32)" << "varchar(32)"
              << "varchar(32)" << "varchar(32)" << "int" << "int";

    QSqlQuery query(dbPrj_);
    ret = createTable(query, "t_user_authority", keyList, valueList, "");
    if(ret == 1) {

    }

    return ret;
}


/**
 * @brief ProjectDataSQLiteDatabase::createTableDeviceInfo
 * @details 创建设备信息表
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::createTableDeviceInfo()
{
    QString autoincrement = "integer not null primary key autoincrement";
    QStringList keyList,valueList;
    int ret = 0;

    keyList.clear();
    valueList.clear();

    keyList << "id" << "type" << "name" << "frame_length"
            << "protocol" << "link" << "state_var" << "frame_time_period"
            << "ctrl_var" << "dynamic_optimization" << "remote_port" << "port_parameters";

    valueList << autoincrement << "varchar(8)" << "varchar(64)" << "int"
              << "varchar(64)" << "varchar(32)" << "int" << "int"
              << "int" << "int" << "int" << "varchar(512)";

    QSqlQuery query(dbPrj_);
    ret = createTable(query, "t_device_info", keyList, valueList, "");
    if(ret == 1) {

    }

    return ret;
}


/**
 * @brief ProjectDataSQLiteDatabase::createTablePictureResourceInfo
 * @details 图片资源信息表
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::createTablePictureResourceInfo()
{
    QString autoincrement = "integer not null primary key autoincrement";
    QStringList keyList,valueList;
    int ret = 0;

    keyList.clear();
    valueList.clear();

    keyList << "id" << "name" << "ref_count";

    valueList << autoincrement << "varchar(64)" << "int";

    QSqlQuery query(dbPrj_);
    ret = createTable(query, "t_picture_resource_info", keyList, valueList, "");
    if(ret == 1) {

    }

    return ret;
}

/**
 * @brief ProjectDataSQLiteDatabase::createTableHisData
 * @details 创建历史数据表
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::createTableHisData(QString currentDate, QStringList keyList,QStringList valueList)
{
    QString autoincrement = "integer not null primary key autoincrement";
    QStringList keyList2,valueList2;
    int ret = 0;

    keyList2.clear();
    valueList2.clear();

    keyList2 << "id" << "timestamp";
    valueList2 << autoincrement << "DateTime not null primary key";

    hdataTableName = "t_hdata_" + QDateTime::currentDateTime().toString("yyyyMMdd");
    QSqlQuery query(dbHis_);

    ret = createTable(query, hdataTableName, keyList, valueList, "");
    ret = createTable(query, "t_hdata_transferFailed", keyList2, valueList2, "");

    QStringList tableList;
    ret = getTableInfo(query, tableList);

    for(int i=0; i<tableList.count(); i++)
    {
        QString tableName = tableList.at(i);
        if(tableName.contains("t_hdata_") and !tableName.contains("transferFailed") and hdataTableName != tableName)
        {
            ret = createTable(query, tableName, keyList, valueList, "");
        }
    }

    return ret;
}


/**
 * @brief ProjectDataSQLiteDatabase::createTables
 * @details 创建工程参数表
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::createTables()
{
#define RET_MAX    (16)
    int i = 0;
    bool ret[RET_MAX];
    for(i=0; i<RET_MAX; i++)
        ret[i] = true;

    // 创建系统参数表
    ret[0] = createTableSystemParameters();
    // 创建组网设置表
    ret[1] = createTableNetSetting();
    // 创建数据库设置表
    ret[2] = createTableDatabaseSetting();
    // 创建用户权限表
    ret[3] = createTableUserAuthority();
    // 创建设备信息表
    ret[4] = createTableDeviceInfo();
    // 图片资源信息表
    ret[5] = createTablePictureResourceInfo();

    for(i=0; i<RET_MAX; i++) {
        if(!ret[i])
            return false;
    }

    return true;
}


/**
 * @brief ProjectDataSQLiteDatabase::insterHisData
 * @details 存储历史数据
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::insterHisData(QStringList fieldList,
                                              QStringList valueList)
{

    int ret = 0;
    QSqlQuery query(dbHis_);
    ret = insertData(query, hdataTableName, fieldList, valueList);
    return ret;
}

/**
 * @brief ProjectDataSQLiteDatabase::insterDataSynchro
 * @details 存储数据同步
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::insterDataSynchro(QStringList fieldList,
                                              QStringList valueList)
{
    int ret = 0;
    QSqlQuery query(dbHis_);
    QString timeStamp = valueList.at(0);
    QDateTime time;
    time = QDateTime::fromString(timeStamp, "yyyy-MM-dd hh:mm:ss");
    QString hisTable = "t_hdata_" + time.toString("yyyyMMdd");

    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(hisTable));
    if (!query.next() || (query.value(0).toInt() == 0))
    {
        QStringList fieldListTemp,valueListTemp;
        QString autoincrement = "integer not null primary key autoincrement";
        fieldListTemp.clear();
        valueListTemp.clear();

        fieldListTemp << "id" << fieldList;
        valueListTemp << autoincrement << "DateTime";
        for (int i = 1; i < fieldList.count(); ++i)
        {
            valueListTemp << "real";
        }

        LogInfo(QString("no find table, create table: %1!").arg(hisTable));
        ret = createTable(query, hisTable, fieldListTemp, valueListTemp, "");
    }
    //如果不存在就插入，存在就忽略
    ret = insertOrIgnoreData(query, hisTable, fieldList, valueList);
    return ret;
}

/**
 * @brief ProjectDataSQLiteDatabase::insterTraFailData
 * @details 存储上传失败数据
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::insterTraFailData(QString timeStamp)
{
    QStringList fieldList,valueList;
    int ret = 0;

    fieldList.clear();
    valueList.clear();

    fieldList << "timestamp";
    valueList << timeStamp;

    QSqlQuery query(dbHis_);
    ret = insertData(query, "t_hdata_transferFailed", fieldList, valueList);
    return ret;
}

/**
 * @brief ProjectDataSQLiteDatabase::removeTransferFailedData
 * @details 上传完成后删除上传失败数据
 * @return true-成功, false-失败
 */
bool ProjectDataSQLiteDatabase::removeTransferFailedData(QString timeStamp)
{
    int ret = 0;
    QSqlQuery query(dbHis_);
    ret = deleteData(query, "t_hdata_transferFailed", "timestamp", timeStamp);
    return ret;
}

/**
 * @brief ProjectDataSQLiteDatabase::reqDataSynchro
 * @details 请求数据同步查询
 * @return 待同步数据
 */
QList<QStringList> ProjectDataSQLiteDatabase::reqDataSynchro(QStringList keyList, int num)
{
    QSqlQuery query(dbHis_);
    return sqlDataSynchro(query, "t_hdata_transferFailed", keyList, num);
}
