#ifndef APPEVENT_H
#define APPEVENT_H

#include <QObject>

class DeviceButton;

class AppEvent : public QObject
{
    Q_OBJECT
public:
    static AppEvent *Instance();
    explicit AppEvent(QObject *parent = 0);

private:
    static QScopedPointer<AppEvent> self;

signals:
    void exitAll();
    void changeStyle();
    void changeText();
    void changeColor();
    void doubleClicked(DeviceButton *btn);

public slots:
    void slot_exitAll();
    void slot_changeStyle();
    void slot_changeText();
    void slot_changeColor();
    void slot_doubleClicked();
};

#endif // APPEVENT_H
