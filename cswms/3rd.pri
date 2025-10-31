INCLUDEPATH += $$PWD/../3rd_smtpclient
INCLUDEPATH += $$PWD/../3rd_qcustomplot
INCLUDEPATH += $$PWD/../3rd_qextserialport
INCLUDEPATH += $$PWD/../3rd_log4qt

include ($$PWD/../3rd_smtpclient/3rd_smtpclient.pri)
include ($$PWD/../3rd_qcustomplot/3rd_qcustomplot.pri)
include ($$PWD/../3rd_qextserialport/3rd_qextserialport.pri)
include ($$PWD/../3rd_log4qt/3rd_log4qt.pri)

contains(DEFINES, scada) {
INCLUDEPATH += $$PWD/../3rd_qtpropertybrowser
include ($$PWD/../3rd_qtpropertybrowser/3rd_qtpropertybrowser.pri)
}
