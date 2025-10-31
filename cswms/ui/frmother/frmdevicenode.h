#ifndef FRMDEVICENODE_H
#define FRMDEVICENODE_H

#include <QWidget>

namespace Ui
{
    class frmDeviceNode;
}

class frmDeviceNode : public QWidget
{
    Q_OBJECT

public:
    explicit frmDeviceNode(QWidget *parent = 0);
    ~frmDeviceNode();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmDeviceNode *ui;
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

#endif // FRMDEVICENODE_H
