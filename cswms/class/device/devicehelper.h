#ifndef DEVICEHELPER_H
#define DEVICEHELPER_H

#include "head.h"

class PanelWidget;
class DeviceButton;
class frmDeviceNode;
class frmDeviceNode2;

class DeviceHelper
{
public:
    //设置对应的控件
    static void setTableData(QTableWidget *tableData);
    static void setTableWidget(QTableWidget *tableWidget);
    static void setTreeWidget(QTreeWidget *treeWidget);
    static void setListWidget(QListWidget *listWidget);
    static void setPanelWidget(PanelWidget *panelWidget);
    static void setLabel(QLabel *label);

    //获取控件集合
    static QList<DeviceButton *> getBtns();
    static QList<frmDeviceNode *> getDevices();
    static QList<frmDeviceNode2 *> getDevices2();

    //初始化设备表格
    static void initDeviceData();
    static void addData(const QStringList &data);

    //初始化表格消息
    static void initDeviceTable();
    static void addMsg(const QString &id, float value);
    static void clearMsg();

    //初始化设备树状表
    static void initDeviceTree();
    static void initDeviceTree(QTreeWidget *treeWidget);

    //初始化设备地图
    static void initDeviceMap();
    static void initDeviceMap(QListWidget *listWidget);

    //显示当前地图设备
    static void initDeviceMapCurrent(const QString &imgName);
    static void initDeviceMapCurrent(QLabel *label, const QString &imgName);

    //初始化设备按钮
    static void initDeviceButton();
    static void initDeviceButton(QLabel *label);

    //保存+改变设备位置
    static void saveDeviceButtonPosition();
    static void changeDeviceButtonPosition();    

    //初始化设备面板和样式
    static void initDevicePanel();
    static void initDeviceStyle();

    //设备上线离线+接收到值+报警
    static void deviceOnline(const QString &deviceName, bool online);
    static void deviceOnline2(const QString &positionID, bool online);
    static void deviceValue(const QString &positionID, float value, const QString &nodeSign = "");
    static void deviceAlarm(const QString &positionID, quint8 alarmType);

    //设置表格颜色
    static void setColor(QTableWidgetItem *itemValue, QTableWidgetItem *itemStatus, quint8 alarmType);
    static void setColor(QTableWidgetItem *itemValue, QTableWidgetItem *itemStatus,
                         float value, float max, float min, const QString &alarmType);

    //播放声音
    static void playAlarm(const QString &soundName, bool stop = false);
    static void playSound(const QString &soundName, int playCount = 1, bool stop = true);
    static void stopSound();
};

#endif // DEVICEHELPER_H
