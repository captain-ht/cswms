#去除UNICODE字符编码
DEFINES -= UNICODE

HEADERS += \
    $$PWD/ConfigUtils.h \
    $$PWD/Helper.h \
    $$PWD/MiscUtils.h \
    $$PWD/ObjectCreator.h \
    $$PWD/ShareMemory.h \
    $$PWD/Singleton.h \
    $$PWD/StyleSheetHelper.h \
    $$PWD/TimeElapsed.h \
    $$PWD/XMLObject.h

SOURCES += \
    $$PWD/ConfigUtils.cpp \
    $$PWD/Helper.cpp \
    $$PWD/MiscUtils.cpp \
    $$PWD/ShareMemory.cpp \
    $$PWD/StyleSheetHelper.cpp \
    $$PWD/TimeElapsed.cpp \
    $$PWD/XMLObject.cpp \
    $$PWD/xmltest.cpp
