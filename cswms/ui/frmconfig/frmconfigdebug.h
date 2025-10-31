#ifndef FRMCONFIGDEBUG_H
#define FRMCONFIGDEBUG_H

#include <QWidget>

namespace Ui {
class frmConfigDebug;
}

class frmConfigDebug : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigDebug(QWidget *parent = 0);
    ~frmConfigDebug();

private:
    Ui::frmConfigDebug *ui;
    QStringList keys1, keys2;

private slots:
    void initForm();
    void initConfig();
    void saveConfig();
    void append(int type, const QString &portName, quint8 addr, const QString &data, bool clear = false);

private slots:
    void sendData(const QString &portName, quint8 addr, const QByteArray &data);
    void receiveData(const QString &portName, quint8 addr, const QByteArray &data);
    void receiveInfo(const QString &portName, quint8 addr, const QString &data);
    void receiveError(const QString &portName, quint8 addr, const QString &data);

    void debugDbLocal(const QString &msg);
    void debugDbNet(const QString &msg);
    void debugDbReceive(const QString &msg);

private slots:
    void on_btnClearData_clicked();
    void on_btnReadValueAll_clicked();
    void on_cboxPortName_currentIndexChanged(int);
    void on_ckOther_stateChanged(int arg1);
};

#endif // FRMCONFIGDEBUG_H
