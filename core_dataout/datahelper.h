#ifndef DATAHELPER_H
#define DATAHELPER_H

#include "datahead.h"

class DataHelper
{    
public:
    //校验列是否满足规则,可以自行拓展其他规则
    static bool CheckColumn(const QString &sourceValue, const QString &checkType, const QString &checkValue);
    //通用数据导出+打印函数
    static void Init();
    static void DataOut(const DataContent &dataContent);
    static void DataOut(const QString &fileName,
                        const QString &sheetName,
                        const QString &title,
                        const QList<QString> &columnNames,
                        const QList<int> &columnWidths,
                        const QStringList &content,
                        bool landscape = false);
};

#endif // DATAHELPER_H
