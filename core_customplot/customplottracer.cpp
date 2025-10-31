#include "customplottracer.h"

CustomPlotTracer::CustomPlotTracer(QCustomPlot *parentPlot) : QCPLayerable(parentPlot)
{
    showLineh = false;
    showLinev = false;
    lineWidth = 3;
    lineColor = QColor(255, 0, 0);

    movePos = QPoint(100, 50);
    customPlot = parentPlot;

    connect(parentPlot, SIGNAL(mouseMove(QMouseEvent *)), this, SLOT(mouseMove(QMouseEvent *)));
}

void CustomPlotTracer::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
    Q_UNUSED(painter);
}

void CustomPlotTracer::draw(QCPPainter *painter)
{
    QPen pen;
    pen.setWidth(lineWidth);
    pen.setColor(lineColor);
    painter->setPen(pen);

    if (showLinev) {
        painter->drawLine(QPointF(x, dHeight), QPointF(x, dY));
        pen.setColor(Qt::white);
        painter->setPen(pen);
        QFontMetrics fm = painter->fontMetrics();
        QString year = QString::number(QDate::currentDate().year());
#if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
        int width = fm.horizontalAdvance(year);
#else
        int width = fm.width(year);
#endif
        painter->drawText(QPointF(x - width / 2, dHeight - 3), year);
    }

    if (showLineh) {
        pen.setColor(lineColor);
        painter->setPen(pen);
        painter->drawLine(QPointF(dX, y), QPointF(dWidth, y));
    }
}

bool CustomPlotTracer::getShowLineh() const
{
    return this->showLineh;
}

bool CustomPlotTracer::getShowLinev() const
{
    return this->showLinev;
}

int CustomPlotTracer::getLineWidth() const
{
    return this->lineWidth;
}

QColor CustomPlotTracer::getLineColor() const
{
    return this->lineColor;
}

void CustomPlotTracer::mouseMove(QMouseEvent *event)
{
    Q_UNUSED(event);
    movePos = event->pos();
    drawLine();
}

void CustomPlotTracer::setShowLineh(bool showLineh)
{
    if (this->showLineh != showLineh) {
        this->showLineh = showLineh;
        customPlot->replot();
    }
}

void CustomPlotTracer::setShowLinev(bool showLinev)
{
    if (this->showLinev != showLinev) {
        this->showLinev = showLinev;
        customPlot->replot();
    }
}

void CustomPlotTracer::setLineWidth(int lineWidth)
{
    if (this->lineWidth != lineWidth) {
        this->lineWidth = lineWidth;
        customPlot->replot();
    }
}

void CustomPlotTracer::setLineColor(const QColor &lineColor)
{
    if (this->lineColor != lineColor) {
        this->lineColor = lineColor;
        customPlot->replot();
    }
}

void CustomPlotTracer::drawLine()
{
    x = movePos.x();
    y = movePos.y();

    //取出当前XY轴的范围值
    QCPRange rangex = customPlot->xAxis->range();
    QCPRange rangey = customPlot->yAxis->range();
    //qDebug() << rangex.lower << rangex.upper << rangey.lower << rangey.upper;

    //取出左下角坐标轴原点(0,0)在整个画布的屏幕坐标位置
    dX = customPlot->xAxis->coordToPixel(rangex.lower);
    dY = customPlot->yAxis->coordToPixel(rangey.lower);
    dWidth = customPlot->xAxis->coordToPixel(rangex.upper);
    dHeight = customPlot->yAxis->coordToPixel(rangey.upper);

    //必须在曲线区域内
    if (x >= dX && x <= dWidth && y <= dY && y >= dHeight) {
        customPlot->replot();
    }
}
