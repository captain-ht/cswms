#include "sendserver.h"
#include "quihelper.h"
#include "dbquery.h"
#include "devicehelper.h"
#include "sendmsgthread.h"
#include "sendemailthread.h"

QScopedPointer<SendServer> SendServer::self;
SendServer *SendServer::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new SendServer);
        }
    }

    return self.data();
}

SendServer::SendServer(QObject *parent) : QObject(parent)
{
    //初始化发送报警短信定时器
    timerMsg = new QTimer(this);
    connect(timerMsg, SIGNAL(timeout()), this, SLOT(checkMsg()));
    changeMsgInterval();

    //初始化发送报警邮件定时器
    timerEmail = new QTimer(this);
    connect(timerEmail, SIGNAL(timeout()), this, SLOT(checkEmail()));
    changeEmailInterval();
}

SendServer::~SendServer()
{
    this->stop();
}

void SendServer::receiveMsgResult(const QString &result)
{
    if (AppConfig::MsgInterval != 10000) {
        //DeviceHelper::addMsg(result, 0);
        DbQuery::addUserLog("设备上报", result);
        if (result.contains("失败")) {
            QUIHelper::showMessageBoxError(result, 3);
        } else {
            QUIHelper::showMessageBoxInfo(result, 3);
        }
    }
}

void SendServer::receiveEmailResult(const QString &result)
{
    if (AppConfig::EmailInterval != 10000) {
        //DeviceHelper::addMsg(result, 0);
        DbQuery::addUserLog("设备上报", result);
        if (result.contains("失败")) {
            QUIHelper::showMessageBoxError(result, 3);
        } else {
            QUIHelper::showMessageBoxInfo(result, 3);
        }
    }
}

void SendServer::start()
{
    if (AppConfig::MsgInterval != 10000) {
        QStringList tels = AppConfig::MsgTel.split(";");
        foreach (QString tel, tels) {
            if (!QUIHelper::isTel(tel)) {
                //DeviceHelper::addMsg("转发手机号码错误", 0);
                break;
            }
        }
    }

    //启动发送报警短信线程发送短信,防止界面卡住
    if (SendMsgThread::Instance()->open(AppConfig::MsgPortName)) {
        connect(SendMsgThread::Instance(), SIGNAL(receiveResult(QString)), this, SLOT(receiveMsgResult(QString)));
        SendMsgThread::Instance()->init();
        SendMsgThread::Instance()->start();

        //DeviceHelper::addMsg("短信端口打开成功",0);
        DbQuery::addUserLog("设备上报", "短信端口打开成功");
    } else {
        if (AppConfig::MsgInterval != 10000) {
            //DeviceHelper::addMsg("短信端口打开失败",0);
            DbQuery::addUserLog("设备上报", "短信端口打开失败");
        }
    }

    if (AppConfig::MsgInterval != 10000) {
        if (!QUIHelper::isEmail(AppConfig::SendEmailAddr)) {
            //DeviceHelper::addMsg("发件人邮箱格式错误",0);
            DbQuery::addUserLog("设备上报", "发件人邮箱格式错误");
        }

        QStringList emails = AppConfig::ReceiveEmailAddr.split(";");
        foreach (QString email, emails) {
            if (!QUIHelper::isEmail(email)) {
                //DeviceHelper::addMsg("收件人邮箱格式错误",0);
                DbQuery::addUserLog("设备上报", "收件人邮箱格式错误");
                break;
            }
        }
    }

    //启动发送报警邮件线程发送邮件,防止界面卡住
    connect(SendEmailThread::Instance(), SIGNAL(receiveEmailResult(QString)), this, SLOT(receiveEmailResult(QString)));
    SendEmailThread::Instance()->setEmailTitle(AppConfig::TitleCn + "报警邮件");
    SendEmailThread::Instance()->setSendEmailAddr(AppConfig::SendEmailAddr);
    SendEmailThread::Instance()->setSendEmailPwd(AppConfig::SendEmailPwd);
    SendEmailThread::Instance()->setReceiveEmailAddr(AppConfig::ReceiveEmailAddr);
    SendEmailThread::Instance()->start();
}

void SendServer::stop()
{
    timerMsg->stop();
    timerEmail->stop();
}

void SendServer::changeMsgInterval()
{
    //如果间隔是0则按照10秒间隔发送,如果是10000则不启动
    if (AppConfig::MsgInterval == 0) {
        timerMsg->start(10000);
    } else if (AppConfig::MsgInterval == 10000) {
        timerMsg->stop();
    } else {
        timerMsg->start(AppConfig::MsgInterval * 60 * 1000);
    }
}

void SendServer::changeEmailInterval()
{
    //如果间隔是0秒则按照10秒间隔发送,如果是10000则不启动
    if (AppConfig::EmailInterval == 0) {
        timerEmail->start(10000);
    } else if (AppConfig::EmailInterval == 10000) {
        timerEmail->stop();
    } else {
        timerEmail->start(AppConfig::EmailInterval * 60 * 1000);
    }
}

void SendServer::appendMsg(const QString &msg)
{
    if (AppConfig::MsgInterval <= 1440 && listMsg.count() <= 30) {
        listMsg << msg;
    }
}

void SendServer::appendEmail(const QString &email)
{
    if (AppConfig::EmailInterval <= 1440 && listEmail.count() <= 30) {
        listEmail << email;
    }
}

void SendServer::append(const QString &content)
{
    appendMsg(content);
    appendEmail(content);
}

void SendServer::clearMsg()
{
    listMsg.clear();
}

void SendServer::clearEmail()
{
    listEmail.clear();
}

void SendServer::checkMsg()
{
    int count = listMsg.count();
    if (count == 0) {
        return;
    }

    //一条短信最多70个汉字160个全英文数字字符,同时保证是一条完成的报警信息
    //将要发送的手机号码和短信内容追加到发送报警短信线程中
    for (int i = 0; i < count; i++) {
        QString msg = listMsg.takeFirst();
        QStringList tels = AppConfig::MsgTel.split(";");
        foreach (QString tel, tels) {
            SendMsgThread::Instance()->append(tel, msg);
        }
    }
}

void SendServer::checkEmail()
{
    int count = listEmail.count();
    if (count == 0) {
        return;
    }

    //邮件内容,无论多长都可以拼在一起发送,<br>用来换行显示
    QStringList listContent, listFileName;
    for (int i = 0; i < count; i++) {
        QString str = listEmail.takeFirst();
        if (str.contains("|")) {
            QStringList list = str.split("|");
            listContent.append(list.at(0));
            listFileName.append(list.at(1));
        } else {
            listContent.append(str);
        }
    }

    //将字符串转换为表格形式,先取出字段名,然后取出内容
    //位号: AT-40002  控制器: 1#调度室控制器  变量: 2#阀站CO  触发值: 78.7 %LEL  类型: 浓度上限报警  时间: 2019-02-24 21:01:30<br>
    //位号: AT-40003  控制器: 1#调度室控制器  变量: 3#阀站CO  触发值: 57 %VOL  类型: 浓度下限报警  时间: 2019-02-24 21:01:30<br>
    //位号: AT-40004  控制器: 1#调度室控制器  变量: 4#阀站CO  触发值: 88 PPM  类型: 浓度下限报警  时间: 2019-02-24 21:01:30

    //先取出字段名
    QString str = listContent.first();
    QStringList list = str.split("  ");
    QStringList columnNames;
    foreach (QString str, list) {
        columnNames << str.split(": ").first();
    }

    //字段宽度
    QList<int> columnWidth;
    columnWidth << 150 << 200 << 180 << 130 << 160 << 200;

    //再遍历取出所有数据
    QList<QStringList> rowContents;
    foreach (QString str, listContent) {
        QStringList list = str.split("  ");
        QStringList rowContent;
        foreach (QString temp, list) {
            rowContent << temp.split(": ").last();
        }

        rowContents << rowContent;
    }

    //构建html字符串
    QStringList html;

    //表格开始
    html.append("<table border='1' cellspacing='0' cellpadding='3'>");

    //标题字段名称
    QString title;
    for (int i = 0; i < columnNames.count(); i++) {
        title += QString("<td width='%1' bgcolor='lightgray' align='center' style='vertical-align:middle;'>%2</td>")
                 .arg(columnWidth.at(i)).arg(columnNames.at(i));
    }

    html.append(QString("<tr>%1</tr>").arg(title));

    //内容
    for (int j = 0; j < rowContents.count(); j++) {
        QString row;
        QStringList rowContent = rowContents.at(j);
        for (int i = 0; i < rowContent.count(); i++) {
            row += QString("<td width='%1' align='center' style='vertical-align:middle;'>%2</td>")
                   .arg(columnWidth.at(i)).arg(rowContent.at(i));
        }

        html.append(QString("<tr>%1</tr>").arg(row));
    }

    //表格结束
    html.append("</table>");

    qDebug() << html;

    //将邮件内容和附件链接追加到发送报警邮件线程中
    SendEmailThread::Instance()->setSendEmailAddr(AppConfig::SendEmailAddr);
    SendEmailThread::Instance()->setSendEmailPwd(AppConfig::SendEmailPwd);
    SendEmailThread::Instance()->setReceiveEmailAddr(AppConfig::ReceiveEmailAddr);
    SendEmailThread::Instance()->append(html.join(""), listFileName.join(";"));
    //SendEmailThread::Instance()->append(listContent.join("<br>"), listFileName.join(";"));
}
