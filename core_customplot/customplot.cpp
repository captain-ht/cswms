#include "customplot.h"
#include "qtimer.h"

CustomPlot::CustomPlot(QWidget *parent) : QWidget(parent)
{
    //实例化曲线控件
    customPlot = new QCustomPlot;
    //绑定鼠标移动显示数据
    connect(customPlot, SIGNAL(mouseMove(QMouseEvent *)), this, SLOT(mouseMove(QMouseEvent *)));

    layerable = 0;
    tracer = 0;
    textTip = 0;
    staticBarv = 0;
    staticBarh = 0;

    //设置布局加入曲线控件
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(customPlot);
    this->setLayout(layout);

    //备用颜色集合
    setColorNames(CustomPlotHelper::getColorNames().join("|"));

    setBgColor(QColor(38, 41, 74));
    setTextColor(QColor(250, 250, 250));

    gridWidth = 1;
    setGridColor(QColor(230, 230, 230));

    lineWidth = 3;
    lineColor = QColor(255, 0, 0);

    showLineh = false;
    showLinev = false;

    fontSize = 12;
    pointSize = 8;

    offsetX = 1;
    offsetY = 20;

    toolTipPosition = 0;
    interactions = 0;
    padding = 5;

    axis2Visible = false;
    percentY = false;

    //定时器模拟数据
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(drawSin()));

    isLoad = false;
    this->init();
}

void CustomPlot::showEvent(QShowEvent *)
{
    if (!isLoad) {
        isLoad = true;
        if (this->showLineh || this->showLinev) {
            this->drawFlowLine();
        }
    }
}

void CustomPlot::resizeEvent(QResizeEvent *)
{
    if (layerable != 0) {
        layerable->drawLine();
    }
}

void CustomPlot::mouseMove(QMouseEvent *event)
{
    if (tracer == 0 || textTip == 0) {
        return;
    }

    //跟踪鼠标点击事件点击位置
    QCPGraph *graph = (QCPGraph *)customPlot->plottableAt(event->pos(), true);
    QRect rect(0, 0, 1, 1);

    QString labx, laby;
    int offset = 10;
    bool toolTip = false;

    if (graph != 0) {
        double posKey;
        double key = 0, value = 0;
        QPoint p;
        p.setX(event->pos().x());
        p.setY(event->pos().y());

#ifdef qcustomplot_v1
        foreach (QCPData data, graph->data()->values()) {
            key = data.key;
            value = data.value;
#else
        for (int i = 0; i < graph->dataCount(); i++) {
            key = graph->dataMainKey(i);
            value = graph->dataMainValue(i);
#endif
            //取出对应key处的label变量,如果变量为空则取key的字符串
            //需要根据tickVector来找到对应的tickVectorLabels,有时候拖动表到不可见的部位tickVectorLabels不是全部的tickVectorLabels
            vdouble values = customPlot->xAxis->tickVector();
            vstring labs = customPlot->xAxis->tickVectorLabels();
            int index = values.indexOf(key);
            if (index >= 0) {
                labx = labs.at(index);
            } else {
                labx = QString::number(key);
            }

            //如果启用了百分比则需要后面显示百分比
            if (this->getPercentY()) {
                laby = QString("%1%").arg(value);
            } else {
                laby = QString("%1").arg(value);
            }

            //计算当前是否在区域内,在的话就显示
            posKey = customPlot->xAxis->coordToPixel(key);
            if (qAbs(posKey - event->pos().x()) <= offset) {
                double posx = graph->keyAxis()->coordToPixel(key);
                double posy = graph->valueAxis()->coordToPixel(value);
                rect.setRect(posx - offset, posy - offset, offset * 2, offset * 2);
                if (!rect.contains(event->pos())) {
                    continue;
                } else {
                    break;
                }
            }

            if (posKey - event->pos().x() > offset) {
                break;
            }
        }

        if (!graph->realVisibility()) {
            toolTip = false;
            tracer->setVisible(false);
            customPlot->replot();
        } else if (rect.contains(event->pos())) {
            //先设置大小和颜色
            tracer->setSize(10);
            tracer->setPen(QPen(textColor, 3));
            tracer->setBrush(QBrush(lineColor, Qt::SolidPattern));

            toolTip = true;
            tracer->setVisible(true);
            tracer->setGraph(graph);
            tracer->setGraphKey(key);
            customPlot->replot();
        } else if (tracer->visible()) {
            toolTip = false;
            tracer->setVisible(false);
            customPlot->replot();
        }
    } else if (tracer->visible()) {
        toolTip = false;
        tracer->setVisible(false);
        customPlot->replot();
    }

    //处理提示信息
    if (toolTip) {
        //根据设定的提示信息位置调整
        double x = event->pos().x();
        double y = event->pos().y();
        checkToolTip(x, y);

        //先设置颜色
        textTip->setColor(textColor);
        textTip->setBrush(bgColor);

        QString text = "横坐标: " + labx + "\n当前值: " + laby;
        textTip->position->setCoords(x, y);
        textTip->setText(text);
        textTip->setVisible(true);
        customPlot->replot();
    } else {
        if (textTip->visible()) {
            textTip->setVisible(false);
            customPlot->replot();
        }
    }
}

void CustomPlot::checkToolTip(double &x, double &y)
{
    //下面的计算位置偏移值未必精准可以自行调整
    //后期可能要根据提示信息面板的宽高进行精准计算
    if (toolTipPosition == 0) {
        //判断曲线控件的位置已经当前鼠标位置做出调整
        bool right = (customPlot->width() - x < 90);
        bool bottom = (customPlot->height() - y < 50);
        if (right) {
            if (bottom) {
                x = x - 100;
                y = y - 50;
            } else {
                x = x - 100;
                y = y + 10;
            }
        } else {
            if (bottom) {
                x = x + 10;
                y = y - 50;
            } else {
                x = x + 10;
                y = y + 10;
            }
        }
    } else if (toolTipPosition == 1) {
        x = x - 30;
        y = y - 50;
    } else if (toolTipPosition == 2) {
        x = x + 10;
        y = y - 50;
    } else if (toolTipPosition == 3) {
        x = x + 10;
        y = y - 20;
    } else if (toolTipPosition == 4) {
        x = x + 10;
        y = y + 10;
    } else if (toolTipPosition == 5) {
        x = x - 30;
        y = y + 10;
    } else if (toolTipPosition == 6) {
        x = x - 75;
        y = y + 10;
    } else if (toolTipPosition == 7) {
        x = x - 80;
        y = y - 20;
    } else if (toolTipPosition == 8) {
        x = x - 80;
        y = y - 50;
    }
}

lcolor CustomPlot::getColors() const
{
    return this->colors;
}

QString CustomPlot::getColorNames() const
{
    return this->colorNames;
}

QColor CustomPlot::getBgColor() const
{
    return this->bgColor;
}

QColor CustomPlot::getTextColor() const
{
    return this->textColor;
}

int CustomPlot::getGridWidth() const
{
    return this->gridWidth;
}

QColor CustomPlot::getGridColor() const
{
    return this->gridColor;
}

int CustomPlot::getLineWidth() const
{
    return this->lineWidth;
}

QColor CustomPlot::getLineColor() const
{
    return this->lineColor;
}

bool CustomPlot::getShowLineh() const
{
    return this->showLineh;
}

bool CustomPlot::getShowLinev() const
{
    return this->showLinev;
}

int CustomPlot::getFontSize() const
{
    return this->fontSize;
}

int CustomPlot::getPointSize() const
{
    return this->pointSize;
}

double CustomPlot::getOffsetX() const
{
    return this->offsetX;
}

double CustomPlot::getOffsetY() const
{
    return this->offsetY;
}

int CustomPlot::getToolTipPosition() const
{
    return this->toolTipPosition;
}

int CustomPlot::getInteractions() const
{
    return this->interactions;
}

int CustomPlot::getPadding() const
{
    return this->padding;
}

bool CustomPlot::getAxis2Visible() const
{
    return this->axis2Visible;
}

bool CustomPlot::getPercentY() const
{
    return this->percentY;
}

QSize CustomPlot::sizeHint() const
{
    return QSize(500, 350);
}

QSize CustomPlot::minimumSizeHint() const
{
    return QSize(50, 35);
}

QCustomPlot *CustomPlot::getPlot() const
{
    return this->customPlot;
}

void CustomPlot::setColorNames(const QString &colorNames)
{
    //数量小于原来的则依次替换
    QStringList names = colorNames.split("|");
    QStringList names2 = this->colorNames.split("|");
    int count = names.count();
    if (count < colors.count()) {
        for (int i = 0; i < count; ++i) {
            names2[i] = names.at(i);
        }
        this->colorNames = names2.join("|");
    } else {
        this->colorNames = colorNames;
    }

    //重新替换颜色集合
    colors.clear();
    names = this->colorNames.split("|");
    foreach (QString name, names) {
        colors << name;
    }
}

void CustomPlot::setBgColor(const QColor &bgColor)
{
    if (this->bgColor != bgColor) {
        this->bgColor = bgColor;
        customPlot->setBackground(bgColor);
        customPlot->axisRect()->setBackground(bgColor);
        customPlot->replot();
    }
}

void CustomPlot::setTextColor(const QColor &textColor)
{
    if (this->textColor != textColor) {
        this->textColor = textColor;
        scatterStyle.setSize(pointSize);
        scatterStyle.setPen(QPen(textColor, 2));
        scatterStyle.setShape(QCPScatterStyle::ssCircle);
        customPlot->legend->setTextColor(textColor);
        customPlot->replot();
    }
}

void CustomPlot::setGridWidth(int gridWidth)
{
    if (this->gridWidth != gridWidth) {
        this->gridWidth = gridWidth;
    }
}

void CustomPlot::setGridColor(const QColor &gridColor)
{
    if (this->gridColor != gridColor) {
        this->gridColor = gridColor;
        QPen pen(gridColor, gridWidth);

        //第一坐标轴
        customPlot->xAxis->setLabelColor(gridColor);
        customPlot->yAxis->setLabelColor(gridColor);
        customPlot->xAxis->setTickLabelColor(gridColor);
        customPlot->yAxis->setTickLabelColor(gridColor);
        customPlot->xAxis->setBasePen(pen);
        customPlot->yAxis->setBasePen(pen);
        customPlot->xAxis->setTickPen(pen);
        customPlot->yAxis->setTickPen(pen);
        customPlot->xAxis->setSubTickPen(pen);
        customPlot->yAxis->setSubTickPen(pen);
        customPlot->xAxis->grid()->setPen(pen);
        customPlot->yAxis->grid()->setPen(pen);
        customPlot->xAxis->grid()->setZeroLinePen(pen);
        customPlot->yAxis->grid()->setZeroLinePen(pen);

        //第二坐标轴
        customPlot->xAxis2->setLabelColor(gridColor);
        customPlot->yAxis2->setLabelColor(gridColor);
        customPlot->xAxis2->setTickLabelColor(gridColor);
        customPlot->yAxis2->setTickLabelColor(gridColor);
        customPlot->xAxis2->setBasePen(pen);
        customPlot->yAxis2->setBasePen(pen);
        customPlot->xAxis2->setTickPen(pen);
        customPlot->yAxis2->setTickPen(pen);
        customPlot->xAxis2->setSubTickPen(pen);
        customPlot->yAxis2->setSubTickPen(pen);
        customPlot->xAxis2->grid()->setPen(pen);
        customPlot->yAxis2->grid()->setPen(pen);
        customPlot->xAxis2->grid()->setZeroLinePen(pen);
        customPlot->yAxis2->grid()->setZeroLinePen(pen);
        customPlot->replot();
    }
}

void CustomPlot::setLineWidth(int lineWidth)
{
    if (this->lineWidth != lineWidth && layerable != 0) {
        this->lineWidth = lineWidth;
        layerable->setLineWidth(lineWidth);
    }
}

void CustomPlot::setLineColor(const QColor &lineColor)
{
    if (this->lineColor != lineColor && layerable != 0) {
        this->lineColor = lineColor;
        layerable->setLineColor(lineColor);
    }
}

void CustomPlot::setShowLineh(bool showLineh)
{
    if (this->showLineh != showLineh && layerable != 0) {
        this->showLineh = showLineh;
        layerable->setShowLineh(showLineh);
    }
}

void CustomPlot::setShowLinev(bool showLinev)
{
    if (this->showLinev != showLinev && layerable != 0) {
        this->showLinev = showLinev;
        layerable->setShowLinev(showLinev);
    }
}

void CustomPlot::setFontSize(int fontSize)
{
    if (this->fontSize != fontSize) {
        this->fontSize = fontSize;
        QFont font;
        font.setPixelSize(fontSize);
        customPlot->legend->setFont(font);
        customPlot->xAxis->setLabelFont(font);
        customPlot->yAxis->setLabelFont(font);
        customPlot->xAxis->setTickLabelFont(font);
        customPlot->yAxis->setTickLabelFont(font);
        customPlot->xAxis2->setLabelFont(font);
        customPlot->yAxis2->setLabelFont(font);
        customPlot->xAxis2->setTickLabelFont(font);
        customPlot->yAxis2->setTickLabelFont(font);
        customPlot->replot();
    }
}

void CustomPlot::setPointSize(int pointSize)
{
    if (this->pointSize != pointSize) {
        this->pointSize = pointSize;
        customPlot->replot();
    }
}

void CustomPlot::setOffsetX(double offsetX)
{
    if (this->offsetX != offsetX) {
        this->offsetX = offsetX;
    }
}

void CustomPlot::setOffsetY(double offsetY)
{
    if (this->offsetY != offsetY) {
        this->offsetY = offsetY;
    }
}

void CustomPlot::setToolTipPosition(int toolTipPosition)
{
    if (this->toolTipPosition != toolTipPosition) {
        this->toolTipPosition = toolTipPosition;
        customPlot->replot();
    }
}

void CustomPlot::setInteractions(int interactions)
{
    if (this->interactions != interactions) {
        this->interactions = interactions;
        if (interactions == 0) {
            customPlot->setInteractions(QCP::iSelectOther);
        } else if (interactions == 1) {
            customPlot->setInteractions(QCP::iRangeDrag);
        } else if (interactions == 2) {
            customPlot->setInteractions(QCP::iRangeZoom);
        } else if (interactions == 3) {
            customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        } else {
            customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iMultiSelect | QCP::iSelectPlottables | QCP::iSelectAxes);
        }
    }
}

void CustomPlot::setPadding(int padding)
{
    if (this->padding != padding) {
        this->padding = padding;
        customPlot->xAxis->setPadding(padding);
        customPlot->yAxis->setPadding(padding);
        customPlot->xAxis2->setPadding(padding);
        customPlot->yAxis2->setPadding(padding);
        customPlot->xAxis2->setOffset(25);
    }
}

void CustomPlot::setAxis2Visible(bool axis2Visible)
{
    if (this->axis2Visible != axis2Visible) {
        this->axis2Visible = axis2Visible;
        //设置第二坐标轴是否显示,可以形成闭合的盒子
        customPlot->xAxis2->setVisible(axis2Visible);
        customPlot->yAxis2->setVisible(axis2Visible);
        customPlot->xAxis2->setTicks(false);
        customPlot->yAxis2->setTicks(false);
        customPlot->xAxis2->setTickLabels(false);
        customPlot->yAxis2->setTickLabels(false);
    }
}

void CustomPlot::setPercentY(bool percentY)
{
    if (this->percentY != percentY) {
        this->percentY = percentY;
    }
}

void CustomPlot::setLegend(bool visible, int position, int column)
{
    customPlot->legend->setVisible(visible);
#ifndef qcustomplot_v1
    customPlot->legend->setWrap(column);
    //设置摆放位置顺序列优先,这样的话就是一行,默认是一列垂直分布
    customPlot->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);
#endif

    QCPLayoutInset *layout = customPlot->axisRect()->insetLayout();
    if (position == 0) {
        layout->setInsetAlignment(0, Qt::AlignTop | Qt::AlignHCenter);
    } else if (position == 1) {
        layout->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
    } else if (position == 2) {
        layout->setInsetAlignment(0, Qt::AlignVCenter | Qt::AlignRight);
    } else if (position == 3) {
        layout->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);
    } else if (position == 4) {
        layout->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignHCenter);
    } else if (position == 5) {
        layout->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
    } else if (position == 6) {
        layout->setInsetAlignment(0, Qt::AlignVCenter | Qt::AlignLeft);
    } else if (position == 7) {
        layout->setInsetAlignment(0, Qt::AlignTop | Qt::AlignLeft);
    }
}

void CustomPlot::setVisible(int index, bool visible)
{
    //画布的数量一定小于等于画布索引
    if (customPlot->graphCount() <= index) {
        return;
    }

    customPlot->graph(index)->setVisible(visible);
    customPlot->legend->elementAt(index)->setVisible(visible);
}

void CustomPlot::init()
{
#ifdef qcustomplot_v1
#if 0
    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot->xAxis->setDateTimeFormat("mm:ss");

    customPlot->xAxis->setAutoTicks(false);
    customPlot->xAxis->setAutoTickLabels(false);
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->xAxis->setTickStep(1);

    customPlot->yAxis->setAutoTicks(false);
    customPlot->yAxis->setAutoTickLabels(false);
    customPlot->yAxis->setAutoTickStep(false);
    customPlot->yAxis->setTickStep(1);
#endif
#endif

    //设置图例
    customPlot->legend->setVisible(false);
    customPlot->legend->setBrush(QColor(255, 255, 255, 10));
    customPlot->legend->setBorderPen(Qt::NoPen);

    //设置网格线可见
    customPlot->xAxis->grid()->setVisible(true);
    customPlot->yAxis->grid()->setVisible(true);

    //设置子网格线可见
    //customPlot->xAxis->grid()->setSubGridVisible(true);
    //customPlot->yAxis->grid()->setSubGridVisible(true);

    //设置网格线伸出部分
    customPlot->xAxis->setTickLength(0, 0);
    //customPlot->yAxis->setTickLength(0, 0);

    //设置子网格伸出部分
    customPlot->xAxis->setSubTickLength(0, 0);
    //customPlot->yAxis->setSubTickLength(0, 0);

    customPlot->xAxis->setVisible(true);
    customPlot->setInteractions(QCP::iSelectOther);
    customPlot->replot();
}

void CustomPlot::initTracer()
{
    if (layerable != 0) {
        return;
    }

    //增加一个层用于绘制游标
    layerable = new CustomPlotTracer(customPlot);
    layerable->setVisible(true);
}

void CustomPlot::initItem()
{
    if (tracer != 0 || textTip != 0) {
        return;
    }

    //增加数据点指示器高亮显示数据点
    tracer = new QCPItemTracer(customPlot);
    tracer->setParent(customPlot);
    tracer->setStyle(QCPItemTracer::tsCircle);
    tracer->setVisible(false);

    //增加数据点提示信息
    textTip = new QCPItemText(customPlot);
    textTip->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    textTip->setPositionAlignment(Qt::AlignLeft);
    textTip->position->setType(QCPItemPosition::ptAbsolute);
    textTip->setPadding(QMargins(6, 6, 6, 6));
    textTip->setVisible(false);
}

void CustomPlot::clearData(int index)
{
    //画布的数量一定小于等于画布索引
    if (customPlot->graphCount() <= index) {
        return;
    }

#ifdef qcustomplot_v1
    customPlot->graph(index)->clearData();
#else
    customPlot->graph(index)->data().clear();
#endif
}

void CustomPlot::clear(bool clearGraphs, bool clearItems, bool clearPlottables)
{
    if (clearGraphs) {
        customPlot->clearGraphs();
    }
    if (clearItems) {
        customPlot->clearItems();
        tracer = 0;
        textTip = 0;
    }
    if (clearPlottables) {
        customPlot->clearPlottables();
    }
}

void CustomPlot::replot()
{
    customPlot->replot();
}

void CustomPlot::drawFlowLine()
{
    if (layerable != 0) {
        layerable->drawLine();
        customPlot->replot();
    }
}

void CustomPlot::drawStaticLinev(double x, double y, const QColor &color, int width)
{
    if (staticBarv == 0) {
        staticBarv = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    }

    //用柱状图形式绘制线条
    staticBarv->setWidth(width);
    staticBarv->setWidthType(QCPBars::wtAbsolute);
    staticBarv->setPen(color);
    staticBarv->setBrush(color);
    staticBarv->setData(vdouble() << x, vdouble() << y);
}

void CustomPlot::drawStaticLineh(double x, double y, const QColor &color, int width)
{
    if (staticBarh == 0) {
        staticBarh = new CustomPlotBarh(customPlot);
    }

    staticBarh->setPen(color);
    staticBarh->setBrush(color);
    staticBarh->setRect(QPointF(x, y), QPointF(customPlot->width(), y + width));
}

void CustomPlot::start(const QColor &lineColor, int lineWidth)
{
    if (customPlot->graphCount() <= 0) {
        return;
    }

#ifdef qcustomplot_v1
    customPlot->xAxis->setAutoTicks(true);
    customPlot->xAxis->setAutoTickLabels(true);
#else
    QSharedPointer<QCPAxisTickerTime> ticker(new QCPAxisTickerTime);
    ticker->setTimeFormat("%m:%s");
    ticker->setTickCount(8);
    customPlot->xAxis->setTicker(ticker);
#endif
    customPlot->graph(0)->setPen(QPen(lineColor, lineWidth));
    timer->start();
}

void CustomPlot::stop()
{
    if (customPlot->graphCount() <= 0) {
        return;
    }

#ifdef qcustomplot_v1
    customPlot->xAxis->setAutoTicks(false);
    customPlot->xAxis->setAutoTickLabels(false);
#else
    QSharedPointer<QCPAxisTicker> ticker(new QCPAxisTicker);
    ticker->setTickCount(1);
    customPlot->xAxis->setTicker(ticker);
#endif
    timer->stop();
}

void CustomPlot::drawSin()
{
    if (customPlot->graphCount() <= 0) {
        return;
    }

    int size = 8;
#ifdef qcustomplot_v1
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
    customPlot->graph(0)->addData(key, qSin(key) * 100);
    customPlot->graph(0)->removeDataBefore(key - size);
#else
    static QTime timeStart = QTime::currentTime();
    double key = timeStart.msecsTo(QTime::currentTime()) / 1000.0;
    customPlot->graph(0)->addData(key, qSin(key) * 100);
#endif
    customPlot->xAxis->setRange(key, size, Qt::AlignRight);
    //customPlot->graph(0)->rescaleValueAxis();
    customPlot->replot();
}

void CustomPlot::addGraph(int count)
{
    for (int i = 0; i < count; i++) {
        customPlot->addGraph();
    }
}

void CustomPlot::setRangeX(double min, double max, int tickCount)
{
    customPlot->xAxis->setRange(min, max);
#ifdef qcustomplot_v1
    customPlot->xAxis->setAutoTickCount(tickCount);
#else
    customPlot->xAxis->ticker()->setTickCount(tickCount);
#endif
}

void CustomPlot::setRangeY(double min, double max, int tickCount, bool percentY)
{
    customPlot->yAxis->setRange(min, max);
#ifdef qcustomplot_v1
    customPlot->yAxis->setAutoTickCount(tickCount);
#else
    customPlot->yAxis->ticker()->setTickCount(tickCount);
#endif

    //如果开启了百分比格式,则先自动计算总共显示多少个数字
    this->percentY = percentY;
    if (percentY) {
        vstring lab;
        vdouble key;
        int step = (max - min) / tickCount;
        for (int i = min; i <= max; i = i + step) {
            key << i;
            lab << QString("%1%").arg(i);
        }

        setLabY(key, lab);
    }
}

void CustomPlot::setLabX(const vdouble &key, const vstring &lab)
{
    //将对应的key用文本替代表示
#ifdef qcustomplot_v1
    customPlot->xAxis->setAutoTicks(false);
    customPlot->xAxis->setAutoTickLabels(false);
    customPlot->xAxis->setTickVector(key);
    customPlot->xAxis->setTickVectorLabels(lab);
#else
    //设置标尺为文本标尺
    QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);
    ticker->addTicks(key, lab);
    customPlot->xAxis->setTicker(ticker);
#endif
}

void CustomPlot::setLabY(const vdouble &key, const vstring &lab)
{
    //将对应的key用文本替代表示
#ifdef qcustomplot_v1
    customPlot->yAxis->setAutoTicks(false);
    customPlot->yAxis->setAutoTickLabels(false);
    customPlot->yAxis->setTickVector(key);
    customPlot->yAxis->setTickVectorLabels(lab);
#else
    //设置标尺为文本标尺
    QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);
    ticker->addTicks(key, lab);
    customPlot->yAxis->setTicker(ticker);
#endif
}

void CustomPlot::addSmoothLine(const LineData &data)
{
    CustomPlotLine *line = new CustomPlotLine(customPlot);
    line->setData(data);
}

void CustomPlot::addDataLine(const LineData &data)
{
    QCPGraph *graph = customPlot->addGraph();
    initGraph(graph, data);
}

void CustomPlot::setDataLine(const LineData &data)
{
    int index = data.index;
    if (customPlot->graphCount() > index) {
        QCPGraph *graph = customPlot->graph(index);
        initGraph(graph, data);

        //是否自适应坐标轴
        if (data.autoScale) {
            graph->rescaleAxes();
        } else {
            customPlot->xAxis->setRange(-offsetX, data.key.count() + offsetX, Qt::AlignLeft);
        }
    }
}

void CustomPlot::initGraph(QCPGraph *graph, const LineData &data)
{
    //设置画布名称
    graph->setName(data.name);
    //设置画布数据
    graph->setData(data.key, data.value);

    //设置线条画笔
    graph->setPen(QPen(data.lineColor, data.lineWidth));
    //设置连接风格
    graph->setLineStyle((QCPGraph::LineStyle)data.lineStyle);

    //设置数据风格
    QCPScatterStyle scatterStyle;
    scatterStyle.setPen(QPen(data.dataColor, data.dataWidth));
    scatterStyle.setBrush(data.lineColor);
    scatterStyle.setSize(data.lineWidth);
    scatterStyle.setShape((QCPScatterStyle::ScatterShape)data.scatterShape);
    graph->setScatterStyle(scatterStyle);

    //设置填充画布
    CustomPlotHelper::setGraphBrush(graph, data.lineColor, data.fillColor);
}

void CustomPlot::setDataBarv(const BarData &data)
{
    //只有1列的才能设置
    if (data.columnNames.count() != 1) {
        return;
    }

    //可以直接用堆积图,因为只有一列的柱状图不会形成堆积
    setDataBars(data);
}

void CustomPlot::setDataBarvs(const BarData &data)
{
    //过滤个数不一致数据,防止索引越界
    int rowCount = data.rowNames.count();
    int columnCount = data.columnNames.count();
    int valueCount = data.values.count();
    if (columnCount == 0 || valueCount == 0 || columnCount != valueCount) {
        return;
    }

    //设置网格线不显示,会更好看
    customPlot->xAxis->grid()->setVisible(false);
    //customPlot->yAxis->grid()->setVisible(false);

    //设置横坐标文字描述
    vdouble ticks;
    vstring labels;
    int count = rowCount * columnCount;
    for (int i = 0; i < rowCount; i++) {
        ticks << 1.5 + (i * columnCount);
        labels << data.rowNames.at(i);
    }

    setLabX(ticks, labels);
    customPlot->xAxis->setRange(0, count + 1);

    for (int i = 0; i < columnCount; i++) {
        //同样也要先过滤个数是否符合要求
        vdouble value = data.values.at(i);
        if (rowCount != value.count()) {
            continue;
        }

        //创建柱状图
        CustomPlotBarv *bar = new CustomPlotBarv(customPlot->xAxis, customPlot->yAxis);
        bar->setCheckData(data.checkData);

        //设置宽度比例
        bar->setWidth(0.9);

        //设置显示值的位置 0-不绘制 1-顶部上面 2-顶部居中 3-中间居中 4-底部居中
        bar->setValuePostion(data.valuePosition);
        bar->setValuePrecision(data.valuePrecision);
        bar->setValueColor(data.valueColor);

        //设置名称
        bar->setName(data.columnNames.at(i));

        //设置颜色,取颜色集合
        QColor color = QColor(51, 204, 255);

        //如果设置了不同的颜色则设置颜色集合
        if (data.differentColor) {
            bar->setBarColors(colors);
        } else {
            if (i < colors.count()) {
                color = colors.at(i);
            }
        }

        //边缘高亮,如果传入了边框颜色则取边框颜色
        if (data.borderColor == Qt::transparent) {
            bar->setPen(color.lighter(150));
        } else {
            bar->setPen(data.borderColor);
        }

        bar->setBrush(color);

        //这个算法很巧妙,想了很久
        vdouble ticks;
        double offset = i * 0.9;
        for (int j = 0; j < rowCount; j++) {
            ticks << 1.0 + (j * columnCount) + offset;
        }

        //设置数据
        bar->setData(ticks, value);
    }
}

void CustomPlot::setDataBarh(const BarData &data)
{
    //只有1列的才能设置
    if (data.columnNames.count() != 1) {
        return;
    }

    //过滤个数不一致数据,防止索引越界
    int rowCount = data.rowNames.count();
    int columnCount = data.columnNames.count();
    int valueCount = data.values.count();
    if (columnCount == 0 || valueCount == 0 || columnCount != valueCount) {
        return;
    }

    //设置网格线不显示,会更好看
    customPlot->xAxis->grid()->setVisible(false);
    customPlot->yAxis->grid()->setVisible(false);
    customPlot->yAxis->setTickLength(0, 0);

    //设置横坐标文字描述
    vdouble ticks;
    vstring labels;
    int count = rowCount * columnCount;
    double padding = 1;
    for (int i = 0; i < rowCount; i++) {
        ticks << padding + (i * columnCount);
        labels << data.rowNames.at(i);
    }

    setLabY(ticks, labels);
    customPlot->yAxis->setRange(0, count + 1);

    //先计算出每个柱子占用的高度
    double barHeight = 0.7;
    for (int i = 0; i < columnCount; i++) {
        //同样也要先过滤个数是否符合要求
        vdouble value = data.values.at(i);
        if (rowCount != value.count()) {
            continue;
        }

        //先绘制系列1的数据,再绘制系列2,依次类推
        for (int j = 0; j < rowCount; j++) {
            //创建横向柱状图
            double y = (0.67 + (j * columnCount));
            CustomPlotBarh *bar = new CustomPlotBarh(customPlot);
            bar->setCheckData(data.checkData);
            bar->setRect(QPointF(0, y), QPointF(value.at(j), y + barHeight));
            bar->setValue(value.at(j));

            //设置显示值的位置 0-不绘制 1-顶部上面 2-顶部居中 3-中间居中 4-底部居中
            bar->setValuePostion(data.valuePosition);
            bar->setValuePrecision(data.valuePrecision);
            bar->setValueColor(data.valueColor);

            //设置颜色,取颜色集合
            QColor color = QColor(51, 204, 255);

            //如果设置了不同的颜色则取不同的颜色
            int index = data.differentColor ? j : i;
            if (index < colors.count()) {
                color = colors.at(index);
            }

            //边缘高亮,如果传入了边框颜色则取边框颜色
            if (data.borderColor == Qt::transparent) {
                bar->setPen(color.lighter(150));
            } else {
                bar->setPen(data.borderColor);
            }

            bar->setBrush(color);
        }
    }
}

void CustomPlot::setDataBarhs(const BarData &data)
{
    //过滤个数不一致数据,防止索引越界
    int rowCount = data.rowNames.count();
    int columnCount = data.columnNames.count();
    int valueCount = data.values.count();
    if (columnCount == 0 || valueCount == 0 || columnCount != valueCount) {
        return;
    }

    //设置网格线不显示,会更好看
    customPlot->xAxis->grid()->setVisible(false);
    customPlot->yAxis->grid()->setVisible(false);
    customPlot->yAxis->setTickLength(0, 0);
    customPlot->xAxis->setVisible(false);

    //设置横坐标文字描述
    vdouble ticks;
    vstring labels;
    int count = rowCount * columnCount;
    //这个算法想了很久,很牛逼
    double padding = 1.5 + (columnCount - 2) * 0.4;
    for (int i = 0; i < rowCount; i++) {
        ticks << padding + (i * columnCount);
        labels << data.rowNames.at(i);
    }

    setLabY(ticks, labels);
    customPlot->yAxis->setRange(0, count + 1);

    //先计算出每个柱子占用的高度
    double barHeight = 0.8;
    for (int i = 0; i < columnCount; i++) {
        //同样也要先过滤个数是否符合要求
        vdouble value = data.values.at(i);
        if (rowCount != value.count()) {
            continue;
        }

        //先绘制系列1的数据,再绘制系列2,依次类推
        for (int j = 0; j < rowCount; j++) {
            //创建横向柱状图
            double y = (0.7 + i * barHeight + (j * columnCount));
            CustomPlotBarh *bar = new CustomPlotBarh(customPlot);
            bar->setCheckData(data.checkData);
            bar->setRect(QPointF(0, y), QPointF(value.at(j), y + barHeight));
            bar->setValue(value.at(j));

            //设置显示值的位置 0-不绘制 1-顶部上面 2-顶部居中 3-中间居中 4-底部居中
            bar->setValuePostion(data.valuePosition);
            bar->setValuePrecision(data.valuePrecision);
            bar->setValueColor(data.valueColor);

            //设置颜色,取颜色集合
            QColor color = QColor(51, 204, 255);

            //如果设置了不同的颜色则取不同的颜色
            int index = data.differentColor ? j : i;
            if (index < colors.count()) {
                color = colors.at(index);
            }

            //边缘高亮,如果传入了边框颜色则取边框颜色
            if (data.borderColor == Qt::transparent) {
                bar->setPen(color.lighter(150));
            } else {
                bar->setPen(data.borderColor);
            }

            bar->setBrush(color);
        }
    }
}

void CustomPlot::setDataBars(const BarData &data)
{
    //过滤个数不一致数据,防止索引越界
    int rowCount = data.rowNames.count();
    int columnCount = data.columnNames.count();
    int valueCount = data.values.count();
    if (columnCount == 0 || valueCount == 0 || columnCount != valueCount) {
        return;
    }

    //设置网格线不显示,会更好看
    customPlot->xAxis->grid()->setVisible(false);
    //customPlot->yAxis->grid()->setVisible(false);

    //先清空原有柱状图
    bars.clear();

    //设置横坐标文字描述
    vdouble ticks;
    vstring labels;
    for (int i = 0; i < rowCount; i++) {
        ticks << i + 1;
        labels << data.rowNames.at(i);
    }

    setLabX(ticks, labels);
    customPlot->xAxis->setRange(0, rowCount + 1);

    for (int i = 0; i < columnCount; i++) {
        //同样也要先过滤个数是否符合要求
        vdouble value = data.values.at(i);
        if (rowCount != value.count()) {
            continue;
        }

        //创建柱状堆积图
        CustomPlotBarv *bar = new CustomPlotBarv(customPlot->xAxis, customPlot->yAxis);
        bar->setCheckData(data.checkData);

        //设置宽度比例
        bar->setWidth(0.6);

        //设置显示值的位置 0-不绘制 1-顶部上面 2-顶部居中 3-中间居中 4-底部居中
        bar->setValuePostion(data.valuePosition);
        bar->setValuePrecision(data.valuePrecision);
        bar->setValueColor(data.valueColor);

#ifndef qcustomplot_v1
        //设置堆积间隙
        if (data.borderColor != Qt::transparent) {
            bar->setStackingGap(1);
        }
#endif
        //设置名称
        bar->setName(data.columnNames.at(i));

        //设置颜色,取颜色集合
        QColor color = QColor(51, 204, 255);

        //如果设置了不同的颜色则设置颜色集合
        if (data.differentColor) {
            bar->setBarColors(colors);
        } else {
            if (i < colors.count()) {
                color = colors.at(i);
            }
        }

        //边缘高亮,如果传入了边框颜色则取边框颜色
        if (columnCount > 1 && data.borderColor == Qt::transparent) {
            bar->setPen(Qt::NoPen);
        } else {
            bar->setPen(QPen(data.borderColor == Qt::transparent ? color.lighter(150) : data.borderColor));
        }

        bar->setBrush(color);

        //设置堆积层叠顺序,后面那个移到前一个上面
        bars << bar;
        if (i > 0) {
            bar->moveAbove(bars.at(i - 1));
        }

        //设置数据
        bar->setData(ticks, value);
    }
}
