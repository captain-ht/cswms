#ifndef FRMCONFIGPOSITION_H
#define FRMCONFIGPOSITION_H

#include <QWidget>

class DeviceButton;

namespace Ui {
class frmConfigPosition;
}

class frmConfigPosition : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigPosition(QWidget *parent = 0);
    ~frmConfigPosition();

private:
    Ui::frmConfigPosition *ui;
    QList<DeviceButton *> btns;

private slots:
    void initForm();
    void initList();

private slots:
    void on_listWidget_pressed();
    void on_btnSave_clicked();
};

#endif // FRMCONFIGPOSITION_H
