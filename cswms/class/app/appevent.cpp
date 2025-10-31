#include "appevent.h"
#include "qmutex.h"
#include "devicebutton.h"

QScopedPointer<AppEvent> AppEvent::self;
AppEvent *AppEvent::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new AppEvent);
        }
    }

    return self.data();
}

AppEvent::AppEvent(QObject *parent) : QObject(parent)
{

}

void AppEvent::slot_exitAll()
{
    emit exitAll();
}

void AppEvent::slot_changeStyle()
{
    emit changeStyle();
}

void AppEvent::slot_changeText()
{
    emit changeText();
}

void AppEvent::slot_changeColor()
{
    emit changeColor();
}

void AppEvent::slot_doubleClicked()
{
    DeviceButton *btn = (DeviceButton *)sender();
    emit doubleClicked(btn);
}
