QT += core gui sql xml network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

#如果改成 cswmstool  则编译出来的是设备模拟工具
#如果改成 scada     则启用组态设计模块 有些环境没有而且需要配合自定义控件大全使用 默认建议关闭
#如果改成 ssl       则发邮件模块启用ssl 有些环境没有opensll需要改成 ssl1
DEFINES += cswmstool1 scada ssl qcustomplot_v2
contains(DEFINES, cswmstool) {
TARGET = cswmstool
RC_ICONS = other/tool.ico
} else {
TARGET = cswms
RC_ICONS = other/main.ico
}

contains(DEFINES, scada) {
greaterThan(QT_MAJOR_VERSION, 4){
QT += designer
} else {
CONFIG += designer
}}

TEMPLATE    = app
VERSION     = 2021.09.25
#开启预编译头可以加快编译速度 但是部分构建套件环境不支持
#编译遇到问题不通过可以试着不要开启下面这行
PRECOMPILED_HEADER = head.h

win32 {
    LIBS += -lpthread -lwsock32 -lws2_32
} else:unix {
    LIBS += -pthread
}

HEADERS     += head.h
SOURCES     += main.cpp
RESOURCES   += other/main.qrc

INCLUDEPATH += $$PWD
include ($$PWD/3rd.pri)
include ($$PWD/core.pri)
include ($$PWD/ui/ui.pri)
include ($$PWD/class/class.pri)

DISTFILES += \
    other/image/BigCart.png

