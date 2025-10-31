greaterThan(QT_MAJOR_VERSION, 4) {
qtHaveModule(multimedia) {
QT += multimedia
DEFINES += playsound
}}

HEADERS += \
    $$PWD/bottomwidget.h \
    $$PWD/colorcombobox.h \
    $$PWD/cpumemorylabel.h \
    $$PWD/customtitlebar.h \
    $$PWD/devicebutton.h \
    $$PWD/framelesswidget.h \
    $$PWD/lcddatetime.h \
    $$PWD/panelwidget.h \
    $$PWD/playwav.h \
    $$PWD/savelog.h \
    $$PWD/saveruntime.h \
    $$PWD/switchbutton.h \
    $$PWD/xslider.h

SOURCES += \
    $$PWD/bottomwidget.cpp \
    $$PWD/colorcombobox.cpp \
    $$PWD/cpumemorylabel.cpp \
    $$PWD/customtitlebar.cpp \
    $$PWD/devicebutton.cpp \
    $$PWD/framelesswidget.cpp \
    $$PWD/lcddatetime.cpp \
    $$PWD/panelwidget.cpp \
    $$PWD/playwav.cpp \
    $$PWD/savelog.cpp \
    $$PWD/saveruntime.cpp \
    $$PWD/switchbutton.cpp \
    $$PWD/xslider.cpp

RESOURCES += \
    $$PWD/control.qrc
