lessThan(QT_MAJOR_VERSION, 5): QT += script

HEADERS += \
    $$PWD/DBTagObject.h \
    $$PWD/RealTimeDB.h

SOURCES += \
    $$PWD/DBTagObject.cpp \
    $$PWD/RealTimeDB.cpp
