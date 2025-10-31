#ifndef CUSTOMPLOTHELPER_H
#define CUSTOMPLOTHELPER_H

#include "customplothead.h"
class CustomPlot;

class CustomPlotHelper
{
public:
    //获取内置颜色集合
    static lcolor colors;
    static lcolor getColorList();
    static QStringList getColorNames();
    //随机获取颜色集合中的颜色
    static QColor getRandColor();

    //获取随机数,指定最小值和最大值
    static double getRandValue(int min, int max);
    //16进制字符串转为short
    static qint16 strHexToShort(const QString &strHex);

    //从数据坐标中取出XY轴范围值 offset为偏移值
    static void getRange(const vpoint &points, double &minX, double &maxX, double &minY, double &maxY, double offset = 0);
    //从多个数组中取出最大值
    static double getMaxValue(const lvdouble &values);
    //从多个数组中取出和最大值
    static double getMaxValue2(const lvdouble &values);

    //设置画布背景颜色形成面积图
    static void setGraphBrush(QCPGraph *graph, const QColor &color, int type);
    static void setGraphBrush(QPainter *painter, const QColor &color, int type);
    static void setGraphBrush(QCPGraph *graph, QPainter *painter, const QColor &color, int type);
};

#endif // CUSTOMPLOTHELPER_H
