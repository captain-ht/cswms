#include "customplothelper.h"
#include "customplot.h"

lcolor CustomPlotHelper::colors = lcolor();
lcolor CustomPlotHelper::getColorList()
{
    //备用颜色集合 可以自行添加
    if (colors.count() == 0) {
        colors << QColor(0, 176, 180) << QColor(0, 113, 193) << QColor(255, 192, 0);
        colors << QColor(72, 103, 149) << QColor(185, 87, 86) << QColor(0, 177, 125);
        colors << QColor(214, 77, 84) << QColor(71, 164, 233) << QColor(34, 163, 169);
        colors << QColor(59, 123, 156) << QColor(162, 121, 197) << QColor(72, 202, 245);
        colors << QColor(0, 150, 121) << QColor(111, 9, 176) << QColor(250, 170, 20);
    }

    return colors;
}

QStringList CustomPlotHelper::getColorNames()
{
    QList<QColor> colors = getColorList();
    QStringList colorNames;
    foreach (QColor color, colors) {
        colorNames << color.name();
    }
    return colorNames;
}

QColor CustomPlotHelper::getRandColor()
{
    QList<QColor> colors = getColorList();
    int index = getRandValue(0, colors.count());
    return colors.at(index);
}

double CustomPlotHelper::getRandValue(int min, int max)
{
    int value;
#if (QT_VERSION <= QT_VERSION_CHECK(5,10,0))
    //(min, max)范围
    value = (rand() % (max - min)) + min;
    //[min, max]范围
    //value = (rand() % (max - min + 1)) + min + 1;
#else
    value = QRandomGenerator::global()->bounded(min, max);
#endif
    return value;
}

qint16 CustomPlotHelper::strHexToShort(const QString &strHex)
{
    bool ok;
    return strHex.toUShort(&ok, 16);
}

void CustomPlotHelper::getRange(const vpoint &points, double &minX, double &maxX, double &minY, double &maxY, double offset)
{
    int count = points.count();
    if (count == 0) {
        return;
    }

    minX = maxX = points.at(0).x();
    minY = maxY = points.at(0).y();
    //开始索引从1开始因为没必要和自己比较
    for (int i = 1; i < count; ++i) {
        double x = points.at(i).x();
        double y = points.at(i).y();
        if (x < minX) {
            minX = x;
        } else if (x > maxX) {
            maxX = x;
        }
        if (y < minY) {
            minY = y;
        } else if (y > maxY) {
            maxY = y;
        }
    }

    //根据偏移值倍数重新调整
    if (offset != 0) {
        double offsetX = (maxX - minX) * offset;
        double offsetY = (maxY - minY) * offset;
        minX -= offsetX;
        maxX += offsetX;
        minY -= offsetY;
        maxY += offsetY;
    }

    //qDebug() << TIMEMS << points << minX << maxX << minY << maxY;
}

double CustomPlotHelper::getMaxValue(const lvdouble &values)
{
    int count = values.count();
    if (count == 0) {
        return 0;
    }

    //找出最大值
    double max = values.at(0).at(0);
    for (int i = 0; i < count; i++) {
        vdouble value = values.at(i);
        foreach (double temp, value) {
            if (temp > max) {
                max = temp;
            }
        }
    }

    return max;
}

double CustomPlotHelper::getMaxValue2(const lvdouble &values)
{
    int count = values.count();
    if (count == 0) {
        return 0;
    }

    double max = values.at(0).at(0);
    int column = values.at(0).count();

    QList<int> maxs;
    for (int j = 0; j < column; j++) {
        maxs << 0;
    }

    //逐个计算每一列相加的值
    for (int i = 0; i < count; i++) {
        vdouble value = values.at(i);
        for (int j = 0; j < column; j++) {
            maxs[j] += value.at(j);
        }
    }

    //取出最大值
    for (int j = 0; j < column; j++) {
        if (maxs.at(j) > max) {
            max = maxs.at(j);
        }
    }

    return max;
}

void CustomPlotHelper::setGraphBrush(QCPGraph *graph, const QColor &color, int type)
{
    setGraphBrush(graph, 0, color, type);
}

void CustomPlotHelper::setGraphBrush(QPainter *painter, const QColor &color, int type)
{
    setGraphBrush(0, painter, color, type);
}

void CustomPlotHelper::setGraphBrush(QCPGraph *graph, QPainter *painter, const QColor &color, int type)
{
    //可以选择设置单色,也可以设置渐变颜色
    //可以自行增加渐变规则
    QColor bgColor = color;
    if (type == 1) {
        bgColor.setAlpha(150);
        if (graph != 0) {
            graph->setBrush(bgColor);
        }
        if (painter != 0) {
            painter->setBrush(bgColor);
        }
    } else if (type == 2) {
        QLinearGradient linearGradient(0, 0, 0, 500);
        bgColor.setAlpha(180);
        linearGradient.setColorAt(0, bgColor);
        bgColor.setAlpha(50);
        linearGradient.setColorAt(1, bgColor);
        if (graph != 0) {
            graph->setBrush(linearGradient);
        }
        if (painter != 0) {
            painter->setBrush(linearGradient);
        }
    }
}
