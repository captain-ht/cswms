#include "frmconfigmap.h"
#include "ui_frmconfigmap.h"
#include "quihelper.h"
#include "appfun.h"
#include "devicemap.h"
#include "devicehelper.h"

frmConfigMap::frmConfigMap(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigMap)
{
    ui->setupUi(this);
    this->initForm();
}

frmConfigMap::~frmConfigMap()
{
    delete ui;
}

void frmConfigMap::initForm()
{
    ui->widgetRight->setFixedWidth(AppData::RightWidth);

    DeviceHelper::initDeviceMap(ui->listWidget);
    if (ui->listWidget->count() > 0) {
        ui->listWidget->setCurrentRow(0);
        on_listWidget_pressed();
    }
}

void frmConfigMap::on_listWidget_pressed()
{
    int row = ui->listWidget->currentRow();
    if (row < 0) {
        return;
    }

    DeviceHelper::initDeviceMapCurrent(ui->labMap, AppData::MapNames.at(row));
}

void frmConfigMap::on_btnInput_clicked()
{
    QString defaultDir = AppFun::getDefaultDir();
    QString fileName = QUIHelper::saveFileName("Images (*.png *.bmp *.jpg *.gif *.jpeg)", defaultDir, "");

    if (fileName.length() > 0) {
        QFileInfo f(fileName);
        QString imageName = f.fileName();

        //判断文件是否过大,暂定1MB,过大则弹出提示并不加载
        if (f.size() > 1 * 1024 * 1024) {
            QUIHelper::showMessageBoxError("图片文件过大,请控制在1MB以内,谢谢!", 3);
            return;
        }

        //如果导入的图片有乱码则自动重命名
        if (imageName.contains("?")) {
            imageName = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss.") + f.suffix();
        }

        QString fullName = QString("%1/%2").arg(AppData::MapPath).arg(imageName);

        //2018-10-18 增加大像素图片过滤,等比例缩放图片
        QImage img(fileName);
        if (img.width() > 1920 || img.height() > 1080) {
            img = img.scaled(AppData::MapWidth, AppData::MapHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            img.save(fullName);
        } else {
            QUIHelper::copyFile(fileName, fullName);
        }

        AppData::MapNames.append(imageName);
        DeviceMap::Instance()->appendMapPix(imageName);
        DeviceHelper::initDeviceMap(ui->listWidget);
        DeviceHelper::initDeviceMap();

        //自动切换到当前导入的地图
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
        on_listWidget_pressed();
        emit changeMap();
    }
}

void frmConfigMap::on_btnDelete_clicked()
{
    int row = ui->listWidget->currentRow();
    if (row < 0) {
        return;
    }

    if (QUIHelper::showMessageBoxQuestion("确定要删除当前选中地图文件吗?") == QMessageBox::Yes) {
        QString imageName = AppData::MapNames.at(row);
        QString fileName = QString("%1/%2").arg(AppData::MapPath).arg(imageName);
        QFile f(fileName);
        f.remove();

        AppData::MapNames.removeOne(imageName);
        DeviceMap::Instance()->removeMapPix(imageName);
        DeviceHelper::initDeviceMap(ui->listWidget);
        DeviceHelper::initDeviceMap();

        //如果还有地图则自动切换到第一张地图,没有则清空显示
        if (ui->listWidget->count() > 0) {
            ui->listWidget->setCurrentRow(0);
            on_listWidget_pressed();
        } else {
            ui->labMap->clear();
        }

        emit changeMap();
    }
}
