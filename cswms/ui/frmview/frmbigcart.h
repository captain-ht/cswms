#ifndef FRMBIGCART_H
#define FRMBIGCART_H

#include "ui_frmbigcart.h"
#include <QCloseEvent>

class frmBigCart : public QWidget, private Ui::frmBigCart
{
    Q_OBJECT

public:
    explicit frmBigCart(QWidget *parent = nullptr);
    void UpdateUI();
protected:

    void closeEvent(QCloseEvent * event);
};

#endif // FRMBIGCART_H
