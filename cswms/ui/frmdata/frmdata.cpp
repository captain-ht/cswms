#include "frmdata.h"
#include "ui_frmdata.h"
#include "quihelper.h"

#include "frmdataalarm.h"
#include "frmdatanode.h"
#include "frmdatauser.h"

frmData::frmData(QWidget *parent) : QWidget(parent), ui(new Ui::frmData)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->initNav();
    this->initIcon();
}

frmData::~frmData()
{
    delete ui;
}

void frmData::initForm()
{
    ui->widgetLeft->setProperty("flag", "btn");
    ui->widgetLeft->setFixedWidth(AppData::LeftWidth);
}

void frmData::initWidget()
{
    frmDataAlarm *dataAlarm = new frmDataAlarm;
    ui->stackedWidget->addWidget(dataAlarm);

    frmDataNode *dataNode = new frmDataNode;
    ui->stackedWidget->addWidget(dataNode);

    frmDataUser *dataUser = new frmDataUser;
    ui->stackedWidget->addWidget(dataUser);
}

void frmData::initNav()
{
    icons << 0xea5b << 0xea44 << 0xea59;
    btns << ui->btnDataAlarm << ui->btnDataNode << ui->btnDataUser;
    QList<QString> texts;
    texts << "报 警 记 录" << "运 行 记 录" << "操 作 记 录";

    for (int i = 0; i < btns.count(); i++) {
        QAbstractButton *btn = btns.at(i);
        btn->setIconSize(QSize(30, 20));
        btn->setCheckable(true);
        btn->setText(texts.at(i) + "   ");
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));

        if (btn->objectName() == AppConfig::LastFormData) {
            btn->click();
        }
    }
}

void frmData::initIcon()
{
    //从图形字体库中设置图标
    int count = btns.count();
    for (int i = 0; i < count; ++i) {
        IconHelper::setPixmap(btns.at(i), QUIConfig::TextColor, icons.at(i), 20, 30, 20);
    }
}

void frmData::buttonClicked()
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
    AppConfig::LastFormData = btn->objectName();
    AppConfig::writeConfig();
}
