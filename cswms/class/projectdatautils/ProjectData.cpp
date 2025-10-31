#include "ProjectData.h"
#include "ProjectDataSQLiteDatabase.h"
#include <QMutexLocker>
#include <QMutex>
#include <cstdlib>
#include <QApplication>

#include <QDebug>

ProjectData* ProjectData::instance_ = nullptr;
ProjectDataSQLiteDatabase *ProjectData::dbData_ = nullptr;
QMutex ProjectData::mutex_;
QString ProjectData::szProjPath_ = "";
QString ProjectData::szProjName_ = "";


ProjectData::ProjectData() :
    dbPath_("")
{

}

ProjectData::~ProjectData()
{
    if(dbData_ != nullptr) {
        delete dbData_;
        dbData_ = nullptr;
    }
}


ProjectData* ProjectData::getInstance()
{
    QMutexLocker locker(&mutex_);
    if(instance_ == nullptr) {
        instance_ = new ProjectData();
        ::atexit(releaseInstance);
    }
    return instance_;
}

void ProjectData::releaseInstance()
{
    if(instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}


/**
 * @brief ProjectData::createOrOpenProjectData
 * @details 创建或打开工程数据
 * @param projFile 工程路径
 * @param projFile 工程名称
 * @return true-成功, false-失败
 */
bool ProjectData::createOrOpenProjectData(const QString &projPath,
                                          const QString &projName)
{
    QString fileNamePrj = projPath;
    QString fileNameHis = projPath;

    if(fileNamePrj.endsWith('/'))
        fileNamePrj.chop(1);

    szProjPath_ = fileNamePrj;
    szProjName_ = projName;

    if(szProjPath_.isEmpty() || szProjName_.isEmpty()) {
        qWarning() << __FILE__ << __LINE__ <<__FUNCTION__
                   << "szProjPath_: " << szProjPath_ << " "
                   << "szProjName_: " << szProjName_;
        return false;
    }

    fileNamePrj = szProjPath_ + "/" + szProjName_ + ".pdt";
    fileNameHis = QApplication::applicationDirPath() + "/DB/HisData.db";

    if(dbPath_ != "") {
        if(dbData_ != nullptr) {
            delete dbData_;
            dbData_ = nullptr;
        }
    }

    dbPath_ = fileNamePrj;
    dbData_ = new ProjectDataSQLiteDatabase(fileNamePrj,fileNameHis);
    if(dbData_->openDatabase()) {
        dbData_->createTables();
        return true;
    }

    return false;
}

/**
 * @brief ProjectData::createOrOpenHisData
 * @details 创建或打开历史数据库
 * @param projFile 历史数据库路径
 * @param projFile 历史数据库名称
 * @return true-成功, false-失败
 */
bool ProjectData::createOrOpenHisData(QString currentDate,
                                      QStringList fieldList,
                                      QStringList valueList)
{
    if(dbData_->createTableHisData(currentDate,fieldList,valueList)) {
        return true;
    }
    return false;
}

/**
 * @brief ProjectData::getDBPath
 * @details 获取工程数据文件路径
 * @return 工程数据文件路径
 */
QString ProjectData::getDBPath() const
{
    return dbPath_;
}

/**
 * @brief ProjectData::insterHisData
 * @details 存储历史数据库数据
 * @return true-成功, false-失败
 */
bool ProjectData::insterHisData(QStringList fieldList,
                                QStringList valueList)
{
    return dbData_->insterHisData(fieldList,valueList);
}

/**
 * @brief ProjectData::insterDataSynchro
 * @details 存储数据同步
 * @return true-成功, false-失败
 */
bool ProjectData::insterDataSynchro(QStringList fieldList,
                                QStringList valueList)
{
    return dbData_->insterDataSynchro(fieldList,valueList);
}

/**
 * @brief ProjectData::insterTraFailData
 * @details 存储历史数据库数据
 * @return true-成功, false-失败
 */
bool ProjectData::insterTraFailData(QString timeStamp)
{
    return dbData_->insterTraFailData(timeStamp);
}

/**
 * @brief ProjectData::createDayTable
 * @details 每日创建一个历史分表
 * @return true-成功, false-失败
 */
bool ProjectData::createDayTable(QString currentDate, QStringList keyList,QStringList valueList)
{
    if(dbData_->createTableHisData(currentDate,keyList,valueList)) {
        return true;
    }
    return false;
}

/**
 * @brief ProjectData::removeTransferFailedData
 * @details 上传完成后删除上传失败数据
 * @return true-成功, false-失败
 */
bool ProjectData::removeTransferFailedData(QString timeStamp)
{
    return dbData_->removeTransferFailedData(timeStamp);
}

/**
 * @brief ProjectData::reqDataSynchro
 * @details 请求数据同步查询
 * @return 待同步数据
// */
QList<QStringList> ProjectData::reqDataSynchro(QStringList keyList, int num)
{
    return dbData_->reqDataSynchro(keyList, num);
}

