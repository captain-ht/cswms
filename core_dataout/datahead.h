#ifndef DATAHEAD_H
#define DATAHEAD_H

#include <QtGui>
#include <QtSql>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#include <QtPrintSupport>
#endif

#pragma execution_character_set("utf-8")

#ifndef TIMEMS
#define TIMEMS qPrintable (QTime::currentTime().toString("HH:mm:ss zzz"))
#endif
#ifndef DATETIME
#define DATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#endif
#ifndef STRDATETIME
#define STRDATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#endif

struct DataContent {
    QString html;               //可外部直接传入html
    QString fileName;           //文件名称 导出到xls和pdf需要
    QString sheetName;          //表名 导出到xls需要

    QString title;              //主标题
    QString subTitle;           //副标题

    QStringList subTitle1;      //子标题1
    QStringList subTitle2;      //子标题2

    QList<QString> columnNames; //字段名称
    QList<int> columnWidths;    //字段宽度

    QStringList content;        //内容
    QString separator;          //行内容分隔符
    QString subSeparator;       //子内容分隔符

    int borderWidth;            //边框宽度
    bool randomColor;           //随机颜色
    QList<int> colorColumn;     //随机颜色列索引集合

    int checkColumn;            //校验列
    QString checkType;          //校验类型
    QString checkValue;         //校验值
    QString checkColor;         //检验颜色

    int maxCount;               //最大行数
    int warnCount;              //警告行数

    //下面的参数是打印才有
    bool stretchLast;           //最后列拉伸填充
    bool landscape;             //横向排版
    QMargins pageMargin;        //纸张边距

    DataContent() {
        separator = ";";
        subSeparator = "|";

        borderWidth = 1;
        randomColor = false;

        checkColumn = -1;
        checkType = "==";
        checkValue = "0";
        checkColor = "red";

        maxCount = 100000;
        warnCount = 10000;

        stretchLast = true;
        landscape = false;
        pageMargin = QMargins(10, 12, 10, 12);
    }
};

#endif // DATAHEAD_H
