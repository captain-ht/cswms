#ifndef CUSTOMPLOTLINE_H
#define CUSTOMPLOTLINE_H

#include "customplothead.h"

//自定义形状-平滑曲线图
class CustomPlotLine : public QCPItemRect
{
    Q_OBJECT
public:
    explicit CustomPlotLine(QCustomPlot *parentPlot);

protected:
    void draw(QCPPainter *painter);

private:
    //数据结构体
    LineData data;

public Q_SLOTS:
    //设置数据结构体
    void setData(const LineData &data);
};

#endif // CUSTOMPLOTLINE_H
