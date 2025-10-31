#ifndef FRMDEVICECONTROL_H
#define FRMDEVICECONTROL_H

#include <QWidget>

namespace Ui {
class frmDeviceControl;
}

class frmDeviceControl : public QWidget
{
    Q_OBJECT

public:
    static frmDeviceControl *Instance();
    explicit frmDeviceControl(QWidget *parent = 0);
    ~frmDeviceControl();

private:
    Ui::frmDeviceControl *ui;
    static frmDeviceControl *self;
    QString positionID;

private slots:
    void initForm();

public slots:
    void setPositionID(const QString &positionID);

};

#endif // FRMDEVICECONTROL_H
