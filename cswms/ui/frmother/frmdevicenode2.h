#ifndef FRMDEVICENODE2_H
#define FRMDEVICENODE2_H

#include <QWidget>

namespace Ui
{
    class frmDeviceNode2;
}

class frmDeviceNode2 : public QWidget
{
    Q_OBJECT

public:
    explicit frmDeviceNode2(QWidget *parent = 0);
    ~frmDeviceNode2();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmDeviceNode2 *ui;
    QString positionID;
    QString deviceName;
    QString nodeName;
    QString nodeType;
    QString nodeSign;
    float value;

    bool online;
    bool alarm;
    QString qss;

public:
    QString getPositionID();
    QString getDeviceName();
    QString getNodeName();

public slots:
    void initStyle();
    void initForm();    

public slots:
    void setPositionID(const QString &positionID);
    void setDeviceName(const QString &deviceName);
    void setNodeName(const QString &nodeName);
    void setNodeType(const QString &nodeType);
    void setNodeSign(const QString &nodeSign);
    void setValue(float value);

    void setOnline(bool online);
    void setAlarm(bool alarm);
    void setSelect(bool select);
};

#endif // FRMDEVICENODE2_H
