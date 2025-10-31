#ifndef FRMSMALL_H
#define FRMSMALL_H

#include "ui_frmsmall.h"
#include <QCloseEvent>

//小车对应的界面与CPP文件

class frmSmall : public QWidget, private Ui::frmSmall
{
    Q_OBJECT

public:
    explicit frmSmall(QWidget *parent = nullptr);
    void UpdateUI();
protected:

    void closeEvent(QCloseEvent * event);
};

#endif // FRMSMALL_H
