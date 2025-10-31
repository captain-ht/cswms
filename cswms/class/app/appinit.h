#ifndef APPINIT_H
#define APPINIT_H

#include <QObject>

class DbCleanThread;

class AppInit : public QObject
{
    Q_OBJECT
public:
    static AppInit *Instance();
    explicit AppInit(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<AppInit> self;
    DbCleanThread *cleanAlarmLog;
    DbCleanThread *cleanNodeLog;
    DbCleanThread *cleanUserLog;

public slots:
    void start();           //启动服务
    void initDb();          //初始化数据库
    void initWorkMode();    //初始化不同工作模式的全局设置
    void autoLogin();       //自动登录日志
    void changeDbMaxCount();//更新最大日志记录数量
};

#endif // APPINIT_H
