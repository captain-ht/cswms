#include "tableitemdelegate.h"
#include "quihelper.h"

TableItemDelegate::TableItemDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{

}

void TableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //过滤指定的列,不覆盖原有设置的文字颜色
    if (index.column() == 7 || index.column() == 9) {
        QString text = index.data(Qt::DisplayRole).toString();
        int align = index.data(Qt::TextAlignmentRole).toInt();
        QFont font = index.data(Qt::FontRole).value<QFont>();
        QColor color = index.data(Qt::ForegroundRole).value<QColor>();

        QBrush brush = Qt::NoBrush;
        if (option.state & QStyle::State_Selected) {
            brush = QBrush(QColor(QUIConfig::NormalColorStart));
        } else if (option.state & QStyle::State_MouseOver) {
            brush = QBrush(QColor(QUIConfig::NormalColorStart));
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(brush);
        painter->drawRect(option.rect);

        painter->setFont(font);
        painter->setPen(color);
        painter->drawText(option.rect, align, text);
        return;
    }

    QStyledItemDelegate::paint(painter, option, index);
}
