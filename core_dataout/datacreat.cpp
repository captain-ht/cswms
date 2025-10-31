#include "datacreat.h"

void DataCreat::creatUavsReportHead(QStringList &list, const UavsReportData &reportData)
{
    //表格开始
    list << "<table border='0.0' cellspacing='0' cellpadding='6'>";

    //标题
    list << "<tr>";
    list << QString("<td width='100%' align='center' style='font-size:22px;font-weight:bold;' colspan='%1'>%2</td>").arg(2).arg(reportData.title);
    list << "</tr>";

    //横线或者换行
    list << "<hr>";
    //list << "<br>";

    //当前模块通用样式
    QString style = "style='font-size:18px;'";

    //设备名称
    QString name = QString("设备名称: %1").arg(reportData.name);
    list << "<tr>";
    list << QString("<td colspan='%1' %2>%3</td>").arg(2).arg(style).arg(name);
    list << "</tr>";

    //经度纬度
    QString lng = QString("设备经度: %1").arg(reportData.lng);
    QString lat = QString("设备纬度: %1").arg(reportData.lat);
    list << "<tr>";
    list << QString("<td %1>%2</td>").arg(style).arg(lng);
    list << QString("<td %1>%2</td>").arg(style).arg(lat);
    list << "</tr>";

    //告警日期+报告时间
    QString timeAlarm = QString("告警日期: %1").arg(reportData.timeAlarm);
    QString timeReport = QString("报告时间: %1").arg(reportData.timeReport);
    list << "<tr>";
    list << QString("<td %1>%2</td>").arg(style).arg(timeAlarm);
    list << QString("<td %1>%2</td>").arg(style).arg(timeReport);
    list << "</tr>";

    //表格结束
    list << "</table>";
}

void DataCreat::creatUavsReportBody(QStringList &list, const UavsReportData &reportData, int imageWidth)
{
    //换行
    list << "<br><br>";

    //表格开始
    list << "<table border='0.5' cellspacing='0' cellpadding='5'>";

    //标题
    list << "<tr>";
    list << QString("<td width='100%' align='center' style='font-size:20px;font-weight:bold;' colspan='%1'>%2</td>").arg(2).arg(reportData.subTitle);
    list << "</tr>";

    //当前模块通用样式
    QString style = "style='font-size:18px;'";

    //告警类型
    QString type = QString("告警类型: %1").arg(reportData.type);
    list << "<tr>";
    list << QString("<td width='100%' colspan='%1' %2>%3</td>").arg(2).arg(style).arg(type);
    list << "</tr>";

    //目标数量+威胁等级
    QString textCount = QString("目标数量: %1").arg(reportData.textCount);
    QString textLevel = QString("威胁等级: %1").arg(reportData.textLevel);
    list << "<tr>";
    list << QString("<td %1>%2</td>").arg(style).arg(textCount);
    list << QString("<td %1>%2</td>").arg(style).arg(textLevel);
    list << "</tr>";

    //图像队列
    QStringList listImage = reportData.images;
    int count = listImage.count();

    //告警图像
    QString text = QString("告警图像 ( 共 %1 张 )").arg(count);
    list << "<tr>";
    list << QString("<td width='100%' align='center' style='font-size:20px;font-weight:bold;' colspan='%1'>%2</td>").arg(2).arg(text);
    list << "</tr>";

    //计算绘制区域大小,先将图像缩放到合适大小并保存在本地
    QString path = qApp->applicationDirPath() + "/snap/";
    for (int i = 0; i < count; ++i) {
        QImage image(listImage.at(i));
        if (image.width() > imageWidth) {
            QString name = path + QString("%1.jpg").arg(i + 1);
            image = image.scaled(imageWidth, image.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            image.save(name, "jpg");
            listImage[i] = name;
        }
    }

    //qDebug() << listImage;
    //垂直居中
    style = "style='vertical-align:middle;'";
    for (int i = 0; i < count; i = i + 2) {
        list << "<tr>";
        list << QString("<td align='center' %1><img src='%2'></td>").arg(style).arg(listImage.at(i));
        //需要过滤判断下,很可能是奇数会超过下一个
        if (i < count - 1) {
            list << QString("<td align='center' %1><img src='%2'></td>").arg(style).arg(listImage.at(i + 1));
        }
        list << "</tr>";
    }

    //表格结束
    list << "</table>";
}
