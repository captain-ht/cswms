#ifndef FRMVIEWCRANE_H
#define FRMVIEWCRANE_H

#include "ui_frmviewcrane.h"

class frmviewcrane : public QWidget, private Ui::frmviewcrane
{
    Q_OBJECT

public:
    explicit frmviewcrane(QWidget *parent = nullptr);
};

#endif // FRMVIEWCRANE_H
