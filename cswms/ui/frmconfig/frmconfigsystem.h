#ifndef FRMCONFIGSYSTEM_H
#define FRMCONFIGSYSTEM_H

#include <QWidget>

namespace Ui {
class frmConfigSystem;
}

class frmConfigSystem : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigSystem(QWidget *parent = 0);
    ~frmConfigSystem();

private:
    Ui::frmConfigSystem *ui;

private slots:
    void initForm();
    void initConfig1();
    void saveConfig1();
    void initConfig2();
    void saveConfig2();
    void initConfig3();
    void saveConfig3();

    void initLog();
    void saveLog();
    void initColor();
    void saveColor();

    void initNetLink();
    void saveNetLink();
    void initMsgLink();
    void saveMsgLink();
    void initEmailLink();
    void saveEmailLink();

    void initTime();
    void saveTime();
    QString saveExcel();

private slots:
    void on_btnSendData_clicked();
    void on_btnSendMsg_clicked();
    void on_btnSendEmail_clicked();
};

#endif // FRMCONFIGSYSTEM_H
