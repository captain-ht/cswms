#ifndef APPFUN_H
#define APPFUN_H

#include "quihelper.h"

class AppFun
{
public:
    //系统重启
    static void reboot();

    //保存最后选择的文件夹
    static void saveSelectDirName(const QString &fileName);
    //获取保存文件名,传入默认文件名和过滤符
    static QString getSaveFileName(const QString &defaultName, const QString &filter);
    static QString getDefaultDir();

    //导入导出数据
    static void inputData(QSqlTableModel *model, const QStringList &columnNames,
                          const QString &tableName, const QString &flag);
    static void outputData(const QString &orderColumn, const QStringList &columnNames,
                           const QString &tableName, const QString &flag);

    //校验权限
    static bool checkPermission(const QString &text);
};

#endif // APPFUN_H
