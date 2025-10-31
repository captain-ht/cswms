#include "appfun.h"
#include "dbquery.h"

void AppFun::reboot()
{
    AppData::IsReboot = true;
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
    qApp->closeAllWindows();
}

void AppFun::saveSelectDirName(const QString &fileName)
{
    //自动保存最后一次选择的文件夹路径
    if (fileName.length() > 0) {
        QFileInfo fileInfo(fileName);
        AppConfig::SelectDirName = fileInfo.absolutePath();
        AppConfig::writeConfig();
    }
}

QString AppFun::getSaveFileName(const QString &defaultName, const QString &filter)
{
    QString name = QString("%1/%2").arg(AppConfig::SelectDirName).arg(defaultName);
    QString fileName = QFileDialog::getSaveFileName(0, "保存", name, filter);
    saveSelectDirName(fileName);
    return fileName;
}

QString AppFun::getDefaultDir()
{
#ifdef __arm__
    QString defaultDir = "/udisk";
#ifdef arma7
    //默认插入U盘后,会自动在/media/root下生成对应文件夹,该文件夹下还有固定的boot rootfs两个文件夹
    defaultDir = "/media/root";
    QDir dir(defaultDir);
    QStringList list = dir.entryList();
    foreach (QString name, list) {
        if (!name.startsWith("boot") && !name.startsWith("root") && !name.startsWith("BOOT") && !name.contains(".")) {
            defaultDir = defaultDir + "/" + name;
            break;
        }
    }
#endif
#else
    QString defaultDir = QUIHelper::appPath();
#endif
    return defaultDir;
}

void AppFun::inputData(QSqlTableModel *model, const QStringList &columnNames, const QString &tableName, const QString &flag)
{
    QString fileName;
    bool ok = DataCsv::inputData(columnNames.count(), "", tableName, fileName, AppConfig::SelectDirName, true);
    if (!fileName.isEmpty()) {
        QString msg = QString("导入%1信息").arg(flag);
        DbQuery::addUserLog("用户操作", msg);
        if (ok) {
            QUIHelper::showMessageBoxInfo(msg + "成功!", 3);
            model->select();
        } else {
            QUIHelper::showMessageBoxError(msg + "失败!", 3);
        }
    }

    saveSelectDirName(fileName);
}

void AppFun::outputData(const QString &orderColumn, const QStringList &columnNames, const QString &tableName, const QString &flag)
{
    QString columns = "*";
    QString where = orderColumn.isEmpty() ? "" : QString("order by %1").arg(orderColumn);
    QString title = columnNames.join(DataCsv::CsvSpliter);
    QStringList content = DataCsv::getContent(tableName, columns, where, title);

    QString fileName;
    bool ok = DataCsv::outputData(flag + "信息", content, fileName, AppConfig::SelectDirName);
    if (!fileName.isEmpty()) {
        QString msg = QString("导出%1信息").arg(flag);
        DbQuery::addUserLog("用户操作", msg);
        if (ok) {
            QUIHelper::showMessageBoxInfo(msg + "成功!", 3);
        } else {
            QUIHelper::showMessageBoxError(msg + "失败!", 3);
        }
    }

    saveSelectDirName(fileName);
}

bool AppFun::checkPermission(const QString &text)
{
    //从权限模块名称找到当前模块是否需要授权
    int index = AppData::PermissionName.indexOf(text);
    if (index >= 0) {
        if (!AppData::UserPermission.at(index)) {
            QUIHelper::showMessageBoxError(QString("当前用户没有 [%1] 权限!").arg(text), 3);
            return false;
        }
    }

    return true;
}
