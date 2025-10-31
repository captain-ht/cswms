#include "SQLiteDatabase.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include "ulog.h"

SQLiteDatabase::SQLiteDatabase(const QString &dbnamePrj,
                               const QString &dbnameHis,
                               const QString &user,
                               const QString &pwd,
                               const QString &hostname,
                               int port,
                               QObject *parent)
    : Database(dbnamePrj, dbnameHis, user, pwd, hostname, port, parent)
{
    try {
        if(QSqlDatabase::contains("sqlite_connection1") and QSqlDatabase::contains("sqlite_connection1")) {
            dbPrj_ = QSqlDatabase::database("sqlite_connection1");
            dbHis_ = QSqlDatabase::database("sqlite_connection2");
        } else {
            dbPrj_ = QSqlDatabase::addDatabase("QSQLITE", "sqlite_connection1");
            dbHis_ = QSqlDatabase::addDatabase("QSQLITE", "sqlite_connection2");
        }
    } catch(...) {
        LogError(QString("Add Database Failed Exception!"));
    }
}

SQLiteDatabase::~SQLiteDatabase() { closeDatabase(); }

bool SQLiteDatabase::openDatabase()
{
    closeDatabase();
    try {
        dbPrj_.setDatabaseName(namePrj_);
        dbHis_.setDatabaseName(nameHis_);
        if(!dbPrj_.open()) {
            LogError(QString("Open Project Database Failed!"));
            return false;
        }
        if(!dbHis_.open()) {
            LogError(QString("Open History Database Failed!"));
            return false;
        }
    } catch(...) {
        LogError(QString("Open Database Failed Exception!"));
        return false;
	}
	return true;
}

bool SQLiteDatabase::closeDatabase()
{
    if(dbPrj_.isOpen()) {
        dbPrj_.close();
    }
    if(dbHis_.isOpen()) {
        dbHis_.close();
    }
    return false;
}

bool SQLiteDatabase::isOpenDatabase()
{
    QSqlQuery query(dbPrj_);
	bool ret = false;
    try {
        ret = query.exec("select CURRENT_TIMESTAMP");
        if(!ret) ret = openDatabase();
    } catch (...) {
        LogError(QString("Open Database: %1 Failed Exception!").arg(namePrj_));
		return false;
	}
	return ret;
}

bool SQLiteDatabase::createDatabase() { return true; }

bool SQLiteDatabase::getTableInfo(QSqlQuery query, QStringList &tableList)
{
    if(!query.exec(QString("select name from sqlite_master order by name")))
    {
        LogError(QString("getTableInfo:failed!"));
        return false;
    }

    while (query.next()) {
      tableList << query.record().value("name").toString();
    }
    return true;
}

int SQLiteDatabase::createTable(QSqlQuery query,
                                const QString &table,
                                QStringList fieldList,
                                QStringList typeList,
                                const QString &index)
{
    QString key = QString();
    int pos = 0, ret = 0;
    int count = fieldList.count();
    
    if(table.isEmpty() || count == 0 || count != typeList.count()) {
        LogError(QString("create table: %1 failed!").arg(table));
        return 0;
    }

    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(table));

    if (!query.next() || (query.value(0).toInt() == 0)) {
        for (int i = 0; i < fieldList.count(); ++i)
            key += fieldList.at(i) + " " + typeList.at(i) + ",";

        if(index.isEmpty()) key.chop(1);
        else key += QString("index(%1)").arg(index);

        if(!query.exec(QString("create table %1(%2)").arg(table,key))) {
            LogError(QString("create table: %1 failed! deatil: %2!").arg(table).arg(query.lastError().text()));
            return 0;
        }
        return 1;
    } else {
        QStringList oldFieldList,oldTypeList;
        query.exec(QString("pragma table_info (%1)").arg(table));

        while (query.next()) {
            oldFieldList << query.record().value("name").toString();
            oldTypeList << query.record().value("Type").toString();
        }

        for(int i=0;i<count;i++) {
            pos = oldFieldList.indexOf(fieldList.at(i));
            if(pos == -1) {
                if(!query.exec(QString("alter table %1 add column %2 %3")
                               .arg(table,fieldList.at(i),typeList.at(i)))) {
                    LogError(QString("alter table: %1 failed! deatil: Failed or Exception").arg(table));
                    return 0;
                }
                ret = 2;
            }
        }
        return ret;
    }
    return 4;
}

bool SQLiteDatabase::createTables() { return false; }

bool SQLiteDatabase::insertOrUpdateRecord(const QString &table,
                                          const QStringList &keyList,
                                          const QStringList &valueList)
{
    QSqlQuery query(dbPrj_);
    QString key,value,update,sql;
    int count = keyList.count();

    if(!count || count != valueList.count()) {
        LogError(QString("insert record to %1 failed! keylist.count != valuelist.count").arg(table));
        return false;
    }

  for (int i = 0; i < count; ++i) {
    key += keyList.at(i) + ",";
    value += "'" + valueList.at(i) + "',";
  }
  key.chop(1);
  value.chop(1);

    sql = QString("replace into %1(%2) value(%3)")
            .arg(table)
            .arg(key)
            .arg(value);
  
    try {
        if(!query.exec(sql)) {
            LogError(QString("insert record to %1 failed! %2, error: %3")
                     .arg(table)
                     .arg(sql)
                     .arg(query.lastError().text()));
            return false;
        }
    } catch (...) {
        LogError(QString("insert record to %1 failed! %2, Exception")
                 .arg(table)
                 .arg(sql));
        return false;
    }
    return true;
}



bool SQLiteDatabase::isContain(QSqlQuery query,const QString& table)
{
    bool ret = false;

    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(table));

    if (!query.next() || query.value(0).toInt() == 0)
        ret = false;
    else
        ret = true;

    return ret;
}

int SQLiteDatabase::insertData(QSqlQuery query,
                               const QString &table,
                               QStringList fieldList,
                               QStringList valueList)
{
    QString field = QString();
    QString value = QString();

    int count = fieldList.count();
    if(table.isEmpty() || count == 0 || count != valueList.count()) {
        LogError(QString("insert into: %1 failed!").arg(table));
        return 0;
    }

    for (int i = 0; i < fieldList.count(); ++i)
    {
        field += fieldList.at(i) + ",";
        value += "'" + valueList.at(i) + "'" + ",";
    }
    field.chop(1);
    value.chop(1);

    QString sql = QString("insert into %1 (%2) values(%3)").arg(table,field,value);
    if(!query.exec(sql)) {
        LogError(QString("%1 failed! deatil: %2!").arg(sql).arg(query.lastError().text()));
        return 0;
    }
    return 1;
}

//如果不存在就插入，存在就忽略
int SQLiteDatabase::insertOrIgnoreData(QSqlQuery query,
                               const QString &table,
                               QStringList fieldList,
                               QStringList valueList)
{
    QString field = QString();
    QString value = QString();

    int count = fieldList.count();
    if(table.isEmpty() || count == 0 || count != valueList.count()) {
        LogError(QString("insert or Ignore into: %1 failed!").arg(table));
        return 0;
    }

    for (int i = 0; i < fieldList.count(); ++i)
    {
        field += fieldList.at(i) + ",";
        value += "'" + valueList.at(i) + "'" + ",";
    }
    field.chop(1);
    value.chop(1);

    QString sql = QString("insert or ignore into %1 (%2) values(%3)").arg(table,field,value);
    if(!query.exec(sql)) {
        LogError(QString("%1 failed! deatil: %2!").arg(sql).arg(query.lastError().text()));
        return 0;
    }
    return 1;
}

int SQLiteDatabase::deleteData(QSqlQuery query,
                               const QString &table,
                               QString field,
                               QString timeStamp)
{
    QString sql = QString("delete from %1 where %2='%3'").arg(table,field,timeStamp);
    if(!query.exec(sql)) {
        LogError(QString("%1 failed! deatil: %2!").arg(sql).arg(query.lastError().text()));
        return 0;
    }
    return 1;
}

QList<QStringList> SQLiteDatabase::sqlDataSynchro(QSqlQuery query,
                                                  const QString &table,
                                                  QStringList keyList,
                                                  int num)
{
    QList<QStringList> valueListList;

    QString key = keyList.join(",");

    QString expr = nullptr;
    int iCnt = getRowCount(query, table, expr);

    if(iCnt > 1)
    {
        QStringList timeList;
        expr = QString("order by timeStamp asc limit 0,%1").arg(num);
        if(getRecordByKey(query, table, "timeStamp", timeList, expr))
        {
            for (int i = 0; i < timeList.count(); ++i)
            {
                QStringList valueList;
                QString timeStamp = timeList.at(i);
                QDateTime time;
                time = QDateTime::fromString(timeStamp, "yyyy-MM-dd hh:mm:ss");
                QString hisTable = "t_hdata_" + time.toString("yyyyMMdd");
                expr = QString("timeStamp='%1'").arg(timeStamp);
                if(getRecordByKey(query, hisTable, keyList, valueList, expr))
                {
                    if(keyList.count() == valueList.count())
                        valueListList << valueList;
                }
            }
        }
    }
    return valueListList;
}

bool SQLiteDatabase::getRecordByKey(QSqlQuery query,
                                    const QString &table,
                                    const QString &key,
                                    QStringList &valueList,
                                    const QString &expr)
{
    QString sql;

    if (key.isEmpty()) return false;

    if (expr.isEmpty())
        sql = QString("select %1 from %2").arg(key).arg(table);
    else
        sql = QString("select %1 from %2 %3").arg(key).arg(table).arg(expr);

    try {
        if(!query.exec(sql)) {
            LogError(QString("get record: %1 failed! %2 ,error: %3!")
                     .arg(table)
                     .arg(sql)
                     .arg(query.lastError().text()));
            return false;
        }
        //return the first one value
        while(query.next())
        {
            valueList << query.record().value(key).toString();
        }

    } catch (...) {
        LogError(QString("get record: %1 failed! Exception: %2")
                 .arg(table)
                 .arg(sql));
        return false;
    }
    return true;
}

bool SQLiteDatabase::getRecordByKey(QSqlQuery query,
                                    const QString &table,
                                    const QStringList &keyList,
                                    QStringList &valueList,
                                    const QString &expr)
{
    QString key,sql;

    if (keyList.isEmpty())
        return false;

    key = keyList.join(",");

    if (expr.isEmpty())
        sql = QString("select %1 from %2").arg(key).arg(table);
    else
        sql = QString("select %1 from %2 where %3").arg(key).arg(table).arg(expr);

    try {
        if(!query.exec(sql)) {
            LogError(QString("get record: %1 failed! %2 ,error: %3!")
                     .arg(table)
                     .arg(sql)
                     .arg(query.lastError().text()));
            return false;
        }
        if(query.next()) {
            for(int i = 0; i < keyList.count(); ++i)
                valueList << query.record().value(keyList.at(i)).toString();
        }
    } catch (...) {
        LogError(QString("get record: %1 failed! Exception: %2")
                 .arg(table)
                 .arg(sql));
        return false;
    }
    return true;
}




