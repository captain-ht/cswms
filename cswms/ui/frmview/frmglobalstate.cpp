#include "frmglobalstate.h"

frmGlobalState::frmGlobalState(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    this->setWindowTitle("总体监控详情");
    UpdateUI();
}

void frmGlobalState::UpdateUI()
{
    QPixmap bgImage("image//BigCartTop.png");

    if (!bgImage.isNull())////缩放
    {
        lbl_pic->setAlignment(Qt::AlignCenter);

        int TempWidth = this->width() - 80;

        lbl_pic->setPixmap(bgImage.scaled(QSize(TempWidth, 260),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    //lblLeftLimit_30->setStyleSheet("background-color: rgb(0, 255, 0);");
    UpdateLblState(lblLeftLimit_30, false);
}

void frmGlobalState::UpdateLblState(QLabel * templbl, bool IsRed)
{
    if (IsRed)
        templbl->setStyleSheet("background-color: rgb(255, 0, 0);");
    else
        templbl->setStyleSheet("background-color: rgb(0, 255, 0);");
}
