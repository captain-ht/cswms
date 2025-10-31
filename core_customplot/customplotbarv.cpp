#include "customplotbarv.h"

CustomPlotBarv::CustomPlotBarv(QCPAxis *keyAxis, QCPAxis *valueAxis) : QCPBars(keyAxis, valueAxis)
{
    valuePosition = 1;
    valuePrecision = 0;
    valueColor = Qt::white;
    checkData = false;
}

void CustomPlotBarv::draw(QCPPainter *painter)
{
    //必须先继续绘制父类,不然父类的所有东西没有绘制
    //顺序不能反,先绘制完父类再绘制自定义的数据,不然会覆盖
    QCPBars::draw(painter);

    //迭代拿到每个区域的坐标和宽高
    int index = -1;
#ifdef qcustomplot_v1
    QCPBarDataMap::const_iterator visibleBegin, visibleEnd;
    getVisibleDataBounds(visibleBegin, visibleEnd);
    for (QCPBarDataMap::const_iterator it = visibleBegin; it != visibleEnd; ++it) {
#else
    QCPBarsDataContainer::const_iterator visibleBegin, visibleEnd;
    getVisibleDataBounds(visibleBegin, visibleEnd);
    for (QCPBarsDataContainer::const_iterator it = visibleBegin; it != visibleEnd; ++it) {
#endif
        //获取柱状图区域
#ifdef qcustomplot_v1
        QPolygonF barPolygon = getBarPolygon(it.key(), it.value().value);
        QRectF rect;
        int x1 = barPolygon.at(1).x();
        int y1 = barPolygon.at(1).y();
        int x2 = barPolygon.at(3).x();
        int y2 = barPolygon.at(3).y();
        rect.setX(x1);
        rect.setY(y1);
        rect.setWidth(x2 - x1);
        rect.setHeight(y2 - y1);
#else
        QRectF rect = getBarRect(it->key, it->value);
#endif

        //先处理校验数据
        index++;
        if (checkData) {
            if (it->value >= 80) {
                setBrush(QColor(0, 176, 180));
            } else if (it->value >= 60) {
                setBrush(QColor(255, 192, 0));
            } else {
                setBrush(QColor(214, 77, 84));
            }
        } else {
            //如果存在颜色集合则取颜色集合
            if (index < barColors.count()) {
                setBrush(barColors.at(index));
            }
        }

        //设置画笔和画刷,绘制矩形区域形成柱状图
        if (this->pen() != Qt::NoPen) {
            painter->setPen(this->pen().color());
        }

        painter->setBrush(this->brush());
        painter->drawRect(rect);

        //设置文本的颜色,还可以设置字体
        painter->setPen(valueColor);

        //这里可以设置小数点精确度
        QString strValue = QString::number(it->value, 'f', valuePrecision);
        //计算字体的高度
        QFontMetrics fm = painter->fontMetrics();
#if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
        int textWidth = fm.horizontalAdvance(strValue);
#else
        int textWidth = fm.width(strValue);
#endif
        int textHeight = fm.ascent() + fm.descent();

        //如果矩形宽度小于文字宽度则不绘制
        if (rect.width() < textWidth) {
            continue;
        }

        //如果不在顶部,矩形高度小于文字高度则不绘制
        if (valuePosition > 1) {
            if (rect.height() < textHeight) {
                continue;
            }
        }

        //0-不绘制 1-顶部上面 2-顶部居中 3-中间居中 4-底部居中
        //设置区域一点点偏差,看起来不那么拥挤
        int offset = 3;
        if (valuePosition == 1) {
            rect.setY(rect.y() - textHeight - offset);
            painter->drawText(rect, Qt::AlignTop | Qt::AlignHCenter, strValue);
        } else if (valuePosition == 2) {
            rect.setY(rect.y() + offset);
            painter->drawText(rect, Qt::AlignTop | Qt::AlignHCenter, strValue);
        } else if (valuePosition == 3) {
            painter->drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, strValue);
        } else if (valuePosition == 4) {
            rect.setHeight(rect.height() - offset);
            painter->drawText(rect, Qt::AlignBottom | Qt::AlignHCenter, strValue);
        }
    }
}

int CustomPlotBarv::getValuePosition() const
{
    return this->valuePosition;
}

int CustomPlotBarv::getValuePrecision() const
{
    return this->valuePrecision;
}

QColor CustomPlotBarv::getValueColor() const
{
    return this->valueColor;
}

bool CustomPlotBarv::getCheckData() const
{
    return this->checkData;
}

void CustomPlotBarv::setValuePostion(int valuePosition)
{
    if (this->valuePosition != valuePosition) {
        this->valuePosition = valuePosition;
    }
}

void CustomPlotBarv::setValuePrecision(int valuePrecision)
{
    if (this->valuePrecision != valuePrecision) {
        this->valuePrecision = valuePrecision;
    }
}

void CustomPlotBarv::setValueColor(const QColor &valueColor)
{
    if (this->valueColor != valueColor) {
        this->valueColor = valueColor;
    }
}

void CustomPlotBarv::setCheckData(bool checkData)
{
    if (this->checkData != checkData) {
        this->checkData = checkData;
    }
}

void CustomPlotBarv::setBarColors(const lcolor &barColors)
{
    this->barColors = barColors;
}
