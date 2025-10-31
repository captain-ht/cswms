#include "frmconfigposition.h"
#include "ui_frmconfigposition.h"
#include "quihelper.h"
#include "devicehelper.h"

frmConfigPosition::frmConfigPosition(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigPosition)
{
    ui->setupUi(this);
    this->initForm();
}

frmConfigPosition::~frmConfigPosition()
{
    delete ui;
}

void frmConfigPosition::initForm()
{
    ui->widgetRight->setFixedWidth(AppData::RightWidth);

    DeviceHelper::initDeviceButton(ui->labMap);
    DeviceHelper::initDeviceMap(ui->listWidget);

    if (ui->listWidget->count() > 0) {
        ui->listWidget->setCurrentRow(0);
        on_listWidget_pressed();
    }
}

void frmConfigPosition::initList()
{
    DeviceHelper::initDeviceMap(ui->listWidget);
}

void frmConfigPosition::on_listWidget_pressed()
{
    int row = ui->listWidget->currentRow();
    if (row < 0) {
        return;
    }

    DeviceHelper::initDeviceMapCurrent(ui->labMap, AppData::MapNames.at(row));
}

void frmConfigPosition::on_btnSave_clicked()
{
    DeviceHelper::saveDeviceButtonPosition();
    QUIHelper::showMessageBoxInfo("保存位置信息成功!", 2);
}
