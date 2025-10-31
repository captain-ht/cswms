#ifndef CUSTOMPLOTHEAD_H
#define CUSTOMPLOTHEAD_H

#ifdef qcustomplot_v1
#include "qcustomplot1.h"
#elif qcustomplot_v2
#include "qcustomplot2.h"
#else
#include "qcustomplot3.h"
#endif

#include "qlayout.h"

#pragma execution_character_set("utf-8")

#ifndef TIMEMS
#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#endif

#include <QVector>
#include <QColor>
#include <QList>
#include <QStringList>

//重新定义一些数据类型
typedef QList<QColor> lcolor;
typedef QVector<QPointF> vpoint;
typedef QVector<QString> vstring;
typedef QVector<double> vdouble;
typedef QList<QVector<double> > lvdouble;
typedef QList<QStringList> lstringl;

//线条数据结构体
struct LineData {
    int index;              //画布索引
    QString name;           //画布名称
    vdouble key;            //X轴数据
    vdouble value;          //Y轴数据
    vpoint points;          //数据点集合 就是XY轴数据合并的点

    int lineWidth;          //线条宽度
    QColor lineColor;       //线条颜色
    int dataWidth;          //数据点宽度
    QColor dataColor;       //数据点颜色

    int lineStyle;          //线条样式
    int scatterShape;       //数据形状
    int fillColor;          //填充画布形成面积图 0-不填充 1-单色填充 2-渐变填充
    int smoothType;         //平滑算法类型

    bool drawValue;         //绘制对应的值
    bool autoScale;         //坐标轴自动缩放

    LineData() {
        index = 0;
        name = "";

        lineWidth = 3;
        lineColor = "darkorange";
        dataWidth = 6;
        dataColor = "white";

        lineStyle = 1;
        scatterShape = 4;
        fillColor = 0;
        smoothType = 0;

        drawValue = false;
        autoScale = false;
    }
};

//柱状数据结构体
struct BarData {
    QStringList rowNames;   //行名称集合
    QStringList columnNames;//列名称集合
    lvdouble values;        //数据集合

    QColor borderColor;     //边框颜色
    int valuePosition;      //数据显示位置
    int valuePrecision;     //数据小数点
    QColor valueColor;      //数据颜色

    bool checkData;         //是否检验数据
    bool differentColor;    //不同的颜色

    BarData() {
        borderColor = Qt::transparent;
        valuePosition = 0;
        valuePrecision = 0;
        valueColor = Qt::white;

        checkData = false;
        differentColor = false;
    }
};

#endif // CUSTOMPLOTHEAD_H
