#include "frmdevicepannel.h"
#include "ui_frmdevicepannel.h"
#include "quihelper.h"
#include "dbquery.h"
#include "deviceserver.h"
#include "frmdevicecontrol.h"

frmDevicePanel::frmDevicePanel(QWidget *parent) : QWidget(parent), ui(new Ui::frmDevicePanel)
{
    ui->setupUi(this);
    this->initForm();
}

frmDevicePanel::~frmDevicePanel()
{
    delete ui;
}

bool frmDevicePanel::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (!online) {
            //双击重连当前端口下的所有设备

        } else {
            if (AppData::CurrentUserType.contains("管理员")) {
                //QString positionID = this->property("positionID").toString();
                //frmDeviceControl::Instance()->setPositionID(positionID);
                //frmDeviceControl::Instance()->show();
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

QString frmDevicePanel::getPortName()
{
    return this->portName;
}

QString frmDevicePanel::getDeviceName()
{
    return this->deviceName;
}

quint8 frmDevicePanel::getDeviceAddr()
{
    return this->deviceAddr;
}

QString frmDevicePanel::getDeviceType()
{
    return this->deviceType;
}

int frmDevicePanel::getNodeNumber()
{
    return this->nodeNumber;
}

void frmDevicePanel::initForm()
{
    this->installEventFilter(this);

    //取消掉重合的边框使得看起来就一条边框
    QString qss = "QFrame#gboxDeviceTitle{border-bottom-left-radius:0px;border-bottom-right-radius:0px;}"
                  "QFrame#gboxDevicePanel{border-top-width:0px;border-top-left-radius:0px;border-top-right-radius:0px;}"
                  "QLabel:disabled{background:none;}";
    this->setStyleSheet(qss);

    portName = "通信端口-1";
    deviceName = "1#调度室控制器";
    deviceAddr = 1;
    deviceType = "FC-1003-4";
    nodeNumber = 4;

    online = false;
    alarm = false;

    red = false;
    timerAlarm = new QTimer(this);
    connect(timerAlarm, SIGNAL(timeout()), this, SLOT(changeColor()));
    timerAlarm->setInterval(500);

    QFont font;
    font.setPixelSize(QUIConfig::FontSize + 5);
    ui->labDeviceName->setFont(font);
    ui->labDeviceType->setFont(font);

    QUIStyle::setLabStyle(ui->labDeviceName, 1);
    QUIStyle::setLabStyle(ui->labDeviceType, 3);
}

void frmDevicePanel::changeColor()
{

}

void frmDevicePanel::changeStatus(quint8 type, bool alarm)
{
    //报警则面板颜色切换
    if (alarm) {
        timerAlarm->start();
        red = false;
        changeColor();
    } else {
        timerAlarm->stop();
        red = true;
        changeColor();
    }
}

void frmDevicePanel::setPortName(const QString &portName)
{
    this->portName = portName;
}

void frmDevicePanel::setDeviceName(const QString &deviceName)
{
    this->deviceName = deviceName;
    ui->labDeviceName->setText(deviceName);
}

void frmDevicePanel::setDeviceAddr(quint8 deviceAddr)
{
    this->deviceAddr = deviceAddr;
}

void frmDevicePanel::setDeviceType(const QString &deviceType)
{
    this->deviceType = deviceType;
    ui->labDeviceType->setText(deviceType);
}

void frmDevicePanel::setNodeNumber(int nodeNumber)
{
    this->nodeNumber = nodeNumber;

    //位号变量拉伸占2格
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(1);
    sizePolicy1.setVerticalStretch(0);
    //数据值变量拉伸占1格
    QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(2);
    sizePolicy2.setVerticalStretch(0);

    //根据变量数量自动生成位号+值+符号变量
    if (nodeNumber == 1) {
        QLabel *labId1 = new QLabel;
        labId1->setText("AT-400001");
        QLabel *labValue1 = new QLabel;
        labValue1->setText("18.9");
        QLabel *labSign1 = new QLabel;
        labSign1->setText("PPM");

        ui->gridLayout->addWidget(labId1, nodeNumber, 0);
        ui->gridLayout->addWidget(labValue1, nodeNumber, 1);
        ui->gridLayout->addWidget(labSign1, nodeNumber, 2);
    } else {
        int index=1;
        int count = nodeNumber / 2;
        for (int i = 0; i < count; i++) {
            QLabel *labId1 = new QLabel;
            labId1->setText(QString("AT-40000%1").arg(index));
            labId1->setAlignment(Qt::AlignCenter);
            labId1->setFrameShape(QLabel::Box);
            labId1->setSizePolicy(sizePolicy2);

            QLabel *labValue1 = new QLabel;
            labValue1->setText("18.9");
            labValue1->setAlignment(Qt::AlignCenter);
            labValue1->setFrameShape(QLabel::Box);
            labValue1->setSizePolicy(sizePolicy1);

            QLabel *labSign1 = new QLabel;
            labSign1->setText("PPM");
            labSign1->setFixedWidth(45);
            labSign1->setFrameShape(QLabel::Box);

            ui->gridLayout->addWidget(labId1, i, 0);
            ui->gridLayout->addWidget(labValue1, i, 1);
            ui->gridLayout->addWidget(labSign1, i, 2);
            index++;

            QLabel *labId2 = new QLabel;
            labId2->setText(QString("AT-40000%1").arg(index));
            labId2->setAlignment(Qt::AlignCenter);
            labId2->setFrameShape(QLabel::Box);
            labId2->setSizePolicy(sizePolicy2);

            QLabel *labValue2 = new QLabel;
            labValue2->setText("18.9");
            labValue2->setAlignment(Qt::AlignCenter);
            labValue2->setFrameShape(QLabel::Box);
            labValue2->setSizePolicy(sizePolicy1);

            QLabel *labSign2 = new QLabel;
            labSign2->setText("PPM");
            labSign2->setFixedWidth(45);
            labSign2->setFrameShape(QLabel::Box);

            ui->gridLayout->addWidget(labId2, i, 3);
            ui->gridLayout->addWidget(labValue2, i, 4);
            ui->gridLayout->addWidget(labSign2, i, 5);
            index++;
        }
    }
}

void frmDevicePanel::setOnline(bool online)
{
    this->online = online;
}

void frmDevicePanel::setAlarm(bool alarm)
{
    this->alarm = alarm;
}
