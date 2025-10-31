#include "frmmain.h"
#include "ui_frmmain.h"
#include "quihelper.h"

#include "frmlogout.h"
#include "frmview.h"
#include "frmdata.h"
#include "frmconfig.h"
#include "frmabout.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
    this->initText();
    this->initNav();
    this->initIcon();
    QUIHelper::setFormInCenter(this);
    on_btnMenu_Max_clicked();
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::closeEvent(QCloseEvent *e)
{
    //没有自动登录以及当前非重启状态
    if (!AppConfig::AutoLogin && !AppData::IsReboot) {
        frmLogout::Instance()->show();
        e->ignore();
    } else {
        if (!AppData::IsReboot) {
            AppEvent::Instance()->slot_exitAll();
        }

        //如果不采用暴力退出可能会打印出现 QThread: Destroyed while thread is still running
        //如果在关闭的时候出现崩溃现象,建议启用下面这行代码
        exit(0);
    }
}

bool frmMain::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (watched == ui->labLogoImage) {
            frmAbout::Instance()->show();
        }
    }
    else if (event->type() == QEvent::MouseButtonDblClick)
    {
        //双击标题栏最大化
        if (watched == ui->widgetTop) {
            on_btnMenu_Max_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmMain::initForm()
{
    ui->widgetMain->setProperty("form", true);
    ui->widgetTop->setProperty("form", "title");
    ui->widgetTop->setFixedHeight(AppData::TopHeight);
    ui->bottomWidget->setFixedHeight(AppData::BottomHeight);

    this->setProperty("form", true);
    this->setProperty("canMove", true);
    //设置无边框窗体
    this->setWindowFlags(Qt::FramelessWindowHint);
    //设置窗体居中参照窗体
    QUIHelper::centerBaseForm = this;

    ui->widgetTop->installEventFilter(this);
    ui->labLogoImage->installEventFilter(this);

    //设置右上角菜单,图形字体
    //如果发现部分操作系统不显示对应的图标可以自行更改成其他图标
    //QUIConfig::IconNormal = 0xf067;
    //QUIConfig::IconMax = 0xf067;
    IconHelper::setIcon(ui->btnMenu_Min, QUIConfig::IconMin);
    IconHelper::setIcon(ui->btnMenu_Max, QUIConfig::IconNormal);
    IconHelper::setIcon(ui->btnMenu_Close, QUIConfig::IconClose);

    //设置底部状态栏
    ui->bottomWidget->setVersion(AppData::Version);
    ui->bottomWidget->setCurrentUserName(AppData::CurrentUserName);
    ui->bottomWidget->setCurrentUserType(AppData::CurrentUserType);
    ui->bottomWidget->setFontName(QUIConfig::FontName);
    int fontSize = (QUIConfig::FontSize > 12 ? QUIConfig::FontSize - 1 : QUIConfig::FontSize);
    ui->bottomWidget->setFontSize(fontSize);
    ui->bottomWidget->setLineFixedWidth(true);
    ui->bottomWidget->start();
}

void frmMain::initText()
{
    //从配置文件读取logo名称
    ui->labTitleCn->setText(AppConfig::TitleCn);
    ui->labTitleEn->setText(AppConfig::TitleEn);
    this->setWindowTitle(ui->labTitleCn->text());

    //设置标题+版本+版权
    ui->bottomWidget->setCopyright(AppConfig::Copyright);
    ui->bottomWidget->setTitle(ui->labTitleCn->text());
}

void frmMain::initNav()
{
    //加载视图模块-设备监控+地图监控+数据监控+曲线监控
    frmView *view = new frmView;
    ui->stackedWidget->addWidget(view);

    //加载数据查询模块
    frmData *data = new frmData;
    ui->stackedWidget->addWidget(data);

    //加载系统设置模块
    frmConfig *config = new frmConfig;
    ui->stackedWidget->addWidget(config);

    //关联样式和文字改变信号自动重新设置图标和标题等
    connect(AppEvent::Instance(), SIGNAL(changeText()), this, SLOT(initText()));
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), data, SLOT(initIcon()));
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), view, SLOT(initTree()));
    connect(this, SIGNAL(setIndex(int)), view, SLOT(setIndex(int)));

    icons << 0xf200 << 0xea00 << 0xe695 << 0xe60a << 0xe67b << 0xf002 << 0xf085;
    btns << ui->btnViewMain << ui->btnViewDevice << ui->btnViewMap << ui->btnViewData << ui->btnViewPlot << ui->btnData << ui->btnConfig;
    QList<QString> texts;
    texts << "可视化" << "设备监控" << "地图监控" << "数据监控" << "曲线监控" << "数据查询" << "系统设置";

    for (int i = 0; i < btns.count(); i++) {
        QAbstractButton *btn = btns.at(i);
        btn->setIconSize(QSize(25, 20));
        btn->setFixedWidth(115);
        btn->setCheckable(true);
        btn->setText(texts.at(i));
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));

        if (btn->objectName() == AppConfig::LastFormMain) {
            btn->click();
        }
    }
}

void frmMain::initIcon()
{
    //如果logo图片不存在则隐藏变量,否则显示logo图片
    ui->labLogoImage->setAlignment(Qt::AlignCenter);
    QPixmap pixLogo(QUIHelper::appPath() + "/" + AppConfig::LogoImage);
    if (pixLogo.isNull()) {
        //ui->labLogoImage->setVisible(false);
        //设置图形字体作为logo
        int size = 60;
        ui->labLogoImage->setFixedWidth(size + 20);
        IconHelper::setIcon(ui->labLogoImage, 0xe6f2, size);
    } else {
        int size = 150;
        ui->labLogoImage->setFixedWidth(size + 20);
        pixLogo = pixLogo.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labLogoImage->setPixmap(pixLogo);
    }

    //从图形字体库中设置图标
    int count = btns.count();
    for (int i = 0; i < count; ++i) {
        IconHelper::setPixmap(btns.at(i), QUIConfig::TextColor, icons.at(i), 20, 20, 20);
    }
}

void frmMain::buttonClicked()
{
    QAbstractButton *btn = (QAbstractButton *)sender();
    if (!AppFun::checkPermission(btn->text())) {
        btn->setChecked(false);
        return;
    }

    if (btn == ui->btnViewDevice) {
        emit setIndex(0);
        ui->stackedWidget->setCurrentIndex(0);
    } else if (btn == ui->btnViewMap) {
        emit setIndex(1);
        ui->stackedWidget->setCurrentIndex(0);
    } else if (btn == ui->btnViewData) {
        emit setIndex(2);
        ui->stackedWidget->setCurrentIndex(0);
    } else if (btn == ui->btnViewPlot) {
        emit setIndex(3);
        ui->stackedWidget->setCurrentIndex(0);
    } else if (btn == ui->btnViewMain) {
        emit setIndex(4);
        ui->stackedWidget->setCurrentIndex(0);
    } else if (btn == ui->btnData) {
        ui->stackedWidget->setCurrentIndex(1);
    } else if (btn == ui->btnConfig) {
        ui->stackedWidget->setCurrentIndex(2);
    }

    //取消其他按钮选中
    foreach (QAbstractButton *b, btns) {
        b->setChecked(b == btn);
    }

    //保存最后的窗体索引
    AppConfig::LastFormMain = btn->objectName();
    AppConfig::writeConfig();
}

void frmMain::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}

void frmMain::on_btnMenu_Max_clicked()
{
    static bool max = false;
    static QRect location = this->geometry();

    if (max) {
        this->setGeometry(location);
    } else {
        location = this->geometry();
        this->setGeometry(QUIHelper::getScreenRect());
    }

    IconHelper::setIcon(ui->btnMenu_Max, max ? QUIConfig::IconNormal : QUIConfig::IconMax);
    this->setProperty("canMove", max);
    max = !max;

    //最大化以后有个BUG,悬停样式没有取消掉,需要主动模拟鼠标动一下
    if (max) {
        QEvent event(QEvent::Leave);
        QApplication::sendEvent(ui->btnMenu_Max, &event);
    }
}

void frmMain::on_btnMenu_Close_clicked()
{
    this->close();
}
