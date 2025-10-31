#include "frmtool.h"
#include "frmmain.h"
#include "frmlogin.h"
#include "quihelper.h"
#include "appinit.h"
#include "appstyle.h"

void customMessageHandler(QtMsgType type,
                          const QMessageLogContext &context,
                          const QString & msg) {
    QString txt;
    switch (type) {
    //调试信息提示
    case QtDebugMsg:
        txt = QString("%1: Debug: at:%2,%3 on %4; %5").arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                .arg(context.file).arg(context.line).arg(context.function).arg(msg);
        txt = msg;
        break;
    //一般的warning提示
    case QtWarningMsg:
        txt = QString("%1:Warning: at:%2,%3 on %4; %5").arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                .arg(context.file)
                .arg(context.line)
                .arg(context.function)
                .arg(msg);
        break;
    //严重错误提示
    case QtCriticalMsg:
        txt = QString("%1:Critical: at:%2,%3 on %4; %5").arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                .arg(context.file)
                .arg(context.line)
                .arg(context.function)
                .arg(msg);
        break;
    //致命错误提示
    case QtFatalMsg:
        txt = QString("%1:Fatal: at:%2,%3 on %4; %5").arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                .arg(context.file)
                .arg(context.line)
                .arg(context.function)
                .arg(msg);
        abort();
    }

    QString time = QDateTime::currentDateTime().toString("yyyyMMdd");
    QFile outFile(QCoreApplication::applicationDirPath() + "/Log/" + time + "Log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    if (txt != "")
    {
        ts << txt << "\r\n";
    }
    outFile.flush();
    outFile.close();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(customMessageHandler);
#ifndef Q_OS_WIN
    //QCoreApplication::setEventDispatcher(new EventDispatcherLibEv());
#endif
    //设置不应用操作系统设置比如字体
    QApplication::setDesktopSettingsAware(false);

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    //设置高分屏缩放舍入策略
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif
#if (QT_VERSION > QT_VERSION_CHECK(5,6,0))
    //设置启用高分屏缩放支持
    //要注意开启后计算到的控件或界面宽度高度可能都不对,全部需要用缩放比例运算下
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //设置启用高分屏图片支持
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication a(argc, argv);
    AppInit::Instance()->start();
    AppStyle::initStyle();

    QWidget *w;
#ifdef cswmstool
    w = new frmTool;
    w->setWindowTitle(QString("设备模拟调试工具 %1").arg(AppData::Version));
#else
    if (AppConfig::AutoLogin) {
        w = new frmMain;
    } else {
        w = new frmLogin;
    }
#endif

    w->show();
    w->activateWindow();
    return a.exec();
}
