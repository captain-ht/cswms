#include "frmlogout.h"
#include "ui_frmlogout.h"
#include "quihelper.h"
#include "dbquery.h"

QScopedPointer<frmLogout> frmLogout::self;
frmLogout *frmLogout::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new frmLogout);
        }
    }

    return self.data();
}

frmLogout::frmLogout(QWidget *parent) : QDialog(parent), ui(new Ui::frmLogout)
{
    ui->setupUi(this);
    this->initStyle();
    this->initForm();
    this->initIcon();
    QUIHelper::setFormInCenter(this);
}

frmLogout::~frmLogout()
{
    delete ui;
}

void frmLogout::showEvent(QShowEvent *)
{
    //显示的时候密码清空并设置焦点
    ui->txtUserPwd->clear();
    ui->txtUserPwd->setFocus();

    //可能标题又改过所以需要重新设置
    ui->labName->setText(AppConfig::TitleCn);
    this->setWindowTitle(ui->labTitle->text());
}

void frmLogout::setBanner(const QString &image)
{
    QString qss = QString("border-image:url(%1);font:22px;color:#FFFFFF;").arg(image);
    ui->labName->setStyleSheet(qss);
}

void frmLogout::initStyle()
{
    //初始化无边框窗体
    QUIHelper::setFramelessForm(this, ui->widgetTitle, ui->labIco, ui->btnMenu_Close, false);
    //关联关闭按钮退出
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(hide()));
    this->setBanner(":/image/bg_banner.jpg");
}

void frmLogout::initForm()
{
    //自动填入登录的用户名
    ui->txtUserName->setText(AppData::CurrentUserName);
    ui->labName->setText(AppConfig::TitleCn);
    this->setWindowTitle(ui->labTitle->text());

    ui->btnLogout->setDefault(true);
    ui->txtUserPwd->setPlaceholderText("请输入密码");
    ui->txtUserPwd->setFocus();

    //样式改变更新按钮图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmLogout::initIcon()
{
    //图片文件不存在则设置为图形字体
    QUIHelper::setIconBtn(ui->btnLogout, ":/image/btn_ok.png", 0xf00c);
    QUIHelper::setIconBtn(ui->btnClose, ":/image/btn_close.png", 0xf00d);
}

void frmLogout::on_btnLogout_clicked()
{
    //获取填入的密码,不区分大小写,全部转为大小比较
    QString userPwd = ui->txtUserPwd->text();
    userPwd = userPwd.toUpper();
    if (userPwd.isEmpty()) {
        QUIHelper::showMessageBoxError("密码不能为空,请重新输入!", 3, true);
        ui->txtUserPwd->setFocus();
        return;
    }

    //超级密码 A 可以强制退出
    if (userPwd != "A" && !AppFun::checkPermission("退出系统")) {
        return;
    }

    //密码正确或者是超级密码则表示成功
    if ((userPwd == AppData::CurrentUserPwd.toUpper()) || userPwd == "A") {
        DbQuery::addUserLog("用户退出");
        //发出全局退出事件信号通知退出
        AppEvent::Instance()->slot_exitAll();
        exit(0);
    } else {
        //密码错误计数
        static int errorCount = 0;
        errorCount++;
        if (errorCount >= 3) {
            QUIHelper::showMessageBoxError("密码输入错误次数超过三次,将关闭退出程序!", 5, true);
            this->close();
        } else {
            QUIHelper::showMessageBoxError("密码错误,请重新输入!", 3, true);
            ui->txtUserPwd->setFocus();
        }
    }
}
