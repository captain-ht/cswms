#include "frmbigcart.h"
#include <QPixmap>
#include <QDebug>

frmBigCart::frmBigCart(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    this->setWindowTitle("大车监控详情");
    UpdateUI();
}

void frmBigCart::UpdateUI()
{
    QPixmap bgImage("image//BigCart.png");

    if (!bgImage.isNull())////缩放
    {
        lbl_pic->setAlignment(Qt::AlignCenter);

        int TempWidth = this->width() - frame_Left->width() - frame_Right->width() - 80;

        lbl_pic->setPixmap(bgImage.scaled(QSize(TempWidth, 260),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void frmBigCart::closeEvent(QCloseEvent *event)
{
    if (this->isModal())
    {
        //qDebug() <<"deleteLater";
        this->deleteLater();
        event->accept();
    }
}
