#include "frmconfig.h"
#include "ui_frmconfig.h"
#include "quihelper.h"

#include "frmconfigsystem.h"
#include "frmconfigport.h"
#include "frmconfigdevice.h"
#include "frmconfignode.h"
#include "frmconfiglink.h"
#include "frmconfigtype.h"
#include "frmconfiguser.h"
#include "frmconfigmap.h"
#include "frmconfigposition.h"
#include "frmconfigscada.h"
#include "frmconfigdebug.h"
#include "frmconfigcrane.h"

frmConfig::frmConfig(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfig)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->initNav();
    this->initIcon();
}

frmConfig::~frmConfig()
{
    delete ui;
}

void frmConfig::initForm()
{
    ui->widgetLeft->setProperty("flag", "btn");
    ui->widgetLeft->setFixedWidth(AppData::LeftWidth);
#ifndef scada
    ui->btnConfigXml->setVisible(false);
#endif
}

void frmConfig::initWidget()
{
    frmConfigSystem *configSystem = new frmConfigSystem;
    ui->stackedWidget->addWidget(configSystem);

    frmConfigCrane *configCrane = new frmConfigCrane;
    ui->stackedWidget->addWidget(configCrane);

    frmConfigPort *configPort = new frmConfigPort;
    ui->stackedWidget->addWidget(configPort);

    frmConfigDevice *configDevice = new frmConfigDevice;
    ui->stackedWidget->addWidget(configDevice);

    frmConfigNode *configNode = new frmConfigNode;
    ui->stackedWidget->addWidget(configNode);

    frmConfigLink *configLink = new frmConfigLink;
    ui->stackedWidget->addWidget(configLink);

    frmConfigType *configType = new frmConfigType;
    ui->stackedWidget->addWidget(configType);

    frmConfigUser *configUser = new frmConfigUser;
    ui->stackedWidget->addWidget(configUser);

    frmConfigMap *configMap = new frmConfigMap;
    ui->stackedWidget->addWidget(configMap);

    frmConfigPosition *configPosition = new frmConfigPosition;
    ui->stackedWidget->addWidget(configPosition);

    frmConfigScada *configScada = new frmConfigScada;
    ui->stackedWidget->addWidget(configScada);

    frmConfigDebug *configDebug = new frmConfigDebug;
    ui->stackedWidget->addWidget(configDebug);

    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), configCrane, SLOT(changeStyle()));
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), configPort, SLOT(changeStyle()));
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), configDevice, SLOT(changeStyle()));
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), configNode, SLOT(changeStyle()));
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), configLink, SLOT(changeStyle()));

    connect(configMap, SIGNAL(changeMap()), configPosition, SLOT(initList()));
    connect(configMap, SIGNAL(changeMap()), configNode, SLOT(nodeImageChanged()));
    connect(configPort, SIGNAL(portNameChanged()), configDevice, SLOT(portNameChanged()));
    connect(configDevice, SIGNAL(deviceNameChanged()), configNode, SLOT(deviceNameChanged()));
    connect(configNode, SIGNAL(positionIDChanged()), configLink, SLOT(positionIDChanged()));

    connect(configType, SIGNAL(deviceTypeChanged()), configDevice, SLOT(deviceTypeChanged()));
    connect(configType, SIGNAL(nodeTypeChanged()), configNode, SLOT(nodeTypeChanged()));
    connect(configType, SIGNAL(nodeClassChanged()), configNode, SLOT(nodeClassChanged()));
    connect(configType, SIGNAL(nodeSignChanged()), configNode, SLOT(nodeSignChanged()));
}

void frmConfig::initNav()
{
    icons << 0xe60d << 0xf018 << 0xe9c1 << 0xea00 << 0xe603 << 0xe6e1
          << 0xe6c4 << 0xea59 << 0xe6a3 << 0xe702 << 0xea3b << 0xf188;
    btns << ui->btnConfigSystem << ui->btnConfigCrane << ui->btnConfigPort << ui->btnConfigDevice << ui->btnConfigNode << ui->btnConfigLink
         << ui->btnConfigType << ui->btnConfigUser << ui->btnConfigMap << ui->btnConfigPosition << ui->btnConfigXml << ui->btnConfigDebug;
    QList<QString> texts;
    texts << "基 本 设 置" << "起重机管理" << "端 口 管 理" << "控制器管理" << "标 签 管 理" << "报 警 联 动"
          << "类 型 设 置" << "用 户 管 理" << "地 图 管 理" << "位 置 调 整" << "组 态 设 计" << "设 备 调 试";

    for (int i = 0; i < btns.count(); i++) {
        QAbstractButton *btn = btns.at(i);
        btn->setIconSize(QSize(30, 20));
        btn->setCheckable(true);
        btn->setText(texts.at(i) + "   ");
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));

        if (btn->objectName() == AppConfig::LastFormConfig) {
            btn->click();
        }
    }
}

void frmConfig::initIcon()
{
    //从图形字体库中设置图标
    int count = btns.count();
    for (int i = 0; i < count; ++i) {
        IconHelper::setPixmap(btns.at(i), QUIConfig::TextColor, icons.at(i), 20, 30, 20);
    }
}

void frmConfig::buttonClicked()
{
    //切换到当前窗体
    QAbstractButton *btn = (QAbstractButton *)sender();
    int index = btns.indexOf(btn);
    ui->stackedWidget->setCurrentIndex(index);

    //取消其他按钮选中
    foreach (QAbstractButton *b, btns) {
        b->setChecked(b == btn);
    }

    //保存最后的窗体索引
    AppConfig::LastFormConfig = btn->objectName();
    AppConfig::writeConfig();
}
