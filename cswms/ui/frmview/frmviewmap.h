#ifndef FRMVIEWMAP_H
#define FRMVIEWMAP_H

#include <QWidget>

class QLabel;

namespace Ui {
class frmViewMap;
}

class frmViewMap : public QWidget
{
    Q_OBJECT

public:
    explicit frmViewMap(QWidget *parent = 0);
    ~frmViewMap();
    QLabel *getLabel();

private:
    Ui::frmViewMap *ui;

};

#endif // FRMVIEWMAP_H
