#ifndef FRMDEVICEPANEL_H
#define FRMDEVICEPANEL_H

#include <QWidget>

class QLabel;

namespace Ui
{
    class frmDevicePanel;
}

class frmDevicePanel : public QWidget
{
    Q_OBJECT

public:
    explicit frmDevicePanel(QWidget *parent = 0);
    ~frmDevicePanel();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmDevicePanel *ui;

    QString portName;
    QString deviceName;
    quint8 deviceAddr;
    QString deviceType;
    int nodeNumber;
    bool online;
    bool alarm;

    bool red;
    QTimer *timerAlarm;

    QList<QLabel *> labIds;
    QList<QLabel *> labValues;
    QList<QLabel *> labSigns;

public:
    QString getPortName();
    QString getDeviceName();
    quint8 getDeviceAddr();
    QString getDeviceType();
    int getNodeNumber();

private slots:
    void initForm();
    void changeColor();
    void changeStatus(quint8 type, bool alarm);

public slots:    
    void setPortName(const QString &portName);
    void setDeviceName(const QString &deviceName);
    void setDeviceAddr(quint8 deviceAddr);
    void setDeviceType(const QString &deviceType);
    void setNodeNumber(int nodeNumber);
    void setOnline(bool online);
    void setAlarm(bool alarm);
};

#endif // FRMDEVICEPANEL_H
