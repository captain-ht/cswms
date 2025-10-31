
include(../../VendorPlugin.pri)

SOURCES += ModbusRTU.cpp \
    ../../core_tag/IOTag.cpp \
    ../Public/DataPack.cpp \
    ../Public/Modbus.cpp \
    ModbusRTUImpl.cpp

HEADERS += ModbusRTU.h \
    ../../core_tag/IOTag.h \
    ../Public/DataPack.h \
    ../Public/Modbus.h \
    ModbusRTUImpl.h
DISTFILES += ModbusRTU.json


