#include "frmsmall.h"
#include <QPixmap>

frmSmall::frmSmall(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    UpdateUI();
    this->setWindowTitle("小车监控详情");
}

void frmSmall::UpdateUI()
{
    QPixmap bgImage("image//SmallCart.png");

    if (!bgImage.isNull())////缩放
    {
        lbl_pic->setAlignment(Qt::AlignCenter);

        int TempWidth = this->width() - frame_Left->width() - frame_Right->width() - 80;

        lbl_pic->setPixmap(bgImage.scaled(QSize(TempWidth, 260),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void frmSmall::closeEvent(QCloseEvent *event)
{
    if (this->isModal())
    {
        //qDebug() <<"deleteLater";
        this->deleteLater();
        event->accept();
    }
}
