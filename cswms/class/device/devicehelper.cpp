#include "devicehelper.h"
#include "quihelper.h"

#include "playwav.h"
#include "dbquery.h"
#include "devicebutton.h"
#include "panelwidget.h"
#include "devicemap.h"

#include "frmdevicenode.h"
#include "frmdevicenode2.h"
#include "frmdevicepannel.h"
#include "tableitemdelegate.h"

namespace {
QTableWidget *deviceData = NULL;    //显示表格数据
QTableWidget *deviceTable = NULL;   //显示临时消息
QTreeWidget *deviceTree = NULL;     //设备树状列表
QListWidget *deviceMap = NULL;      //地图缩略图
PanelWidget *devicePanel = NULL;    //设备面板
QLabel *labMap = NULL;              //显示背景图和存放设备控件

QList<DeviceButton *> btns;        //设备按钮集合
QList<DeviceButton *> btns2;       //设备按钮集合
QList<QWidget *> widgets;           //设备面板集合
QList<frmDeviceNode *> devices;     //设备元素集合
QList<frmDeviceNode2 *> devices2;   //设备元素集合
}

void DeviceHelper::setTableData(QTableWidget *tableData)
{
    deviceData = tableData;
    deviceData->setRowCount(DbData::NodeInfo_Count);
    QUIHelper::initTableView(deviceData);
}

void DeviceHelper::setTableWidget(QTableWidget *tableWidget)
{
    deviceTable = tableWidget;
    deviceTable->setRowCount(AppConfig::MsgCount);
    QUIHelper::initTableView(deviceTable);
}

void DeviceHelper::setTreeWidget(QTreeWidget *treeWidget)
{
    deviceTree = treeWidget;
}

void DeviceHelper::setListWidget(QListWidget *listWidget)
{
    deviceMap = listWidget;
}

void DeviceHelper::setPanelWidget(PanelWidget *panelWidget)
{
    devicePanel = panelWidget;
}

void DeviceHelper::setLabel(QLabel *label)
{
    labMap = label;
}

QList<DeviceButton *> DeviceHelper::getBtns()
{
    return btns;
}

QList<frmDeviceNode *> DeviceHelper::getDevices()
{
    return devices;
}

QList<frmDeviceNode2 *> DeviceHelper::getDevices2()
{
    return devices2;
}

void DeviceHelper::initDeviceData()
{
    if (deviceData == NULL) {
        return;
    }

    QList<QString> columnNames = AppData::TableDataColumns.split("|");
    QList<int> columnWidths;
    if (QUIHelper::deskWidth() < 1360) {
        columnWidths << 50 << 100 << 110 << 100 << 120 << 90 << 80 << 70 << 70 << 70;
    } else if (QUIHelper::deskWidth() < 1440) {
        columnWidths << 50 << 110 << 130 << 110 << 140 << 100 << 80 << 70 << 70 << 70;
    } else {
        columnWidths << 55 << 115 << 140 << 130 << 150 << 120 << 80 << 70 << 70 << 70;
    }

    int columnCount = columnNames.count();
    deviceData->setColumnCount(columnCount);
    deviceData->setHorizontalHeaderLabels(columnNames);
    for (int i = 0; i < columnCount; i++) {
        deviceData->setColumnWidth(i, columnWidths.at(i));
    }

    //载入数据
    for (int i = 0; i < DbData::NodeInfo_Count; i++) {
        //过滤没有对应设备的变量
        int index = DbData::DeviceInfo_DeviceName.indexOf(DbData::NodeInfo_DeviceName.at(i));
        if (index < 0) {
            continue;
        }

        QStringList data;
        data << QString::number(i + 1);
        data << DbData::NodeInfo_PositionID.at(i);
        data << DbData::NodeInfo_DeviceName.at(i);
        data << DbData::DeviceInfo_DeviceType.at(index);
        data << DbData::NodeInfo_NodeName.at(i);
        data << DbData::NodeInfo_NodeType.at(i);
        data << DbData::NodeInfo_NodeClass.at(i);
        data << QString::number(0);
        data << DbData::NodeInfo_NodeSign.at(i);
        data << "离线";
        data << QString::number(DbData::NodeInfo_AlarmH.at(i));
        data << QString::number(DbData::NodeInfo_AlarmL.at(i));
        data << DbData::NodeInfo_AlarmType.at(i);
        addData(data);
    }

    deviceData->setItemDelegate(new TableItemDelegate);
}

void DeviceHelper::addData(const QStringList &data)
{
    if (deviceData == NULL) {
        return;
    }

    static int row = -1;
    row++;
    for (int i = 0; i < data.count() - 2; i++) {
        QTableWidgetItem *item = new QTableWidgetItem(data.at(i));
        item->setTextAlignment(Qt::AlignCenter);

        //加粗+不同颜色显示对应的数值
        if (i == 7 || i == 9) {
            QFont font;
            font.setPixelSize(QUIConfig::FontSize + 2);
            font.setBold(true);
            item->setFont(font);

            if (i == 9) {
                float value = data.at(i - 2).toFloat();
                float max = data.at(i + 1).toFloat();
                float min = data.at(i + 2).toFloat();
                setColor(deviceData->item(row, 7), item, value, max, min, data.at(i + 3));
            }
        }

        deviceData->setItem(row, i, item);
    }
}

void DeviceHelper::initDeviceTable()
{
    if (deviceTable == NULL) {
        return;
    }

    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "时间" << "位号" << "报警值";
    columnWidths << 60 << 80 << 40;

    int columnCount = columnNames.count();
    deviceTable->setColumnCount(columnCount);
    deviceTable->setHorizontalHeaderLabels(columnNames);
    for (int i = 0; i < columnCount; i++) {
        deviceTable->setColumnWidth(i, columnWidths.at(i));
    }
}

void DeviceHelper::addMsg(const QString &id, float value)
{
    if (deviceTable == NULL || AppConfig::MsgCount == 0) {
        return;
    }

    //移除末尾一行,将当前消息插入到第一行
    deviceTable->removeRow(deviceTable->rowCount() - 1);
    deviceTable->insertRow(0);

    QTableWidgetItem *itemTime = new QTableWidgetItem(TIME);
    itemTime->setTextAlignment(Qt::AlignCenter);
    deviceTable->setItem(0, 0, itemTime);

    QTableWidgetItem *itemID = new QTableWidgetItem(id);
    itemID->setTextAlignment(Qt::AlignCenter);
    deviceTable->setItem(0, 1, itemID);

    QTableWidgetItem *itemValue = new QTableWidgetItem(QString::number(value));
    itemValue->setTextAlignment(Qt::AlignCenter);
    deviceTable->setItem(0, 2, itemValue);
}

void DeviceHelper::clearMsg()
{
    if (deviceTable == NULL) {
        return;
    }

    deviceTable->clearContents();
}

void DeviceHelper::initDeviceTree()
{
    if (deviceTree == NULL) {
        return;
    }

    initDeviceTree(deviceTree);
}

void DeviceHelper::initDeviceTree(QTreeWidget *treeWidget)
{
    treeWidget->clear();
    treeWidget->setColumnCount(1);
    treeWidget->setHeaderHidden(true);
    treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeWidget->setIconSize(QSize(AppData::BtnIconSize, AppData::BtnIconSize));

#if 1
    for (int i = 0; i < DbData::DeviceInfo_Count; i++) {
        //添加父变量
        QString deviceName = DbData::DeviceInfo_DeviceName.at(i);
        QTreeWidgetItem *itemParent = new QTreeWidgetItem(treeWidget);
        itemParent->setText(0, deviceName);

        //循环添加子变量
        for (int j = 0; j < DbData::NodeInfo_Count; j++) {
            if (DbData::NodeInfo_DeviceName.at(j) == deviceName) {
                QTreeWidgetItem *itemChild = new QTreeWidgetItem(itemParent);
                itemChild->setText(0, DbData::NodeInfo_NodeName.at(j));
                QPixmap iconNormal = IconHelper::getPixmap(QUIConfig::TextColor, 0xe67b, 18, 20, 20);
                itemChild->setIcon(0, iconNormal);
            }
        }
    }
#else
    int count = AppData::MapNames.count();
    for (int i = 0; i < count; i++) {
        //添加父变量
        QString mapName = AppData::MapNames.at(i);
        QString name = mapName.split(".").first();
        QTreeWidgetItem *itemParent = new QTreeWidgetItem(treeWidget);
        itemParent->setText(0, name);

        //循环添加子变量
        for (int j = 0; j < DbData::NodeInfo_Count; j++) {
            if (DbData::NodeInfo_NodeImage.at(j) == mapName) {
                QTreeWidgetItem *itemChild = new QTreeWidgetItem(itemParent);
                itemChild->setText(0, DbData::NodeInfo_NodeName.at(j));
                QPixmap iconNormal = IconHelper::getPixmap(QUIConfig::TextColor, 0xe67b, 18, 20, 20);
                itemChild->setIcon(0, iconNormal);
            }
        }

        if (i < 2) {
            //treeWidget->expandItem(itemParent);
        }
    }
#endif

    treeWidget->expandAll();
}

void DeviceHelper::initDeviceMap()
{
    if (deviceMap == NULL) {
        return;
    }

    initDeviceMap(deviceMap);
}

void DeviceHelper::initDeviceMap(QListWidget *listWidget)
{
    listWidget->clear();
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setMovement(QListView::Static);
    listWidget->setIconSize(QSize(AppData::RightWidth - 20, AppData::RightWidth - 20));

    int count = AppData::MapNames.count();
    for (int i = 0; i < count; i++) {
        QString imageName = AppData::MapNames.at(i);
        QPixmap pix = DeviceMap::Instance()->getMapPix(imageName);

        //取文件名的前字符+末尾字符+去掉拓展名
        QString itemName = imageName;
        itemName = itemName.split(".").first();
        if (itemName.length() > 16) {
            itemName = QString("%1...%2").arg(itemName.left(8)).arg(itemName.right(8));
        }

        QListWidgetItem *listItem = new QListWidgetItem(listWidget, i);
        listItem->setIcon(QIcon(pix));
        listItem->setText(itemName);
        listItem->setData(Qt::UserRole, imageName);
        listItem->setTextAlignment(Qt::AlignCenter);
    }
}

void DeviceHelper::initDeviceMapCurrent(const QString &imgName)
{
    if (labMap == NULL) {
        return;
    }

    initDeviceMapCurrent(labMap, imgName);
}

void DeviceHelper::initDeviceMapCurrent(QLabel *label, const QString &imgName)
{
    if (labMap != NULL && label == labMap) {
        AppData::CurrentImage = imgName;
    }

    label->setProperty("image", imgName);
    //优先取带防区线条的图像
    QString mapName = "normal_" + imgName;
    if (DeviceMap::Instance()->existMapPix(mapName)) {
        label->setPixmap(DeviceMap::Instance()->getMapPix(mapName));
    } else {
        label->setPixmap(DeviceMap::Instance()->getMapPix(imgName));
    }

    QList<DeviceButton *> btns = label->findChildren<DeviceButton *>();
    foreach (DeviceButton *btn, btns) {
        btn->setVisible(btn->property("nodeImage").toString() == imgName);
    }
}

void DeviceHelper::initDeviceButton()
{
    if (labMap == NULL) {
        return;
    }

    initDeviceButton(labMap);
}

void DeviceHelper::initDeviceButton(QLabel *label)
{
    bool isMain = (labMap == label);
    if (isMain) {
        qDeleteAll(btns);
        btns.clear();
    } else {
        qDeleteAll(btns2);
        btns2.clear();
    }

    for (int i = 0; i < DbData::NodeInfo_Count; i++) {
        //地图为空的不需要
        if (DbData::NodeInfo_NodeImage.at(i) == " -- 无 -- ") {
            continue;
        }

        DeviceButton *btn = new DeviceButton(label);
        btn->setButtonStyle(DeviceButton::ButtonStyle_Msg2);
        btn->resize(AppData::DeviceWidth, AppData::DeviceHeight);
        btn->setText(QString("%1").arg(DbData::NodeInfo_PositionID.at(i)));
        btn->setButtonColor(isMain ? DeviceButton::ButtonColor_Black : DeviceButton::ButtonColor_Green);
        btn->setCanMove(!isMain);

        //设置弱属性
        btn->setProperty("nodeID", DbData::NodeInfo_NodeID.at(i));
        btn->setProperty("positionID", DbData::NodeInfo_PositionID.at(i));
        btn->setProperty("deviceName", DbData::NodeInfo_DeviceName.at(i));
        btn->setProperty("nodeName", DbData::NodeInfo_NodeName.at(i));
        btn->setProperty("nodeAddr", DbData::NodeInfo_NodeAddr.at(i));
        btn->setProperty("nodeType", DbData::NodeInfo_NodeType.at(i));
        btn->setProperty("nodeSign", DbData::NodeInfo_NodeSign.at(i));
        btn->setProperty("nodeImage", DbData::NodeInfo_NodeImage.at(i));

        //设置位置
        int x = DbData::NodeInfo_NodeX.at(i);
        int y = DbData::NodeInfo_NodeY.at(i);
        btn->move(x, y);

        if (isMain) {
            btns << btn;
            QObject::connect(btn, SIGNAL(doubleClicked()), AppEvent::Instance(), SLOT(slot_doubleClicked()));
        } else {
            btns2 << btn;
        }
    }
}

void DeviceHelper::saveDeviceButtonPosition()
{
    QSqlDatabase::database().transaction();

    //构建设备位置更新sql,逐个保存位置
    foreach (DeviceButton *btn, btns2) {
        DbQuery::updatePosition(btn);
    }

    if (!QSqlDatabase::database().commit()) {
        QSqlDatabase::database().rollback();
    } else {
        DbQuery::loadNodeInfo();
        changeDeviceButtonPosition();
    }
}

void DeviceHelper::changeDeviceButtonPosition()
{
    //改变现有设备的位置
    foreach (DeviceButton *btn, btns) {
        int nodeID = btn->property("nodeID").toInt();
        int index = DbData::NodeInfo_NodeID.indexOf(nodeID);
        if (index >= 0) {
            int x = DbData::NodeInfo_NodeX.at(index);
            int y = DbData::NodeInfo_NodeY.at(index);
            btn->move(x, y);
        }
    }
}

void DeviceHelper::initDevicePanel()
{
    if (devicePanel == NULL) {
        return;
    }

    qDeleteAll(widgets);
    qDeleteAll(devices);
    widgets.clear();
    devices.clear();
    devices2.clear();

    bool autoWidth = (DbData::NodeInfo_Count >= AppConfig::DeviceColumn);
    if (AppConfig::PanelMode == 0) {
        for (int i = 0; i < DbData::NodeInfo_Count; i++) {
            frmDeviceNode *widget = new frmDeviceNode;
            widget->setPositionID(DbData::NodeInfo_PositionID.at(i));
            widget->setDeviceName(DbData::NodeInfo_DeviceName.at(i));
            widget->setNodeName(DbData::NodeInfo_NodeName.at(i));
            widget->setNodeType(DbData::NodeInfo_NodeType.at(i));
            widget->setNodeSign(DbData::NodeInfo_NodeSign.at(i));
            widget->setOnline(false);

            if (autoWidth) {
                widget->setMinimumWidth(150);
            } else {
                widget->setFixedWidth(150);
            }

            widget->setFixedHeight(widget->sizeHint().height() + 15);
            widgets << widget;
            devices << widget;
        }
    } else if (AppConfig::PanelMode == 1) {
        for (int i = 0; i < DbData::NodeInfo_Count; i++) {
            frmDeviceNode2 *widget = new frmDeviceNode2;
            widget->setPositionID(DbData::NodeInfo_PositionID.at(i));
            widget->setDeviceName(DbData::NodeInfo_DeviceName.at(i));
            widget->setNodeName(DbData::NodeInfo_NodeName.at(i));
            widget->setNodeType(DbData::NodeInfo_NodeType.at(i));
            widget->setNodeSign(DbData::NodeInfo_NodeSign.at(i));
            widget->setOnline(false);

            if (autoWidth) {
                widget->setMinimumWidth(200);
            } else {
                widget->setFixedWidth(200);
            }

            widget->setFixedHeight(widget->sizeHint().height() - 20);
            widgets << widget;
            devices2 << widget;
        }
    } else {
        for (int i = 0; i < DbData::DeviceInfo_Count; i++) {
            frmDevicePanel *widget = new frmDevicePanel;
            widget->setPortName(DbData::DeviceInfo_PortName.at(i));
            widget->setDeviceName(DbData::DeviceInfo_DeviceName.at(i));
            widget->setDeviceAddr(DbData::DeviceInfo_DeviceAddr.at(i));
            widget->setDeviceType(DbData::DeviceInfo_DeviceType.at(i));
            widget->setNodeNumber(DbData::DeviceInfo_NodeNumber.at(i));
            widgets << widget;
        }
    }

    devicePanel->setMargin(3);
    devicePanel->setSpace(3);
    devicePanel->setAutoWidth(autoWidth);
    devicePanel->setColumnCount(AppConfig::DeviceColumn);
    devicePanel->setWidgets(widgets);
}

void DeviceHelper::initDeviceStyle()
{
    if (AppConfig::PanelMode == 0) {
        foreach (frmDeviceNode *device, devices) {
            device->initStyle();
        }
    } else if (AppConfig::PanelMode == 1) {
        foreach (frmDeviceNode2 *device, devices2) {
            device->initStyle();
        }
    }
}

void DeviceHelper::deviceOnline(const QString &deviceName, bool online)
{
    //处理设备面板
    if (AppConfig::PanelMode == 0) {
        foreach (frmDeviceNode *device, devices) {
            if (device->getDeviceName() == deviceName) {
                device->setOnline(online);
            }
        }
    } else if (AppConfig::PanelMode == 1) {
        foreach (frmDeviceNode2 *device, devices2) {
            if (device->getDeviceName() == deviceName) {
                device->setOnline(online);
            }
        }
    }

    //处理设备按钮
    foreach (DeviceButton *btn, btns) {
        if (btn->property("deviceName").toString() == deviceName) {
            if (online) {
                btn->setButtonColor(DeviceButton::ButtonColor_Green);
            } else {
                btn->setButtonColor(DeviceButton::ButtonColor_Black);
                btn->setText(btn->property("positionID").toString());
            }

            //云端数据同步
            if (AppConfig::UseNetDb) {
                QString positionID = btn->property("positionID").toString();
                QString sql = QString("update NodeData set NodeStatus='%1' where PositionID='%2'").arg(online ? 1 : 0).arg(positionID);
                DbData::DbNet->append(sql);
            }
        }
    }

    //处理设备表格
    int count = deviceData->rowCount();
    for (int i = 0; i < count; i++) {
        QString name = deviceData->item(i, 2)->text();
        if (name == deviceName) {
            setColor(deviceData->item(i, 7), deviceData->item(i, 9), online ? 1 : 4);
        }
    }
}

void DeviceHelper::deviceOnline2(const QString &positionID, bool online)
{
    //处理设备面板
    if (AppConfig::PanelMode == 0) {
        foreach (frmDeviceNode *device, devices) {
            if (device->getPositionID() == positionID) {
                device->setOnline(online);
                break;
            }
        }
    } else if (AppConfig::PanelMode == 1) {
        foreach (frmDeviceNode2 *device, devices2) {
            if (device->getPositionID() == positionID) {
                device->setOnline(online);
                break;
            }
        }
    }

    //处理设备按钮
    foreach (DeviceButton *btn, btns) {
        if (btn->property("positionID").toString() == positionID) {
            if (online) {
                btn->setButtonColor(DeviceButton::ButtonColor_Green);
            } else {
                btn->setButtonColor(DeviceButton::ButtonColor_Black);
                btn->setText(btn->property("positionID").toString());
            }

            //云端数据同步
            if (AppConfig::UseNetDb) {
                QString positionID = btn->property("positionID").toString();
                QString sql = QString("update NodeData set NodeStatus='%1' where PositionID='%2'").arg(online ? 1 : 0).arg(positionID);
                DbData::DbNet->append(sql);
            }

            break;
        }
    }

    //处理设备表格
    int count = deviceData->rowCount();
    for (int i = 0; i < count; i++) {
        QString id = deviceData->item(i, 1)->text();
        if (id == positionID) {
            setColor(deviceData->item(i, 7), deviceData->item(i, 9), online ? 1 : 4);
            break;
        }
    }
}

void DeviceHelper::deviceValue(const QString &positionID, float value, const QString &nodeSign)
{
    //下面的不会被频繁执行,只有数据变动了才会执行,已经在解析的地方做了过滤

    //处理设备面板
    if (AppConfig::PanelMode == 0) {
        foreach (frmDeviceNode *device, devices) {
            if (device->getPositionID() == positionID) {
                device->setValue(value);
                if (!nodeSign.isEmpty()) {
                    device->setNodeSign(nodeSign);
                }
                break;
            }
        }
    } else if (AppConfig::PanelMode == 1) {
        foreach (frmDeviceNode2 *device, devices2) {
            if (device->getPositionID() == positionID) {
                device->setValue(value);
                if (!nodeSign.isEmpty()) {
                    device->setNodeSign(nodeSign);
                }
                break;
            }
        }
    }

    //处理设备按钮
    bool alarm = false;
    foreach (DeviceButton *btn, btns) {
        if (btn->property("positionID").toString() == positionID) {
            alarm = (btn->getButtonColor() == DeviceButton::ButtonColor_Red);
            btn->setProperty("value", value);

            //有两个传感器 是开关量 数值是 1、2  显示时 用正常 和异常代替
            QString nodeType = btn->property("nodeType").toString();
            if (nodeType == "SJ-0001" || nodeType == "JG-0001") {
                btn->setText(value == 1 ? "正常" : "异常");
            } else {
                btn->setText(QString("%1 %2").arg(value).arg(btn->property("nodeSign").toString()));
            }

            if (!nodeSign.isEmpty()) {
                btn->setProperty("nodeSign", nodeSign);
            }

            break;
        }
    }

    //处理设备表格
    int count = deviceData->rowCount();
    for (int i = 0; i < count; i++) {
        QString id = deviceData->item(i, 1)->text();
        if (id == positionID) {
            deviceData->item(i, 7)->setText(QString::number(value));
            if (!nodeSign.isEmpty()) {
                deviceData->item(i, 8)->setText(nodeSign);
            }

            break;
        }
    }
}

void DeviceHelper::deviceAlarm(const QString &positionID, quint8 alarmType)
{
    //0-高报 2-低报 5-其他
    bool alarm = (alarmType == 0 || alarmType == 2 || alarmType == 5);

    //处理设备面板
    if (AppConfig::PanelMode == 0) {
        foreach (frmDeviceNode *device, devices) {
            if (device->getPositionID() == positionID) {
                device->setAlarm(alarm);
                break;
            }
        }
    } else if (AppConfig::PanelMode == 1) {
        foreach (frmDeviceNode2 *device, devices2) {
            if (device->getPositionID() == positionID) {
                device->setAlarm(alarm);
                break;
            }
        }
    }

    //处理设备按钮
    foreach (DeviceButton *btn, btns) {
        if (btn->property("positionID").toString() == positionID) {
            btn->setButtonColor(alarm ? DeviceButton::ButtonColor_Red : DeviceButton::ButtonColor_Green);
            //切换到当前地图
            initDeviceMapCurrent(btn->property("nodeImage").toString());
            break;
        }
    }

    //处理设备表格
    int count = deviceData->rowCount();
    for (int i = 0; i < count; i++) {
        QString id = deviceData->item(i, 1)->text();
        if (id == positionID) {
            setColor(deviceData->item(i, 7), deviceData->item(i, 9), alarmType);
            break;
        }
    }

    //云端数据同步
    if (AppConfig::UseNetDb) {
        int nodeStatus = 1;
        if (alarmType == 0) {
            nodeStatus = 3;
        } else if (alarmType == 2) {
            nodeStatus = 2;
        } else if (alarmType == 5) {
            nodeStatus = 5;
        }

        QString sql = QString("update NodeData set NodeStatus='%1' where PositionID='%2'").arg(nodeStatus).arg(positionID);
        DbData::DbNet->append(sql);
    }
}

void DeviceHelper::setColor(QTableWidgetItem *itemValue, QTableWidgetItem *itemStatus, quint8 alarmType)
{
    //0-低报 1-低报恢复 2-高报 3-高报恢复 5-其他报警 6-其他报警恢复
    if (alarmType == 0) {
        itemValue->setForeground(QColor(AppConfig::ColorLimit));
        itemStatus->setForeground(QColor(AppConfig::ColorLimit));
        itemStatus->setText("低报");
    } else if (alarmType == 2) {
        itemValue->setForeground(QColor(AppConfig::ColorUpper));
        itemStatus->setForeground(QColor(AppConfig::ColorUpper));
        itemStatus->setText("高报");
    } else if (alarmType == 1 || alarmType == 3 || alarmType == 6) {
        itemValue->setForeground(QColor(AppConfig::ColorNormal));
        itemStatus->setForeground(QColor(AppConfig::ColorNormal));
        itemStatus->setText("正常");
    } else if (alarmType == 4) {
        itemValue->setForeground(QColor(AppConfig::ColorZero));
        itemStatus->setForeground(QColor(AppConfig::ColorZero));
        itemStatus->setText("离线");
        itemValue->setText("0");
    } else if (alarmType == 5) {
        itemValue->setForeground(QColor(AppConfig::ColorOther));
        itemStatus->setForeground(QColor(AppConfig::ColorOther));
        itemStatus->setText(AppData::AlarmTypes.last());
    }
}

void DeviceHelper::setColor(QTableWidgetItem *itemValue, QTableWidgetItem *itemStatus,
                            float value, float max, float min, const QString &alarmType)
{
    //这里还有个未处理的,是否需要对初始值进行判断报警
    if (value == 0) {
        itemValue->setForeground(QColor(AppConfig::ColorZero));
        itemStatus->setForeground(QColor(AppConfig::ColorZero));
    } else if (value > max) {
        itemValue->setForeground(QColor(AppConfig::ColorUpper));
        itemStatus->setForeground(QColor(AppConfig::ColorUpper));
    } else if (value < min) {
        itemValue->setForeground(QColor(AppConfig::ColorLimit));
        itemStatus->setForeground(QColor(AppConfig::ColorLimit));
    } else {
        itemValue->setForeground(QColor(AppConfig::ColorNormal));
        itemStatus->setForeground(QColor(AppConfig::ColorNormal));
    }

    itemValue->setText(QString::number(value));
}

void DeviceHelper::playAlarm(const QString &soundName, bool stop)
{
    if (AppConfig::PlayAlarmSound) {
        playSound(soundName, AppConfig::PlayCount, stop);
    }
}

void DeviceHelper::playSound(const QString &soundName, int playCount, bool stop)
{
    //名称为空或者设置无则不播放
    if (soundName.isEmpty() || soundName.contains("无")) {
        return;
    }

    //设置了先停止
    if (stop) {
        stopSound();
    }

    //重新设置文件全路径
    QString fileName = QString("%1/sound/%2").arg(QUIHelper::appPath()).arg(soundName);
    PlayWav::play(fileName, playCount);
}

void DeviceHelper::stopSound()
{
    PlayWav::stop();
}
