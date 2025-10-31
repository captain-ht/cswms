
include(../../VendorPlugin.pri)

SOURCES += ModbusASCII.cpp \
    ../../core_tag/IOTag.cpp \
    ../Public/DataPack.cpp \
    ../Public/Modbus.cpp \
    ModbusASCIIImpl.cpp

HEADERS += ModbusASCII.h \
    ../../core_tag/IOTag.h \
    ../Public/DataPack.h \
    ../Public/Modbus.h \
    ModbusASCIIImpl.h
DISTFILES += ModbusASCII.json


