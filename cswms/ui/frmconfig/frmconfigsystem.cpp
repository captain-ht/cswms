#include "frmconfigsystem.h"
#include "ui_frmconfigsystem.h"
#include "quihelper.h"
#include "dbquery.h"
#include "appinit.h"
#include "appstyle.h"
#include "sendserver.h"
#include "dataxls.h"
#include "udpsend.h"
#include "savelog.h"

frmConfigSystem::frmConfigSystem(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigSystem)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig1();
    this->initConfig2();
    this->initConfig3();
    this->initLog();
    this->initColor();
    this->initNetLink();
    this->initMsgLink();
    this->initEmailLink();
    this->initTime();
}

frmConfigSystem::~frmConfigSystem()
{
    delete ui;
}

void frmConfigSystem::initForm()
{
    int fixWidth = 440;
    QList<QGroupBox *> gboxs = this->findChildren<QGroupBox *>();
    foreach (QGroupBox *gbox, gboxs) {
        gbox->setFixedWidth(fixWidth);
    }

    ui->widgetConfigDb->setBtnInRight(true);

    //启动报警短信+报警邮件转发
    QTimer::singleShot(1000, SendServer::Instance(), SLOT(start()));
}

void frmConfigSystem::initConfig1()
{
    ui->btnAutoRun->setChecked(AppConfig::AutoRun);
    connect(ui->btnAutoRun, SIGNAL(checkedChanged(bool)), this, SLOT(saveConfig1()));

    ui->btnAutoLogin->setChecked(AppConfig::AutoLogin);
    connect(ui->btnAutoLogin, SIGNAL(checkedChanged(bool)), this, SLOT(saveConfig1()));

    ui->btnAutoPwd->setChecked(AppConfig::AutoPwd);
    connect(ui->btnAutoPwd, SIGNAL(checkedChanged(bool)), this, SLOT(saveConfig1()));

    ui->btnAutoConfirm->setChecked(AppConfig::AutoConfirm);
    connect(ui->btnAutoConfirm, SIGNAL(checkedChanged(bool)), this, SLOT(saveConfig1()));

    ui->btnSaveLog->setChecked(AppConfig::SaveLog);
    connect(ui->btnSaveLog, SIGNAL(checkedChanged(bool)), this, SLOT(saveConfig1()));
    if (AppConfig::SaveLog) {
        SaveLog::Instance()->setPath(QUIHelper::appPath() + "/log");
        SaveLog::Instance()->setToNet(false);
        SaveLog::Instance()->start();
    }
}

void frmConfigSystem::saveConfig1()
{
    AppConfig::AutoRun = ui->btnAutoRun->getChecked();
    AppConfig::AutoLogin = ui->btnAutoLogin->getChecked();
    AppConfig::AutoPwd = ui->btnAutoPwd->getChecked();
    AppConfig::AutoConfirm = ui->btnAutoConfirm->getChecked();

    bool saveLog = ui->btnSaveLog->getChecked();
    if (AppConfig::SaveLog != saveLog) {
        AppConfig::SaveLog = saveLog;
        AppConfig::SaveLog ? SaveLog::Instance()->start() : SaveLog::Instance()->stop();
    }

    AppConfig::writeConfig();
}

void frmConfigSystem::initConfig2()
{
    ui->txtCopyright->setText(AppConfig::Copyright);
    ui->txtCopyright->setSelection(0, 0);
    connect(ui->txtCopyright, SIGNAL(textChanged(QString)), this, SLOT(saveConfig2()));

    ui->txtTitleCn->setText(AppConfig::TitleCn);
    ui->txtTitleCn->setSelection(0, 0);
    connect(ui->txtTitleCn, SIGNAL(textChanged(QString)), this, SLOT(saveConfig2()));

    ui->txtTitleEn->setText(AppConfig::TitleEn);
    ui->txtTitleEn->setSelection(0, 0);
    connect(ui->txtTitleEn, SIGNAL(textChanged(QString)), this, SLOT(saveConfig2()));    

    ui->txtCopyright->setCursorPosition(0);
    ui->txtTitleCn->setCursorPosition(0);
    ui->txtTitleEn->setCursorPosition(0);

    QStringList styleNames, styleFiles;
    QUIStyle::getStyle(styleNames, styleFiles);
    for (int i = 0; i < styleNames.count(); i++) {
        ui->cboxStyleName->addItem(styleNames.at(i), styleFiles.at(i));
    }

    ui->cboxStyleName->setCurrentIndex(ui->cboxStyleName->findData(AppConfig::StyleName));
    connect(ui->cboxStyleName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig2()));
}

void frmConfigSystem::saveConfig2()
{
    QString copyright = ui->txtCopyright->text();
    if (AppConfig::Copyright != copyright) {
        AppConfig::Copyright = copyright;
        AppEvent::Instance()->slot_changeText();
    }

    QString titleCn = ui->txtTitleCn->text();
    if (AppConfig::TitleCn != titleCn) {
        AppConfig::TitleCn = titleCn;
        AppEvent::Instance()->slot_changeText();
    }

    QString titleEn = ui->txtTitleEn->text();
    if (AppConfig::TitleEn != titleEn) {
        AppConfig::TitleEn = titleEn;
        AppEvent::Instance()->slot_changeText();
    }    

    QString styleName = ui->cboxStyleName->itemData(ui->cboxStyleName->currentIndex()).toString();
    if (AppConfig::StyleName != styleName) {
        AppConfig::StyleName = styleName;
        AppStyle::initStyle();
        AppEvent::Instance()->slot_changeStyle();
    }

    AppConfig::writeConfig();
}

void frmConfigSystem::initConfig3()
{
    ui->cboxWorkMode->addItem("从设备采集");
    ui->cboxWorkMode->addItem("从数据库采集");
    ui->cboxWorkMode->addItem("从数据库读取");
    ui->cboxWorkMode->addItem("从设备采集2");
    ui->cboxWorkMode->setCurrentIndex(AppConfig::WorkMode);
    connect(ui->cboxWorkMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig3()));

    QStringList playCount;
    playCount << "1次" << "2次" << "3次" << "5次" << "10次" << "15次" << "20次" << "30次" << "40次" << "50次" << "80次" << "100次";
    ui->cboxPlayCount->addItems(playCount);
    QString strPlayCount = QString("%1次").arg(AppConfig::PlayCount);
    ui->cboxPlayCount->setCurrentIndex(ui->cboxPlayCount->findText(strPlayCount));
    connect(ui->cboxPlayCount, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig3()));

    ui->btnPlayAlarmSound->setChecked(AppConfig::PlayAlarmSound);
    connect(ui->btnPlayAlarmSound, SIGNAL(checkedChanged(bool)), this, SLOT(saveConfig3()));

    QStringList rowCount;
    rowCount << "0行";
    for (int i = 12; i <= 40; i++) {
        rowCount << QString("%1行").arg(i);
    }
    rowCount << "45行" << "50行" << "55行" << "60行" << "70行" << "80行" << "90行" << "100行";

    ui->cboxMsgCount->addItems(rowCount);
    QString strMsgCount = QString("%1行").arg(AppConfig::MsgCount);
    ui->cboxMsgCount->setCurrentIndex(ui->cboxMsgCount->findText(strMsgCount));
    connect(ui->cboxMsgCount, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig3()));

    QStringList deviceColumn;
    for (int i = 2; i <= 20; i++) {
        deviceColumn << QString("%1列").arg(i);
    }

    ui->cboxDeviceColumn->addItems(deviceColumn);
    QString strDeviceColumn = QString("%1列").arg(AppConfig::DeviceColumn);
    ui->cboxDeviceColumn->setCurrentIndex(ui->cboxDeviceColumn->findText(strDeviceColumn));
    connect(ui->cboxDeviceColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig3()));

    ui->cboxRecordsPerpage->addItems(rowCount);
    QString strRecordsPerpage = QString("%1行").arg(AppConfig::RecordsPerpage);
    ui->cboxRecordsPerpage->setCurrentIndex(ui->cboxRecordsPerpage->findText(strRecordsPerpage));
    connect(ui->cboxRecordsPerpage, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig3()));

    QStringList tipInterval;
    tipInterval << "0秒" << "3秒" << "5秒" << "10秒" << "20秒" << "30秒" << "60秒" << "120秒" << "10000秒";
    ui->cboxTipInterval->addItems(tipInterval);
    QString strTipInterval = QString("%1秒").arg(AppConfig::TipInterval);
    ui->cboxTipInterval->setCurrentIndex(ui->cboxTipInterval->findText(strTipInterval));
    connect(ui->cboxTipInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig3()));

    QStringList precision;
    precision << "0位" << "1位" << "2位" << "3位";
    ui->cboxPrecision->addItems(precision);
    QString strPrecision = QString("%1位").arg(AppConfig::Precision);
    ui->cboxPrecision->setCurrentIndex(ui->cboxPrecision->findText(strPrecision));
    connect(ui->cboxPrecision, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig3()));
}

void frmConfigSystem::saveConfig3()
{
    int workMode = ui->cboxWorkMode->currentIndex();
    if (AppConfig::WorkMode != workMode) {
        AppConfig::WorkMode = workMode;
        AppConfig::writeConfig();
        AppFun::reboot();
    }

    AppConfig::PlayAlarmSound = ui->btnPlayAlarmSound->getChecked();
    QString strPlayCount = ui->cboxPlayCount->currentText();
    AppConfig::PlayCount = strPlayCount.mid(0, strPlayCount.length() - 1).toInt();

    QString strMsgCount = ui->cboxMsgCount->currentText();
    AppConfig::MsgCount = strMsgCount.mid(0, strMsgCount.length() - 1).toInt();

    QString strDeviceColumn = ui->cboxDeviceColumn->currentText();
    AppConfig::DeviceColumn = strDeviceColumn.mid(0, strDeviceColumn.length() - 1).toInt();

    QString strRecordsPerpage = ui->cboxRecordsPerpage->currentText();
    AppConfig::RecordsPerpage = strRecordsPerpage.mid(0, strRecordsPerpage.length() - 1).toInt();

    QString strTipInterval = ui->cboxTipInterval->currentText();
    AppConfig::TipInterval = strTipInterval.mid(0, strTipInterval.length() - 1).toInt();

    QString strPrecision = ui->cboxPrecision->currentText();
    AppConfig::Precision = strPrecision.mid(0, strPrecision.length() - 1).toInt();

    AppConfig::writeConfig();
}

void frmConfigSystem::initLog()
{
    QStringList logMaxCount;
    logMaxCount << "1万条" << "5万条" << "10万条" << "50万条" << "100万条" << "500万条" << "1000万条";
    ui->cboxAlarmLogMaxCount->addItems(logMaxCount);
    ui->cboxNodeLogMaxCount->addItems(logMaxCount);
    ui->cboxUserLogMaxCount->addItems(logMaxCount);

    QString strAlarmLogMaxCount = QString("%1万条").arg(AppConfig::AlarmLogMaxCount);
    ui->cboxAlarmLogMaxCount->setCurrentIndex(ui->cboxAlarmLogMaxCount->findText(strAlarmLogMaxCount));
    connect(ui->cboxAlarmLogMaxCount, SIGNAL(currentIndexChanged(int)), this, SLOT(saveLog()));

    QString strNodeLogMaxCount = QString("%1万条").arg(AppConfig::NodeLogMaxCount);
    ui->cboxNodeLogMaxCount->setCurrentIndex(ui->cboxNodeLogMaxCount->findText(strNodeLogMaxCount));
    connect(ui->cboxNodeLogMaxCount, SIGNAL(currentIndexChanged(int)), this, SLOT(saveLog()));

    QString strUserLogMaxCount = QString("%1万条").arg(AppConfig::UserLogMaxCount);
    ui->cboxUserLogMaxCount->setCurrentIndex(ui->cboxUserLogMaxCount->findText(strUserLogMaxCount));
    connect(ui->cboxUserLogMaxCount, SIGNAL(currentIndexChanged(int)), this, SLOT(saveLog()));

    ui->cboxAlarmLogOrder->addItem("升序", "asc");
    ui->cboxAlarmLogOrder->addItem("降序", "desc");
    ui->cboxAlarmLogOrder->setCurrentIndex(AppConfig::AlarmLogOrder == "asc" ? 0 : 1);
    connect(ui->cboxAlarmLogOrder, SIGNAL(currentIndexChanged(int)), this, SLOT(saveLog()));

    ui->cboxNodeLogOrder->addItem("升序", "asc");
    ui->cboxNodeLogOrder->addItem("降序", "desc");
    ui->cboxNodeLogOrder->setCurrentIndex(AppConfig::NodeLogOrder == "asc" ? 0 : 1);
    connect(ui->cboxNodeLogOrder, SIGNAL(currentIndexChanged(int)), this, SLOT(saveLog()));

    ui->cboxUserLogOrder->addItem("升序", "asc");
    ui->cboxUserLogOrder->addItem("降序", "desc");
    ui->cboxUserLogOrder->setCurrentIndex(AppConfig::UserLogOrder == "asc" ? 0 : 1);
    connect(ui->cboxUserLogOrder, SIGNAL(currentIndexChanged(int)), this, SLOT(saveLog()));
}

void frmConfigSystem::saveLog()
{
    QString strAlarmLogMaxCount = ui->cboxAlarmLogMaxCount->currentText();
    int alarmLogMaxCount = strAlarmLogMaxCount.mid(0, strAlarmLogMaxCount.length() - 2).toInt();
    if (AppConfig::AlarmLogMaxCount != alarmLogMaxCount) {
        AppConfig::AlarmLogMaxCount = alarmLogMaxCount;
        AppInit::Instance()->changeDbMaxCount();
    }

    QString strNodeLogMaxCount = ui->cboxNodeLogMaxCount->currentText();
    int nodeLogMaxCount = strNodeLogMaxCount.mid(0, strNodeLogMaxCount.length() - 2).toInt();
    if (AppConfig::NodeLogMaxCount != nodeLogMaxCount) {
        AppConfig::NodeLogMaxCount = nodeLogMaxCount;
        AppInit::Instance()->changeDbMaxCount();
    }

    QString strUserLogMaxCount = ui->cboxUserLogMaxCount->currentText();
    int userLogMaxCount = strUserLogMaxCount.mid(0, strUserLogMaxCount.length() - 2).toInt();
    if (AppConfig::UserLogMaxCount != userLogMaxCount) {
        AppConfig::UserLogMaxCount = userLogMaxCount;
        AppInit::Instance()->changeDbMaxCount();
    }

    AppConfig::AlarmLogOrder = ui->cboxAlarmLogOrder->itemData(ui->cboxAlarmLogOrder->currentIndex()).toString();
    AppConfig::NodeLogOrder = ui->cboxNodeLogOrder->itemData(ui->cboxNodeLogOrder->currentIndex()).toString();
    AppConfig::UserLogOrder = ui->cboxUserLogOrder->itemData(ui->cboxUserLogOrder->currentIndex()).toString();

    AppConfig::writeConfig();
}

void frmConfigSystem::initColor()
{
    ui->cboxColorZero->setColorName(AppConfig::ColorZero);
    connect(ui->cboxColorZero, SIGNAL(colorChanged(QString)), this, SLOT(saveColor()));

    ui->cboxColorUpper->setColorName(AppConfig::ColorUpper);
    connect(ui->cboxColorUpper, SIGNAL(colorChanged(QString)), this, SLOT(saveColor()));

    ui->cboxColorLimit->setColorName(AppConfig::ColorLimit);
    connect(ui->cboxColorLimit, SIGNAL(colorChanged(QString)), this, SLOT(saveColor()));

    ui->cboxColorNormal->setColorName(AppConfig::ColorNormal);
    connect(ui->cboxColorNormal, SIGNAL(colorChanged(QString)), this, SLOT(saveColor()));

    ui->cboxColorPlotBg->setColorName(AppConfig::ColorPlotBg);
    connect(ui->cboxColorPlotBg, SIGNAL(colorChanged(QString)), this, SLOT(saveColor()));

    ui->cboxColorPlotText->setColorName(AppConfig::ColorPlotText);
    connect(ui->cboxColorPlotText, SIGNAL(colorChanged(QString)), this, SLOT(saveColor()));

    ui->cboxColorPlotLine->setColorName(AppConfig::ColorPlotLine);
    connect(ui->cboxColorPlotLine, SIGNAL(colorChanged(QString)), this, SLOT(saveColor()));

    ui->cboxColorOther->setColorName(AppConfig::ColorOther);
    connect(ui->cboxColorOther, SIGNAL(colorChanged(QString)), this, SLOT(saveColor()));
}

void frmConfigSystem::saveColor()
{
    AppConfig::ColorZero = ui->cboxColorZero->getColorName();
    AppConfig::ColorUpper = ui->cboxColorUpper->getColorName();
    AppConfig::ColorLimit = ui->cboxColorLimit->getColorName();
    AppConfig::ColorNormal = ui->cboxColorNormal->getColorName();
    AppConfig::ColorOther = ui->cboxColorOther->getColorName();

    QString colorPlotBg = ui->cboxColorPlotBg->getColorName();
    if (AppConfig::ColorPlotBg != colorPlotBg) {
        AppConfig::ColorPlotBg = colorPlotBg;
        AppEvent::Instance()->slot_changeColor();
    }

    QString colorPlotText = ui->cboxColorPlotText->getColorName();
    if (AppConfig::ColorPlotText != colorPlotText) {
        AppConfig::ColorPlotText = colorPlotText;
        AppEvent::Instance()->slot_changeColor();
    }

    QString colorPlotLine = ui->cboxColorPlotLine->getColorName();
    if (AppConfig::ColorPlotLine != colorPlotLine) {
        AppConfig::ColorPlotLine = colorPlotLine;
        AppEvent::Instance()->slot_changeColor();
    }

    AppConfig::writeConfig();
}

void frmConfigSystem::initNetLink()
{
    ui->btnUseNetSend->setChecked(AppConfig::UseNetSend);
    connect(ui->btnUseNetSend, SIGNAL(checkedChanged(bool)), this, SLOT(saveNetLink()));

    ui->txtNetSendInfo->setText(AppConfig::NetSendInfo);
    ui->txtNetSendInfo->setPlaceholderText("多个IP中间用英文分号 ; 隔开");
    connect(ui->txtNetSendInfo, SIGNAL(textChanged(QString)), this, SLOT(saveNetLink()));

    ui->btnUseNetReceive->setChecked(AppConfig::UseNetReceive);
    connect(ui->btnUseNetReceive, SIGNAL(checkedChanged(bool)), this, SLOT(saveNetLink()));

    ui->txtNetReceivePort->setText(QString::number(AppConfig::NetReceivePort));
    connect(ui->txtNetReceivePort, SIGNAL(textChanged(QString)), this, SLOT(saveNetLink()));

    //将光标移到前面
    ui->txtNetSendInfo->setCursorPosition(0);
}

void frmConfigSystem::saveNetLink()
{
    AppConfig::UseNetSend = ui->btnUseNetSend->getChecked();
    AppConfig::NetSendInfo = ui->txtNetSendInfo->text().trimmed();
    AppConfig::UseNetReceive = ui->btnUseNetReceive->getChecked();
    AppConfig::NetReceivePort = ui->txtNetReceivePort->text().trimmed().toInt();
    AppConfig::writeConfig();
}

void frmConfigSystem::initMsgLink()
{
    ui->cboxMsgPortName->addItems(AppData::PortNames);
    ui->cboxMsgPortName->setCurrentIndex(ui->cboxMsgPortName->findText(AppConfig::MsgPortName));
    connect(ui->cboxMsgPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveMsgLink()));

    ui->cboxMsgBaudRate->setCurrentIndex(ui->cboxMsgBaudRate->findText(QString::number(AppConfig::MsgBaudRate)));
    connect(ui->cboxMsgBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(saveMsgLink()));

    ui->txtMsgTel->setPlaceholderText("多个号码中间用英文分号 ; 隔开");
    ui->txtMsgTel->setText(AppConfig::MsgTel);
    connect(ui->txtMsgTel, SIGNAL(textChanged(QString)), this, SLOT(saveMsgLink()));

    QStringList interval;
    interval << "0" << "1" << "5" << "10" << "30";
    for (int i = 60; i <= 1440; i = i + 60) {
        interval << QString::number(i);
    }

    interval << "10000";
    ui->cboxMsgInterval->addItems(interval);
    ui->cboxMsgInterval->setCurrentIndex(ui->cboxMsgInterval->findText(QString::number(AppConfig::MsgInterval)));
    connect(ui->cboxMsgInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveMsgLink()));

    //将光标移到前面
    ui->txtMsgTel->setCursorPosition(0);
}

void frmConfigSystem::saveMsgLink()
{
    AppConfig::MsgPortName = ui->cboxMsgPortName->currentText();
    AppConfig::MsgBaudRate = ui->cboxMsgBaudRate->currentText().toInt();
    int msgInterval = ui->cboxMsgInterval->currentText().toInt();

    //自动更改告警短信发送间隔,同时将已经存在的信息清空
    if (msgInterval != AppConfig::MsgInterval) {
        AppConfig::MsgInterval = msgInterval;
        SendServer::Instance()->clearMsg();
        SendServer::Instance()->changeMsgInterval();
    }

    AppConfig::MsgTel = ui->txtMsgTel->text();
    AppConfig::writeConfig();
}

void frmConfigSystem::initEmailLink()
{
    ui->txtSendEmailAddr->setText(AppConfig::SendEmailAddr);
    connect(ui->txtSendEmailAddr, SIGNAL(textChanged(QString)), this, SLOT(saveEmailLink()));

    ui->txtSendEmailPwd->setText(AppConfig::SendEmailPwd);
    connect(ui->txtSendEmailPwd, SIGNAL(textChanged(QString)), this, SLOT(saveEmailLink()));

    ui->txtReceiveEmailAddr->setPlaceholderText("多个邮箱中间用英文分号 ; 隔开");
    ui->txtReceiveEmailAddr->setText(AppConfig::ReceiveEmailAddr);
    connect(ui->txtReceiveEmailAddr, SIGNAL(textChanged(QString)), this, SLOT(saveEmailLink()));

    QStringList interval;
    interval << "0" << "1" << "5" << "10" << "30";
    for (int i = 60; i <= 1440; i = i + 60) {
        interval << QString::number(i);
    }

    interval << "10000";
    ui->cboxEmailInterval->addItems(interval);
    ui->cboxEmailInterval->setCurrentIndex(ui->cboxEmailInterval->findText(QString::number(AppConfig::EmailInterval)));
    connect(ui->cboxEmailInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveEmailLink()));

    //将光标移到前面
    ui->txtSendEmailAddr->setCursorPosition(0);
    ui->txtSendEmailPwd->setCursorPosition(0);
    ui->txtReceiveEmailAddr->setCursorPosition(0);
}

void frmConfigSystem::saveEmailLink()
{
    AppConfig::SendEmailAddr = ui->txtSendEmailAddr->text();
    AppConfig::SendEmailPwd = ui->txtSendEmailPwd->text();
    int emailInterval = ui->cboxEmailInterval->currentText().toInt();

    //自动更改报警邮件发送间隔,同时将已经存在的信息清空
    if (emailInterval != AppConfig::EmailInterval) {
        AppConfig::EmailInterval = emailInterval;
        SendServer::Instance()->clearEmail();
        SendServer::Instance()->changeEmailInterval();
    }

    AppConfig::ReceiveEmailAddr = ui->txtReceiveEmailAddr->text();
    AppConfig::writeConfig();
}

void frmConfigSystem::initTime()
{
    for (int i = 2018; i <= 2030; i++) {
        ui->cboxYear->addItem(QString::number(i));
    }

    for (int i = 1; i <= 12; i++) {
        ui->cboxMonth->addItem(QString::number(i));
    }

    for (int i = 1; i <= 31; i++) {
        ui->cboxDay->addItem(QString::number(i));
    }

    for (int i = 0; i < 24; i++) {
        ui->cboxHour->addItem(QString::number(i));
    }

    for (int i = 0; i < 60; i++) {
        ui->cboxMin->addItem(QString::number(i));
        ui->cboxSec->addItem(QString::number(i));
    }

    QString now = QDateTime::currentDateTime().toString("yyyy-M-d-H-m-s");
    QStringList str = now.split("-");

    int indexYear = ui->cboxYear->findText(str.at(0));
    ui->cboxYear->setCurrentIndex(indexYear < 0 ? 0 : indexYear);
    ui->cboxMonth->setCurrentIndex(ui->cboxMonth->findText(str.at(1)));
    ui->cboxDay->setCurrentIndex(ui->cboxDay->findText(str.at(2)));
    ui->cboxHour->setCurrentIndex(ui->cboxHour->findText(str.at(3)));
    ui->cboxMin->setCurrentIndex(ui->cboxMin->findText(str.at(4)));
    ui->cboxSec->setCurrentIndex(ui->cboxSec->findText(str.at(5)));

    connect(ui->btnSetTime, SIGNAL(clicked(bool)), this, SLOT(saveTime()));
}

void frmConfigSystem::saveTime()
{
    QString year = QString("%1").arg(ui->cboxYear->currentText());
    QString month = QString("%1").arg(ui->cboxMonth->currentText().toInt(), 2, 10, QChar('0'));
    QString day = QString("%1").arg(ui->cboxDay->currentText().toInt(), 2, 10, QChar('0'));
    QString hour = QString("%1").arg(ui->cboxHour->currentText().toInt(), 2, 10, QChar('0'));
    QString min = QString("%1").arg(ui->cboxMin->currentText().toInt(), 2, 10, QChar('0'));
    QString sec = QString("%1").arg(ui->cboxSec->currentText().toInt(), 2, 10, QChar('0'));
    QUIHelper::setSystemDateTime(year, month, day, hour, min, sec);
}

QString frmConfigSystem::saveExcel()
{
    QString whereSql = whereSql = "where Content like '%报警'";
    QString name = "报警记录";
    QString name2 = "alarmlog";

    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "位号" << "控制器名称" << "变量名称" << "报警值" << "单位" << "报警类型" << "开始时间" << "结束时间";
    columnWidths << 130 << 150 << 150 << 70 << 70 << 120 << 150 << 150;

    QString fileName = QString("%1/db/%2_%3.xls").arg(QUIHelper::appPath()).arg(name2).arg(STRDATETIME);
    QString columns = "PositionID,DeviceName,NodeName,NodeValue,NodeSign,Content,StartTime,EndTime";
    QString where = whereSql + " order by LogID desc limit 10000";
    QStringList content = DataCsv::getContent("AlarmLog", columns, where, "", ";");

    if (content.count() == 0) {
        QUIHelper::showMessageBoxError("没有一条对应的报警记录,不用处理", 5);
        return "";
    }

    DataContent dataContent;
    dataContent.fileName = fileName;
    dataContent.sheetName = name;
    dataContent.title = name;
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    dataContent.content = content;
    DataXls::saveXls(dataContent);

    QString msg = QString("发送%1").arg(name);
    DbQuery::addUserLog(msg);
    return fileName;
}

void frmConfigSystem::on_btnSendData_clicked()
{
    UdpSend::Instance()->sendInfo();
}

void frmConfigSystem::on_btnSendMsg_clicked()
{
    SendServer::Instance()->clearMsg();
    SendServer::Instance()->appendMsg("测试报警短信");
    SendServer::Instance()->checkMsg();
}

void frmConfigSystem::on_btnSendEmail_clicked()
{
    QString fileName = "";//saveExcel();
    SendServer::Instance()->clearEmail();
    SendServer::Instance()->appendEmail(QString("测试报警邮件|%1").arg(fileName));
    SendServer::Instance()->checkEmail();
}
