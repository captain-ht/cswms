#ifndef DATACREAT_H
#define DATACREAT_H

#include "datahead.h"

//报告内容结构体
struct UavsReportData {
    QString title;
    QString subTitle;

    QString name;
    QString lng;
    QString lat;
    QString timeAlarm;
    QString timeReport;

    QString type;
    QString textCount;
    QString textLevel;
    QStringList images;

    //下面的参数是打印才有
    bool stretchLast;           //最后列拉伸填充
    bool landscape;             //横向排版
    QMargins pageMargin;        //纸张边距

    UavsReportData() {
        title = QString::fromUtf8("无人机监控系统告警报告");
        subTitle = QString::fromUtf8("告警结果详情");

        name = QString::fromUtf8("无人机-001");
        lng = "121.414005";
        lat = "31.182805";
        timeAlarm = DATETIME;
        timeReport = DATETIME;

        type = QString::fromUtf8("可疑人员、可疑车辆、可疑物品");
        textCount = QString::fromUtf8("10个");
        textLevel = QString::fromUtf8("一级");

        stretchLast = false;
        landscape = false;
        pageMargin = QMargins(10, 12, 10, 12);
    }
};

class DataCreat
{
public:
    //创建无人机报表头部数据
    static void creatUavsReportHead(QStringList &list, const UavsReportData &reportData);
    //创建无人机报表主体数据
    static void creatUavsReportBody(QStringList &list, const UavsReportData &reportData, int imageWidth = 300);

    //可以自行增加其他模板数据
};

#endif // DATACREAT_H
