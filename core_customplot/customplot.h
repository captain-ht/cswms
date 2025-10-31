#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

/**
 * QCustomPlot控件封装 2019-5-1
 * 1. 可设置X轴Y轴范围值
 * 2. 可设置背景颜色+文本颜色+网格颜色
 * 3. 可设置三条曲线颜色+颜色集合
 * 4. 可设置是否显示定位十字线,可分别设置横向和纵向
 * 5. 可设置十字线的宽度和颜色
 * 6. 可设置是否显示数据点以及数据点的大小
 * 7. 可设置是否填充背景形成面积图
 * 8. 可设置模式-拖动+缩放等
 * 9. 可设置坐标轴间距+第二坐标系可见
 * 10. 提供接口setDataLine直接设置曲线,支持多条
 * 11. 提供接口setDataBar直接设置柱状图,支持多条形成堆积图
 * 12. 提供接口setLabs设置文本变量替代key,包括X轴和Y轴
 * 13. 提供清空数据+重绘图表+外部获取QCustomPlot对象,这样就可以进行更加详细的参数设置
 * 14. 提供函数start+stop来模拟正弦曲线
 * 15. 可设置柱状图的值的位置+精确度+颜色
 * 16. 支持鼠标移动到数据点高亮显示数据点以及显示数据提示信息
 * 17. 可设置提示信息位置 自动处理+顶部+右上角+右侧+右下角+底部+左下角+左侧+左上角
 * 18. 可设置是否校验数据产生不同的背景颜色,比如柱状图的每根柱子都可以根据数据生成不同背景颜色
 * 19. 可设置是否显示图例+图例位置+图例行数以及图例单行显示
 * 20. 支持多条曲线+柱状图+柱状分组图+横向柱状图+横向分组图+柱状堆积图
 * 21. 内置15套精美颜色,自动取颜色集合的颜色,省去配色的烦恼
 * 22. 每条柱状图都可以设置不同的颜色,分组柱状图可以设置颜色交替
 * 23. Y轴数值支持百分比显示,可拓展成其他格式
 * 24. 内置平滑曲线算法,支持平滑曲线绘制,传入点集合即可
 * 25. 同时支持 QCustomPlot 1.0 和 QCustomPlot 2.0
 * 26. 支持Qt4-Qt5任意Qt版本,支持任意编译器+任意操作系统
 */

#include "customplothelper.h"
#include "customplottracer.h"
#include "customplotbarv.h"
#include "customplotbarh.h"
#include "customplotline.h"

class CustomPlot : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString colorNames READ getColorNames WRITE setColorNames)

    Q_PROPERTY(QColor bgColor READ getBgColor WRITE setBgColor)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)

    Q_PROPERTY(int gridWidth READ getGridWidth WRITE setGridWidth)
    Q_PROPERTY(QColor gridColor READ getGridColor WRITE setGridColor)

    Q_PROPERTY(int lineWidth READ getLineWidth WRITE setLineWidth)
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor)

    Q_PROPERTY(bool showLineh READ getShowLineh WRITE setShowLineh)
    Q_PROPERTY(bool showLinev READ getShowLinev WRITE setShowLinev)

    Q_PROPERTY(int fontSize READ getFontSize WRITE setFontSize)
    Q_PROPERTY(int pointSize READ getPointSize WRITE setPointSize)

    Q_PROPERTY(double offsetX READ getOffsetX WRITE setOffsetX)
    Q_PROPERTY(double offsetY READ getOffsetY WRITE setOffsetY)

    Q_PROPERTY(int toolTipPosition READ getToolTipPosition WRITE setToolTipPosition)
    Q_PROPERTY(int interactions READ getInteractions WRITE setInteractions)
    Q_PROPERTY(int padding READ getPadding WRITE setPadding)

    Q_PROPERTY(bool axis2Visible READ getAxis2Visible WRITE setAxis2Visible)
    Q_PROPERTY(bool percentY READ getPercentY)

public:
    explicit CustomPlot(QWidget *parent = 0);

protected:
    void showEvent(QShowEvent *);
    void resizeEvent(QResizeEvent *);

private:
    QString colorNames;             //颜色名称集合
    lcolor colors;                  //颜色集合

    QColor bgColor;                 //背景颜色
    QColor textColor;               //文字颜色

    int gridWidth;                  //网格宽度
    QColor gridColor;               //网格颜色

    int lineWidth;                  //十字线宽度
    QColor lineColor;               //十字线颜色

    bool showLineh;                 //显示横线
    bool showLinev;                 //显示竖线

    int fontSize;                   //字体大小
    int pointSize;                  //数据点大小

    double offsetX;                 //X轴范围偏移
    double offsetY;                 //Y轴范围偏移

    int toolTipPosition;            //提示信息位置
    int interactions;               //交互模式-鼠标拖动滚轮缩放等
    int padding;                    //坐标轴边距

    bool axis2Visible;              //第二坐标轴可见
    bool percentY;                  //Y轴百分比显示

    bool isLoad;                    //是否加载过
    QList<QCPBars *> bars;          //存储柱状图集合
    QCustomPlot *customPlot;        //曲线图表对象
    QCPScatterStyle scatterStyle;   //曲线数据样式
    QTimer *timer;                  //定时器模拟数据

    CustomPlotTracer *layerable;    //游标层
    QCPItemTracer *tracer;          //数据点指示器
    QCPItemText *textTip;           //提示信息
    QCPBars *staticBarv;            //静态柱状图
    CustomPlotBarh *staticBarh;     //静态横向柱状图

private slots:
    //响应鼠标移动
    void mouseMove(QMouseEvent *event);
    //计算提示信息位置
    void checkToolTip(double &x, double &y);

public:
    lcolor getColors()              const;
    QString getColorNames()         const;

    QColor getBgColor()             const;
    QColor getTextColor()           const;

    int getGridWidth()              const;
    QColor getGridColor()           const;

    int getLineWidth()              const;
    QColor getLineColor()           const;

    bool getShowLineh()             const;
    bool getShowLinev()             const;

    int getFontSize()               const;
    int getPointSize()              const;

    double getOffsetX()             const;
    double getOffsetY()             const;

    int getToolTipPosition()        const;
    int getInteractions()           const;
    int getPadding()                const;

    bool getAxis2Visible()          const;
    bool getPercentY()              const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;
    QCustomPlot *getPlot()          const;

public Q_SLOTS:
    //设置颜色集合
    void setColorNames(const QString &colorNames);

    //设置背景颜色+文字颜色
    void setBgColor(const QColor &bgColor);
    void setTextColor(const QColor &textColor);

    //设置网格宽度+网格颜色
    void setGridWidth(int gridWidth);
    void setGridColor(const QColor &gridColor);

    //设置十字线宽度+颜色
    void setLineWidth(int lineWidth);
    void setLineColor(const QColor &lineColor);

    //设置是否显示横线竖线
    void setShowLineh(bool showLineh);
    void setShowLinev(bool showLinev);

    //设置字体大小+数据点宽度
    void setFontSize(int fontSize);
    void setPointSize(int pointSize);

    //设置X轴偏移值+Y轴偏移值
    void setOffsetX(double offsetX);
    void setOffsetY(double offsetY);

    //设置提示信息位置 0 1 2 3 4 5 6 7 8 依次表示为 自动处理+顶部+右上角+右侧+右下角+底部+左下角+左侧+左上角
    void setToolTipPosition(int toolTipPosition);
    //设置交互模式-鼠标拖动滚轮缩放等
    void setInteractions(int interactions);
    //设置坐标轴边距
    void setPadding(int padding);

    //设置第二坐标轴可见
    void setAxis2Visible(bool axis2Visible);
    //设置Y轴可见
    void setPercentY(bool percentY);

    //设置图例可见+位置+行数
    void setLegend(bool visible, int position = 0, int column = 1);
    //设置画布隐藏
    void setVisible(int index, bool visible);

    //初始化曲线
    void init();
    //初始化图层用于绘制十字线游标
    void initTracer();
    //初始化指示器用于高亮数据点+提示数据点信息
    void initItem();

    //清空数据+清空曲线+重绘
    void clearData(int index);
    void clear(bool clearGraphs = true, bool clearItems = true, bool clearPlottables = true);
    void replot();

    //绘制十字线游标
    void drawFlowLine();
    //绘制静态线条
    void drawStaticLinev(double x, double y, const QColor &color = Qt::red, int width = 2);
    void drawStaticLineh(double x, double y, const QColor &color = Qt::red, int width = 1);

    //启动+停止模拟数据
    void start(const QColor &lineColor, int lineWidth);
    void stop();
    //模拟产生正弦曲线
    void drawSin();

    //添加画布
    void addGraph(int count);

    //设置X轴范围+Y轴范围
    void setRangeX(double min, double max, int tickCount = 6);
    void setRangeY(double min, double max, int tickCount = 6, bool percentY = false);

    //设置变量文本
    void setLabX(const vdouble &key, const vstring &lab);
    void setLabY(const vdouble &key, const vstring &lab);

    //添加平滑曲线
    void addSmoothLine(const LineData &data);
    //添加折线曲线
    void addDataLine(const LineData &data);
    //设置曲线数据
    void setDataLine(const LineData &data);
    //初始化画布
    void initGraph(QCPGraph *graph, const LineData &data);

    //设置柱状图,带显示值
    void setDataBarv(const BarData &data);
    //设置柱状分组图,带显示值
    void setDataBarvs(const BarData &data);
    //设置横向柱状图,带显示值
    void setDataBarh(const BarData &data);
    //设置横向柱状分组图,带显示值
    void setDataBarhs(const BarData &data);
    //设置柱状堆积图,带显示值
    void setDataBars(const BarData &data);
};

#endif // CUSTOMPLOT_H
