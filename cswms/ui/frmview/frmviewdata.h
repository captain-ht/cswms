#ifndef FRMVIEWDATA_H
#define FRMVIEWDATA_H

#include <QWidget>

class QTableWidget;

namespace Ui {
class frmViewData;
}

class frmViewData : public QWidget
{
    Q_OBJECT

public:
    explicit frmViewData(QWidget *parent = 0);
    ~frmViewData();
    QTableWidget *getTable();

private:
    Ui::frmViewData *ui;

};

#endif // FRMVIEWDATA_H
