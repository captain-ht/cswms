#ifndef CUSTOMPLOTBARH_H
#define CUSTOMPLOTBARH_H

#include "customplothead.h"

//自定义形状-横向柱状图
class CustomPlotBarh : public QCPItemRect
{
    Q_OBJECT
public:
    explicit CustomPlotBarh(QCustomPlot *parentPlot);

protected:
    void draw(QCPPainter *painter);

private:
    double value;               //当前值
    int valuePosition;          //值的位置 0表示不显示
    int valuePrecision;         //值的精确度
    QColor valueColor;          //值的颜色
    bool checkData;             //校验数据改变背景颜色

public:
    double getValue()           const;
    int getValuePosition()      const;
    int getValuePrecision()     const;
    QColor getValueColor()      const;
    bool getCheckData()         const;

public Q_SLOTS:
    //设置值
    void setValue(double value);
    //设置值的位置
    void setValuePostion(int valuePosition);
    //设置值的精确度
    void setValuePrecision(int valuePrecision);
    //设置值的颜色
    void setValueColor(const QColor &valueColor);
    //设置是否校验数据
    void setCheckData(bool checkData);

    //设置区域
    void setRect(const QPointF &p1, const QPointF &p2);
};

#endif // CUSTOMPLOTBARH_H
