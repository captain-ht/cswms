#include "frmconfigdebug.h"
#include "ui_frmconfigdebug.h"
#include "quihelper.h"
#include "deviceserver.h"
#include "dbreceive.h"

frmConfigDebug::frmConfigDebug(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigDebug)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmConfigDebug::~frmConfigDebug()
{
    delete ui;
}

void frmConfigDebug::initForm()
{
    QFont font;
    font.setPixelSize(QUIConfig::FontSize + 5);
    ui->txtMain->setFont(font);
    ui->frameRight->setFixedWidth(AppData::RightWidth);

    ui->cboxPortName->addItem("所有端口");
    ui->cboxPortName->addItems(DbData::PortInfo_PortName);

    QUIStyle::setLabStyle(ui->labPortName, 1);
    QUIStyle::setLabStyle(ui->labDeviceName, 1);
    QUIStyle::setLabStyle(ui->labKeyValue1, 3);
    QUIStyle::setLabStyle(ui->labKeyValue2, 3);

#if (QT_VERSION >= QT_VERSION_CHECK(5,2,0))
    ui->txtKeyValue1->setPlaceholderText("支持多个关键字,用 | 隔开");
    ui->txtKeyValue2->setPlaceholderText("支持多个关键字,用 | 隔开");
#endif

    //颜色下拉框
    QStringList colorList = QColor::colorNames();
    foreach (QString strColor, colorList) {
        QPixmap pix(ui->cboxKeyColor1->iconSize());
        pix.fill(strColor);
        ui->cboxKeyColor1->addItem(QIcon(pix), strColor);
        ui->cboxKeyColor2->addItem(QIcon(pix), strColor);
    }

    //绑定设备采集服务信号槽
    connect(DeviceServer::Instance(), SIGNAL(sendData(QString, quint8, QByteArray)),
            this, SLOT(sendData(QString, quint8, QByteArray)));
    connect(DeviceServer::Instance(), SIGNAL(receiveData(QString, quint8, QByteArray)),
            this, SLOT(receiveData(QString, quint8, QByteArray)));
    connect(DeviceServer::Instance(), SIGNAL(receiveInfo(QString, quint8, QString)),
            this, SLOT(receiveInfo(QString, quint8, QString)));
    connect(DeviceServer::Instance(), SIGNAL(receiveError(QString, quint8, QString)),
            this, SLOT(receiveError(QString, quint8, QString)));
}

void frmConfigDebug::initConfig()
{
    ui->cboxPortName->setCurrentIndex(AppConfig::PortNameIndex);
    connect(ui->cboxPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxDeviceName->setCurrentIndex(AppConfig::DeviceNameIndex);
    connect(ui->cboxDeviceName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    keys1 = AppConfig::KeyValue1.split("|");
    ui->txtKeyValue1->setText(AppConfig::KeyValue1);
    connect(ui->txtKeyValue1, SIGNAL(textChanged()), this, SLOT(saveConfig()));

    ui->cboxKeyColor1->setCurrentIndex(ui->cboxKeyColor1->findText(AppConfig::KeyColor1));
    connect(ui->cboxKeyColor1, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    keys2 = AppConfig::KeyValue2.split("|");
    ui->txtKeyValue2->setText(AppConfig::KeyValue2);
    connect(ui->txtKeyValue2, SIGNAL(textChanged()), this, SLOT(saveConfig()));

    ui->cboxKeyColor2->setCurrentIndex(ui->cboxKeyColor2->findText(AppConfig::KeyColor2));
    connect(ui->cboxKeyColor2, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
}

void frmConfigDebug::saveConfig()
{
    AppConfig::PortNameIndex = ui->cboxPortName->currentIndex();
    AppConfig::DeviceNameIndex = ui->cboxDeviceName->currentIndex();
    AppConfig::KeyValue1 = ui->txtKeyValue1->toPlainText().trimmed();
    AppConfig::KeyColor1 = ui->cboxKeyColor1->currentText();
    AppConfig::KeyValue2 = ui->txtKeyValue2->toPlainText().trimmed();
    AppConfig::KeyColor2 = ui->cboxKeyColor2->currentText();

    if (AppConfig::KeyValue1.isEmpty()) {
        AppConfig::KeyValue1 = "|";
    }

    if (AppConfig::KeyValue2.isEmpty()) {
        AppConfig::KeyValue2 = "|";
    }

    AppConfig::writeConfig();

    keys1 = AppConfig::KeyValue1.split("|");
    keys2 = AppConfig::KeyValue2.split("|");
}

void frmConfigDebug::append(int type, const QString &portName, quint8 addr, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 500;

    //执行清空数据命令
    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }

    //执行条数到了清空数据
    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

    //暂停显示
    if (ui->ckPause->isChecked()) {
        return;
    }

    //过滤端口+过滤地址
    if (!ui->ckOther->isChecked()) {
        QString currentPortName = ui->cboxPortName->currentText();
        if (currentPortName != "所有端口") {
            if (currentPortName != portName) {
                return;
            }
        }
        quint8 deviceAddr = ui->cboxDeviceName->itemData(AppConfig::DeviceNameIndex).toInt();
        if (deviceAddr != 255) {
            if (deviceAddr != addr) {
                return;
            }
        }
    }

    //过滤回车换行符
    QString strData = data;
    strData.replace("\r", "");
    strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "发送";
        ui->txtMain->setTextColor(QColor("#009679"));
    } else if (type == 1) {
        strType = "接收";
        ui->txtMain->setTextColor(QColor("#D64D54"));
    } else if (type == 2) {
        strType = "解析";
        ui->txtMain->setTextColor(QColor("#B59481"));
    } else if (type == 3) {
        strType = "错误";
        ui->txtMain->setTextColor(QColor("#A279C5"));
    } else if (type == 4) {
        strType = "解析";
        ui->txtMain->setTextColor(QColor("#047058"));
    } else if (type == 5) {
        strType = "解析";
        ui->txtMain->setTextColor(QColor("#9157C8"));
    } else if (type == 6) {
        strType = "解析";
        ui->txtMain->setTextColor(QColor("#BA5656"));
    }

    //过滤关键字1,设置不同颜色
    foreach (QString key, keys1) {
        if (!key.isEmpty() && data.contains(key)) {
            ui->txtMain->setTextColor(QColor(AppConfig::KeyColor1));
            break;
        }
    }

    //过滤关键字2,设置不同颜色
    foreach (QString key, keys2) {
        if (!key.isEmpty() && data.contains(key)) {
            ui->txtMain->setTextColor(QColor(AppConfig::KeyColor2));
            break;
        }
    }

    QString strAddr = QString("%1").arg(addr, 3, 10, QChar('0'));
    strData = QString("%1 %2[%3] %4: %5").arg(TIMEMS).arg(portName).arg(strAddr).arg(strType).arg(strData);
    ui->txtMain->append(strData);
    currentCount++;

    //输出数据
    if (ui->ckDebug->isChecked()) {
        qDebug() << strData;
    }
}

void frmConfigDebug::sendData(const QString &portName, quint8 addr, const QByteArray &data)
{
    if (!ui->ckSendData->isChecked()) {
        return;
    }

    append(0, portName, addr, QUIHelper::byteArrayToHexStr(data));
}

void frmConfigDebug::receiveData(const QString &portName, quint8 addr, const QByteArray &data)
{
    if (!ui->ckReceiveData->isChecked()) {
        return;
    }

    append(1, portName, addr, QUIHelper::byteArrayToHexStr(data));
}

void frmConfigDebug::receiveInfo(const QString &portName, quint8 addr, const QString &data)
{
    if (!ui->ckReceiveInfo->isChecked()) {
        return;
    }

    append(2, portName, addr, data);
}

void frmConfigDebug::receiveError(const QString &portName, quint8 addr, const QString &data)
{
    append(3, portName, addr, data);
}

void frmConfigDebug::debugDbLocal(const QString &msg)
{
    append(4, "本地数据库", 255, msg);
}

void frmConfigDebug::debugDbNet(const QString &msg)
{
    append(5, "云端数据库", 255, msg);
}

void frmConfigDebug::debugDbReceive(const QString &msg)
{
    append(6, "下载数据库", 255, msg);
}

void frmConfigDebug::on_btnClearData_clicked()
{
    append(0, "", 255, "", true);
}

void frmConfigDebug::on_btnReadValueAll_clicked()
{
    DeviceServer::Instance()->readValueAll();
}

void frmConfigDebug::on_cboxPortName_currentIndexChanged(int)
{
    //重新载入该端口对应的所有设备
    ui->cboxDeviceName->clear();
    ui->cboxDeviceName->addItem("所有设备", 255);

    for (int i = 0; i < DbData::DeviceInfo_Count; i++) {
        if (DbData::DeviceInfo_PortName.at(i) == ui->cboxPortName->currentText()) {
            QString deviceName = DbData::DeviceInfo_DeviceName.at(i);
            quint8 deviceAddr = DbData::DeviceInfo_DeviceAddr.at(i);
            ui->cboxDeviceName->addItem(deviceName, deviceAddr);
        }
    }
}

void frmConfigDebug::on_ckOther_stateChanged(int arg1)
{
    on_btnClearData_clicked();
    if (arg1 != 0) {
        //先解除设备采集服务信号槽
        disconnect(DeviceServer::Instance(), SIGNAL(sendData(QString, quint8, QByteArray)),
                   this, SLOT(sendData(QString, quint8, QByteArray)));
        disconnect(DeviceServer::Instance(), SIGNAL(receiveData(QString, quint8, QByteArray)),
                   this, SLOT(receiveData(QString, quint8, QByteArray)));
        disconnect(DeviceServer::Instance(), SIGNAL(receiveInfo(QString, quint8, QString)),
                   this, SLOT(receiveInfo(QString, quint8, QString)));
        disconnect(DeviceServer::Instance(), SIGNAL(receiveError(QString, quint8, QString)),
                   this, SLOT(receiveError(QString, quint8, QString)));

        //绑定其他信号槽
        connect(DbData::DbLocal, SIGNAL(debug(QString)), this, SLOT(debugDbLocal(QString)));
        connect(DbData::DbNet, SIGNAL(debug(QString)), this, SLOT(debugDbNet(QString)));
        connect(DbReceive::Instance(), SIGNAL(debug(QString)), this, SLOT(debugDbReceive(QString)));
    } else {
        //先绑定设备采集服务信号槽
        connect(DeviceServer::Instance(), SIGNAL(sendData(QString, quint8, QByteArray)),
                this, SLOT(sendData(QString, quint8, QByteArray)));
        connect(DeviceServer::Instance(), SIGNAL(receiveData(QString, quint8, QByteArray)),
                this, SLOT(receiveData(QString, quint8, QByteArray)));
        connect(DeviceServer::Instance(), SIGNAL(receiveInfo(QString, quint8, QString)),
                this, SLOT(receiveInfo(QString, quint8, QString)));
        connect(DeviceServer::Instance(), SIGNAL(receiveError(QString, quint8, QString)),
                this, SLOT(receiveError(QString, quint8, QString)));

        //解除其他信号槽
        disconnect(DbData::DbLocal, SIGNAL(debug(QString)), this, SLOT(debugDbLocal(QString)));
        disconnect(DbData::DbNet, SIGNAL(debug(QString)), this, SLOT(debugDbNet(QString)));
        disconnect(DbReceive::Instance(), SIGNAL(debug(QString)), this, SLOT(debugDbReceive(QString)));
    }
}
