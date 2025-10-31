#include "frmlift.h"
#include <QPixmap>

frmLift::frmLift(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    this->setWindowTitle("起升监控详情");
    UpdateUI();
}

void frmLift::UpdateUI()
{
    QPixmap bgImage("image//Lift.png");

    if (!bgImage.isNull())////缩放
    {
        lbl_pic->setAlignment(Qt::AlignCenter);

        int TempWidth = this->width() - frame_Left->width() - frame_Right->width() - 80;

        lbl_pic->setPixmap(bgImage.scaled(QSize(TempWidth, 260),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void frmLift::closeEvent(QCloseEvent *event)
{
    if (this->isModal())
    {
        //qDebug() <<"deleteLater";
        this->deleteLater();
        event->accept();
    }
}
