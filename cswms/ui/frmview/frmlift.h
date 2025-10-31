#ifndef FRMLIFT_H
#define FRMLIFT_H

#include "ui_frmlift.h"
#include <QCloseEvent>

class frmLift : public QWidget, private Ui::frmLift
{
    Q_OBJECT

public:
    explicit frmLift(QWidget *parent = nullptr);
    void UpdateUI();
protected:

    void closeEvent(QCloseEvent * event);
};

#endif // FRMLIFT_H
