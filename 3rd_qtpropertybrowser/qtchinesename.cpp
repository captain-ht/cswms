#pragma execution_character_set("utf-8")
#include "qtchinesename.h"
#include "qstringlist.h"
#include "qfile.h"
#include "qelapsedtimer.h"
#include "qdatetime.h"
#include "qdebug.h"

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))

QStringList QtChineseName::listPropertyName = QStringList();
QStringList QtChineseName::listPropertyClass = QStringList();
QStringList QtChineseName::listPropertyChinese = QStringList();

QStringList QtChineseName::listControlName = QStringList();
QStringList QtChineseName::listControlChinese = QStringList();

void QtChineseName::loadFile(const QString &fileName)
{
    //根据名称判断不同的类别
    int type = -1;
    if (fileName == ":/image/chinesename_property.txt") {
        type = 0;
    } else if (fileName == ":/image/chinesename_control.txt") {
        type = 1;
    }

    //开始计时
    QElapsedTimer time;
    time.start();

    //从文件加载英文属性与中文属性对照表
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        //QTextStream方法读取速度至少快30%
#if 0
        while (!file.atEnd()) {
            QString line = file.readLine();
            appendName(type, line);
        }
#else
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            appendName(type, line);
        }
#endif
        file.close();
    }

    //统计用时
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
    double elapsed = (double)time.nsecsElapsed() / 1000000;
#else
    double elapsed = (double)time.elapsed();
#endif
    QString strTime = QString::number(elapsed, 'f', 3);
    qDebug() << TIMEMS << QString("加载中文对照表完成,用时 %1 毫秒").arg(strTime);
}

void QtChineseName::appendName(int type, const QString &data)
{
    QString line = data;
    //过滤注释等
    if (type < 0 || line.contains("#") || line.contains("//")) {
        return;
    }

    //去掉空格和回车符
    line = line.trimmed();
    line.replace("\r", "");
    line.replace("\n", "");

    if (!line.isEmpty()) {
        QStringList list = line.split(",");

        //至少要有两列
        if (list.count() >= 2) {
            QString className = "";
            QString englishName = list.at(0).trimmed();
            QString chineseName = list.at(1).trimmed();

            //取出类名
            if (englishName.contains("|")) {
                QStringList list = englishName.split("|");
                if (list.count() == 2) {
                    englishName = list.at(0).trimmed();
                    className = list.at(1).trimmed();
                }
            }

            //都不能为空
            if (!englishName.isEmpty() && !chineseName.isEmpty()) {
                if (type == 0) {
                    listPropertyName << englishName;
                    listPropertyClass << className;
                    listPropertyChinese << chineseName;
                } else if (type == 1) {
                    listControlName << englishName;
                    listControlChinese << chineseName;
                }
            }
        }
    }
}

QString QtChineseName::getPropertyName(const QString &propertyName, const QString &className)
{
    int count = listPropertyName.count();

    //如果类名不为空则查找当前类名+属性名对应的属性别名
    if (!className.isEmpty()) {
        for (int i = 0; i < count; i++) {
            if (listPropertyClass.at(i) == className && listPropertyName.at(i) == propertyName) {
                return listPropertyChinese.at(i);
            }
        }
    }

    //如果没有找到属性别名则重新以属性名查找
    for (int i = 0; i < count; i++) {
        if (listPropertyName.at(i) == propertyName) {
            return listPropertyChinese.at(i);
        }
    }

    return propertyName;
}

QString QtChineseName::getControlName(const QString &controlName)
{
    //查找对应英文控件名称的中文名称
    int count = listControlName.count();
    for (int i = 0; i < count; i++) {
        if (listControlName.at(i) == controlName) {
            return listControlChinese.at(i);
        }
    }

    return controlName;
}
