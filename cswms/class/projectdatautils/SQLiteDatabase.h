#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <Database.h>

class SQLiteDatabase : public Database
{
    Q_OBJECT
public:
    explicit SQLiteDatabase(const QString &dbnamePrj = "sqlitedb",
                            const QString &dbnameHis = "sqlitedb",
                            const QString &user = "root",
                            const QString &pwd = "725431",
                            const QString &hostname = "127.0.0.1",
                            int port = 0,
                            QObject *parent = 0);

    virtual ~SQLiteDatabase();

    bool openDatabase() override;
    bool closeDatabase() override;
    bool isOpenDatabase() override;
    bool createDatabase() override;
    //when table is not exist then create it
    virtual bool createTables();

    int createTable(QSqlQuery query,
                    const QString &table,
                    QStringList fieldList,
                    QStringList typeList,
                    const QString &index="") override;

    bool insertOrUpdateRecord(const QString &table,
                              const QStringList &keyList,
                              const QStringList &valueList) override;

    bool getTableInfo(QSqlQuery query, QStringList &tableList);

    bool isContain(QSqlQuery query,const QString& table);

    int insertData(QSqlQuery query,const QString &table,QStringList fieldList,QStringList valueList);

    int insertOrIgnoreData(QSqlQuery query,const QString &table,QStringList fieldList,QStringList valueList);

    int deleteData(QSqlQuery query,const QString &table,QString field,QString timeStamp);

    QList<QStringList> sqlDataSynchro(QSqlQuery query,const QString &table, QStringList keyList, int num);

    bool getRecordByKey(QSqlQuery query, const QString &table,
                        const QString &key,
                        QStringList &valueList,
                        const QString &expr);
    bool getRecordByKey(QSqlQuery query, const QString &table,
                        const QStringList &keyList,
                        QStringList &valueList,
                        const QString &expr);

};

#endif // SQLITEDATABASE_H
