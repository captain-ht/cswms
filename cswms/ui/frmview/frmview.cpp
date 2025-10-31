#include "frmview.h"
#include "ui_frmview.h"
#include "quihelper.h"
#include "dbquery.h"

#include "deviceserver.h"
#include "devicehelper.h"
#include "alarmlink.h"
#include "deviceserver.h"

#include "frmviewdevice.h"
#include "frmviewmap.h"
#include "frmviewdata.h"
#include "frmviewplot.h"
#include "frmviewmain.h"
#include "frmdevicecontrol.h"

frmView::frmView(QWidget *parent) : QWidget(parent), ui(new Ui::frmView)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->initControl();
    QTimer::singleShot(1000, this, SLOT(initServer()));
}

frmView::~frmView()
{
    delete ui;
}

void frmView::initForm()
{
    ui->navTitleTable->setText("警情消息");
    ui->navTitleTable->setLeftIcon(0xf108);
    ui->navTitleTable->setRightIcon4(0xf1f7);
    ui->navTitleTable->setRightIcon5(0xf1f8);
    ui->navTitleTree->setText("设备列表");
    ui->navTitleTree->setLeftIcon(0xf0e8);
    ui->navTitleTree->setRightIcon5(0xf021);
    ui->navTitleList->setText("地图列表");
    ui->navTitleList->setLeftIcon(0xf0e8);
    ui->navTitleList->setRightIcon5(0xf021);
    ui->widgetLeft->setFixedWidth(AppData::LeftWidth);
    ui->widgetRight->setFixedWidth(AppData::RightWidth);

    connect(ui->navTitleTable, SIGNAL(mousePressed(int)), this, SLOT(mousePressed(int)));
    connect(AppEvent::Instance(), SIGNAL(doubleClicked(DeviceButton *)), this, SLOT(doubleClicked(DeviceButton *)));
}

void frmView::initWidget()
{
    frmViewDevice *viewDevice = new frmViewDevice;
    ui->stackedWidget->addWidget(viewDevice);

    frmViewMap *viewMap = new frmViewMap;
    ui->stackedWidget->addWidget(viewMap);

    frmViewData *viewData = new frmViewData;
    ui->stackedWidget->addWidget(viewData);

    frmViewPlot *viewPlot = new frmViewPlot;
    ui->stackedWidget->addWidget(viewPlot);

    frmViewMain *viewMain = new frmViewMain;
    ui->stackedWidget->addWidget(viewMain);

    connect(this, SIGNAL(itemClicked(QString)), viewDevice, SLOT(itemClicked(QString)));
    connect(DeviceServer::Instance(), SIGNAL(receiveAlarm(QString, QString, quint8)),
            this, SLOT(receiveAlarm(QString, QString, quint8)));

    DeviceHelper::setTableData(viewData->getTable());
    DeviceHelper::setTableWidget(ui->tableWidget);
    DeviceHelper::setTreeWidget(ui->treeWidget);
    DeviceHelper::setListWidget(ui->listWidget);
    DeviceHelper::setPanelWidget(viewDevice->getPanleWidget());
    DeviceHelper::setLabel(viewMap->getLabel());
}

void frmView::initTree()
{
    DeviceHelper::initDeviceTree();
    DeviceHelper::initDeviceStyle();
}

void frmView::initControl()
{
    DeviceHelper::initDeviceData();
    DeviceHelper::initDeviceTable();
    DeviceHelper::initDeviceTree();
    DeviceHelper::initDeviceMap();
    DeviceHelper::initDevicePanel();
    DeviceHelper::initDeviceButton();

    if (ui->listWidget->count() > 0) {
        ui->listWidget->setCurrentRow(0);
        on_listWidget_pressed();
    }
}

void frmView::initServer()
{
    //启动设备解析
    DeviceServer::Instance()->init();
    DeviceServer::Instance()->start();
}

void frmView::mousePressed(int index)
{
    if (index == 5) {
        //改变声音图标并停止报警声音
        ui->navTitleTable->setRightIcon4(0xf1f7);
        DeviceHelper::stopSound();
        AlarmLink::Instance()->reset();
    } else if (index == 6) {
        //清空临时消息
        DeviceHelper::clearMsg();
    }
}

void frmView::doubleClicked(DeviceButton *btn)
{
    if (AppData::CurrentUserType.contains("管理员")) {
        if (btn->getButtonColor() == DeviceButton::ButtonColor_Black) {
            //双击重连当前端口下的所有设备
            QString portName = DbQuery::getPortName(btn->property("deviceName").toString());
            DeviceServer::Instance()->readValue(portName, 255, true);
        } else {
            //弹出详细信息
            QString positionID = btn->property("positionID").toString();
            frmDeviceControl::Instance()->setPositionID(positionID);
            frmDeviceControl::Instance()->show();
        }
    }
}

void frmView::on_listWidget_pressed()
{
    int row = ui->listWidget->currentRow();
    if (row < 0) {
        return;
    }

    DeviceHelper::initDeviceMapCurrent(AppData::MapNames.at(row));
}

void frmView::on_treeWidget_itemPressed(QTreeWidgetItem *item, int column)
{
    if (item->parent() != 0) {
        emit itemClicked(item->text(column));
    }
}

void frmView::setIndex(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
    bool visible = (index == 0 || index == 2);
    ui->widgetLeft->setVisible(!(index == 3 || index == 4));
    ui->widgetRight->setVisible(!(index == 3 || index == 4));
    ui->navTitleTree->setVisible(visible);
    ui->treeWidget->setVisible(visible);
    ui->navTitleList->setVisible(!visible);
    ui->listWidget->setVisible(!visible);
}

void frmView::receiveAlarm(const QString &deviceName, const QString &positionID, quint8 alarmType)
{
    if (alarmType == 0 || alarmType == 2 || alarmType == 5) {
        ui->navTitleTable->setRightIcon4(0xf0a2);
    } else {
        ui->navTitleTable->setRightIcon4(0xf1f7);
    }
}

void frmView::on_btnClearSound_clicked()
{
    mousePressed(5);
}

void frmView::on_btnClearMsg_clicked()
{
    mousePressed(6);
}
