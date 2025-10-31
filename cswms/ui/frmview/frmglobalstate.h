#ifndef FRMGLOBALSTATE_H
#define FRMGLOBALSTATE_H

#include "ui_frmglobalstate.h"

class frmGlobalState : public QWidget, private Ui::frmGlobalState
{
    Q_OBJECT

public:
    explicit frmGlobalState(QWidget *parent = nullptr);
    void UpdateUI();

    void UpdateLblState(QLabel * templbl, bool IsRed = true);
};

#endif // FRMGLOBALSTATE_H
