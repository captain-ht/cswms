#include "customplotline.h"
#include "customplothelper.h"
#include "smoothcurve.h"

CustomPlotLine::CustomPlotLine(QCustomPlot *parentPlot) : QCPItemRect(parentPlot)
{
}

void CustomPlotLine::draw(QCPPainter *painter)
{
    int count = data.points.count();
    if (count == 0) {
        return;
    }

    //要做坐标转换
    vpoint points;
    foreach (QPointF point, data.points) {
        double x = parentPlot()->xAxis->coordToPixel(point.x());
        double y = parentPlot()->yAxis->coordToPixel(point.y());
        points << QPointF(x, y);
    }

    //创建平滑曲线
    QPainterPath path;
    if (data.smoothType == 0) {
        path = SmoothCurve::createSmoothCurve(points);
    } else {
        path = SmoothCurve::createSmoothCurve2(points);
    }

    //设置画笔
    painter->setPen(QPen(data.lineColor, data.lineWidth));
    //绘制路径
    painter->drawPath(path);

    if (data.fillColor > 0) {
        vpoint points;
        //末一个点对应Y轴0
        points << QPointF(data.points.last().x(), 0);
        //第一个点对应Y轴0
        points << QPointF(data.points.first().x(), 0);
        //移动到第一个点
        points << data.points.first();

        //需要重新设置路径不然绘制的区域不规整
        foreach (QPointF point, points) {
            double x = parentPlot()->xAxis->coordToPixel(point.x());
            double y = parentPlot()->yAxis->coordToPixel(point.y());
            path.lineTo(x, y);
        }

        //去掉边框更符合需要
        painter->setPen(Qt::NoPen);
        //设置画布背景颜色
        CustomPlotHelper::setGraphBrush(painter, data.lineColor, data.fillColor);
        painter->drawPath(path);
    }

    //绘制数据点
    if (data.scatterShape > 0) {
        //曲线源码中绘制数据点的宽度貌似要做一个运算
        int width = data.dataWidth;
        painter->setPen(QPen(data.dataColor, width));
        painter->setBrush(data.lineColor);

        //循环绘制数据点
        foreach (QPointF point, points) {
            painter->drawEllipse(point, width + 1, width + 1);
        }

        //循环绘制数据值
        if (data.drawValue) {
            for (int i = 0; i < count; ++i) {
                QPointF point = points.at(i);
                //数值要在正上方
                int offset = 25;
                QRect textRect(point.x() - offset, point.y() - offset, offset * 2, offset);
                painter->drawRect(textRect);
                //可以指定显示的值
                QString text = QString::number(data.points.at(i).y(), 'f', 1);
                painter->drawText(textRect, Qt::AlignCenter, text);
            }
        }
    }
}

void CustomPlotLine::setData(const LineData &data)
{
    this->data = data;
}
