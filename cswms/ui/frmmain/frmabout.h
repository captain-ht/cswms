#ifndef FRMABOUT_H
#define FRMABOUT_H

#include <QDialog>

namespace Ui {
class frmAbout;
}

class frmAbout : public QDialog
{
    Q_OBJECT

public:
    static frmAbout *Instance();
    explicit frmAbout(QWidget *parent = 0);
    ~frmAbout();

private:
    Ui::frmAbout *ui;
    static frmAbout *self;

private slots:
    void initForm();
};

#endif // FRMABOUT_H
