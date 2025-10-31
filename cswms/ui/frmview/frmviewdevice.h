#ifndef FRMVIEWDEVICE_H
#define FRMVIEWDEVICE_H

#include <QWidget>

class PanelWidget;

namespace Ui {
class frmViewDevice;
}

class frmViewDevice : public QWidget
{
    Q_OBJECT

public:
    explicit frmViewDevice(QWidget *parent = 0);
    ~frmViewDevice();
    PanelWidget *getPanleWidget();

private:
    Ui::frmViewDevice *ui;    

private slots:
    void itemClicked(const QString &text);

};

#endif // FRMVIEWDEVICE_H
