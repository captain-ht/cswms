#include "frmabout.h"
#include "ui_frmabout.h"
#include "quihelper.h"

frmAbout *frmAbout::self = NULL;
frmAbout *frmAbout::Instance()
{
    if (!self) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new frmAbout;
        }
    }

    return self;
}

frmAbout::frmAbout(QWidget *parent) : QDialog(parent), ui(new Ui::frmAbout)
{
    ui->setupUi(this);
    this->initForm();
    QUIHelper::setFormInCenter(this);
}

frmAbout::~frmAbout()
{
    delete ui;
}

void frmAbout::initForm()
{
    ui->labName->setText(AppConfig::TitleCn);
    ui->labTitle->setText(QString("关于%1").arg(AppConfig::TitleCn));
    ui->labVesion->setText(QString("版本 : %1").arg(AppData::Version));
    ui->labHttp->setText(QString("<style> a{text-decoration:none;color:white;} </style><a href=%1>网址 : %1</a>").arg(AppConfig::HttpUrl));
    ui->labCopyright->setText(QString("版权 : %1").arg(AppConfig::Copyright));

    this->setProperty("canMove", true);
    this->setWindowTitle(ui->labTitle->text());
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    IconHelper::setIcon(ui->labIco, QUIConfig::IconMain, 15);
    IconHelper::setIcon(ui->btnMenu_Close, QUIConfig::IconClose, 12);
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(hide()));
}
