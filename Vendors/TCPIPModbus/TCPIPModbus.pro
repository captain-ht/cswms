
include(../../VendorPlugin.pri)

SOURCES += TCPIPModbus.cpp \
    ../../core_tag/IOTag.cpp \
    ../Public/DataPack.cpp \
    ../Public/Modbus.cpp \
    TCPIPModbusImpl.cpp

HEADERS += TCPIPModbus.h \
    ../../core_tag/IOTag.h \
    ../Public/DataPack.h \
    ../Public/Modbus.h \
    TCPIPModbusImpl.h
DISTFILES += TCPIPModbus.json


