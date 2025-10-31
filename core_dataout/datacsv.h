#ifndef DATACSV_H
#define DATACSV_H

#include "datahead.h"

class DataCsv
{
public:
    //保存文件对应的分隔符+拓展名+过滤条件
    static QString CsvFilter;
    static QString CsvExtension;
    static QString CsvSpliter;

    //获取指定表名字段数据内容
    static QStringList getContent(const QString &tableName,
                                  const QString &columnNames,
                                  const QString &whereSql = QString(),
                                  const QString &title = QString(),
                                  const QString &spliter = DataCsv::CsvSpliter);
    //导入数据文件 字段名为空则表示所有字段
    static bool inputData(int columnCount,
                          const QString &columnNames,
                          const QString &tableName,                          
                          QString &fileName,
                          const QString &defaultDir = QCoreApplication::applicationDirPath(),
                          bool existTitle = false);
    //导出数据文件
    static bool outputData(const QString &defaultName,
                           const QStringList &content,
                           QString &fileName,
                           const QString &defaultDir = QCoreApplication::applicationDirPath());
    //导出数据
    static bool outputData(QString &fileName, const QStringList &content);
};

#endif // DATACSV_H
