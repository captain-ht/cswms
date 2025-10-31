#include "customplotbarh.h"

CustomPlotBarh::CustomPlotBarh(QCustomPlot *parentPlot) : QCPItemRect(parentPlot)
{
    value = 0;
    valuePosition = 1;
    valuePrecision = 0;
    valueColor = Qt::white;
    checkData = false;
}

void CustomPlotBarh::draw(QCPPainter *painter)
{
    //获取整个画布区域
#ifdef qcustomplot_v1
    QRectF rect = QRectF(topLeft->pixelPoint(), bottomRight->pixelPoint());
#else
    QRectF rect = QRectF(topLeft->pixelPosition(), bottomRight->pixelPosition());
#endif

    painter->setPen(this->pen().color());
    painter->setBrush(this->brush());
    painter->drawRect(rect);

    //设置文本的颜色,还可以设置字体
    painter->setPen(valueColor);

    //这里可以设置小数点精确度
    QString strValue = QString::number(value, 'f', valuePrecision);
    //计算值的宽度
    QFontMetrics fm = painter->fontMetrics();
#if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
    int textWidth = fm.horizontalAdvance(strValue);
#else
    int textWidth = fm.width(strValue);
#endif

    //0-不绘制 1-顶部上面 2-顶部居中 3-中间居中 4-底部居中
    //设置区域一点点偏差,看起来不那么拥挤
    int offset = 5;
    if (valuePosition == 1) {
        rect.setWidth(rect.width() + textWidth + offset);
        painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, strValue);
    } else if (valuePosition == 2) {
        rect.setWidth(rect.width() - offset);
        painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, strValue);
    } else if (valuePosition == 3) {
        painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, strValue);
    } else if (valuePosition == 4) {
        rect.setX(rect.x() + offset);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, strValue);
    }
}

double CustomPlotBarh::getValue() const
{
    return this->value;
}

int CustomPlotBarh::getValuePosition() const
{
    return this->valuePosition;
}

int CustomPlotBarh::getValuePrecision() const
{
    return this->valuePrecision;
}

QColor CustomPlotBarh::getValueColor() const
{
    return this->valueColor;
}

bool CustomPlotBarh::getCheckData() const
{
    return this->checkData;
}

void CustomPlotBarh::setValue(double value)
{
    if (this->value != value) {
        this->value = value;
    }
}

void CustomPlotBarh::setValuePostion(int valuePosition)
{
    if (this->valuePosition != valuePosition) {
        this->valuePosition = valuePosition;
    }
}

void CustomPlotBarh::setValuePrecision(int valuePrecision)
{
    if (this->valuePrecision != valuePrecision) {
        this->valuePrecision = valuePrecision;
    }
}

void CustomPlotBarh::setValueColor(const QColor &valueColor)
{
    if (this->valueColor != valueColor) {
        this->valueColor = valueColor;
    }
}

void CustomPlotBarh::setCheckData(bool checkData)
{
    if (this->checkData != checkData) {
        this->checkData = checkData;
    }
}

void CustomPlotBarh::setRect(const QPointF &p1, const QPointF &p2)
{
    this->topLeft->setCoords(p1.x(), p1.y());
    this->bottomRight->setCoords(p2.x(), p2.y());
}
