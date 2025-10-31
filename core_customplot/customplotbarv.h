#ifndef CUSTOMPLOTBARV_H
#define CUSTOMPLOTBARV_H

#include "customplothead.h"

//自定义柱状图,可设置绘制值的位置+精确度+文字颜色+颜色集合
class CustomPlotBarv : public QCPBars
{
    Q_OBJECT
public:
    explicit CustomPlotBarv(QCPAxis *keyAxis, QCPAxis *valueAxis);

protected:
    void draw(QCPPainter *painter);

private:
    int valuePosition;          //值的位置 0表示不显示
    int valuePrecision;         //值的精确度
    QColor valueColor;          //值的颜色
    bool checkData;             //校验数据改变背景颜色
    lcolor barColors;    //柱状图背景颜色集合

public:
    int getValuePosition()      const;
    int getValuePrecision()     const;
    QColor getValueColor()      const;
    bool getCheckData()         const;

public Q_SLOTS:
    //设置值的位置
    void setValuePostion(int valuePosition);
    //设置值的精确度
    void setValuePrecision(int valuePrecision);
    //设置值的颜色
    void setValueColor(const QColor &valueColor);
    //设置是否校验数据
    void setCheckData(bool checkData);
    //设置柱状图背景颜色集合
    void setBarColors(const lcolor &barColors);
};

#endif // CUSTOMPLOTBARV_H
