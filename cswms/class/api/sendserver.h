#ifndef SENDSERVER_H
#define SENDSERVER_H

#include <QObject>
#include <QDateTime>

class QTimer;

class SendServer : public QObject
{
    Q_OBJECT
public:
    static SendServer *Instance();
    explicit SendServer(QObject *parent = 0);
    ~SendServer();

private:
    static QScopedPointer<SendServer> self;

    QTimer *timerMsg;           //报警短信转发定时器
    QTimer *timerEmail;         //报警邮件转发定时器

    QList<QString> listMsg;     //报警短信链表
    QList<QString> listEmail;   //报警邮件链表

private slots:
    void receiveMsgResult(const QString &result);
    void receiveEmailResult(const QString &result);

public slots:
    void start();
    void stop();
    void changeMsgInterval();
    void changeEmailInterval();

    //追加到报警短信链表
    void appendMsg(const QString &msg);
    //追加到报警邮件链表
    void appendEmail(const QString &email);
    //统一添加内容
    void append(const QString &content);

    //清空报警短信链表
    void clearMsg();
    //清空报警邮件链表
    void clearEmail();

    //报警短信转发
    void checkMsg();
    //报警邮件转发
    void checkEmail();
};

#endif // SENDSERVER_H
