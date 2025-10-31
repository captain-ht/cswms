#ifndef CUSTOMPLOTTRACER_H
#define CUSTOMPLOTTRACER_H

#include "customplothead.h"

//自定义图层绘制十字线,也叫游标,定位线
class CustomPlotTracer : public QCPLayerable
{
    Q_OBJECT
public:
    explicit CustomPlotTracer(QCustomPlot *parentPlot);

protected:
    void applyDefaultAntialiasingHint(QCPPainter *painter) const;
    void draw(QCPPainter *painter);

private:
    bool showLineh;             //显示横线
    bool showLinev;             //显示竖线
    int lineWidth;              //十字线宽度
    QColor lineColor;           //十字线颜色

    double x, y;                //当前鼠标坐标
    double dX, dY;              //曲线区域坐标
    double dWidth, dHeight;     //曲线区域宽高
    QPoint movePos;             //按下鼠标坐标
    QCustomPlot *customPlot;    //图表对象

public:
    bool getShowLineh()         const;
    bool getShowLinev()         const;
    int getLineWidth()          const;
    QColor getLineColor()       const;

private slots:
    void mouseMove(QMouseEvent *event);

public Q_SLOTS:
    //设置是否显示横线竖线+十字线宽度+颜色
    void setShowLineh(bool showLineh);
    void setShowLinev(bool showLinev);
    void setLineWidth(int lineWidth);
    void setLineColor(const QColor &lineColor);

    //绘制十字线
    void drawLine();
};

#endif // CUSTOMPLOTTRACER_H
