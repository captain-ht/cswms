
include(../../VendorPlugin.pri)

SOURCES += S7_200.cpp \
    ../../core_tag/IOTag.cpp \
    ../Public/DataPack.cpp \
    SiemensS7_200Impl.cpp

HEADERS += S7_200.h \
    ../../core_tag/IOTag.h \
    ../Public/DataPack.h \
    SiemensS7_200Impl.h
DISTFILES += S7_200.json


