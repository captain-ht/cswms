#include "frmviewdevice.h"
#include "ui_frmviewdevice.h"
#include "quihelper.h"
#include "devicehelper.h"
#include "frmdevicenode.h"
#include "frmdevicenode2.h"

frmViewDevice::frmViewDevice(QWidget *parent) : QWidget(parent), ui(new Ui::frmViewDevice)
{
    ui->setupUi(this);
}

frmViewDevice::~frmViewDevice()
{
    delete ui;
}

PanelWidget *frmViewDevice::getPanleWidget()
{
    return ui->panelWidget;
}

void frmViewDevice::itemClicked(const QString &text)
{
    if (AppConfig::PanelMode == 0) {
        static frmDeviceNode *selectDevice = 0;

        //恢复上一次选中的
        if (selectDevice != 0) {
            selectDevice->setSelect(false);
        }

        QList<frmDeviceNode *> devices = DeviceHelper::getDevices();
        foreach (frmDeviceNode *device, devices) {
            if (device->getNodeName() == text) {
                if (device->isEnabled()) {
                    selectDevice = device;
                    device->setSelect(true);
                    break;
                }
            }
        }
    } else if (AppConfig::PanelMode == 1) {
        static frmDeviceNode2 *selectDevice = 0;

        //恢复上一次选中的
        if (selectDevice != 0) {
            selectDevice->setSelect(false);
        }

        QList<frmDeviceNode2 *> devices = DeviceHelper::getDevices2();
        foreach (frmDeviceNode2 *device, devices) {
            if (device->getNodeName() == text) {
                if (device->isEnabled()) {
                    selectDevice = device;
                    device->setSelect(true);
                    break;
                }
            }
        }
    }
}
