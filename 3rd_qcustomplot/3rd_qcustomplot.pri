greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

!contains(DEFINES, qcustomplot_v1) {
!contains(DEFINES, qcustomplot_v2) {
DEFINES += qcustomplot_v1
}}

greaterThan(QT_MAJOR_VERSION, 5) {
DEFINES -= qcustomplot_v1
DEFINES -= qcustomplot_v2
DEFINES += qcustomplot_v3
}

contains(DEFINES, qcustomplot_v1) {
HEADERS += $$PWD/qcustomplot1.h
SOURCES += $$PWD/qcustomplot1.cpp
} else {
contains(DEFINES, qcustomplot_v2) {
HEADERS += $$PWD/qcustomplot2.h
SOURCES += $$PWD/qcustomplot2.cpp
} else {
HEADERS += $$PWD/qcustomplot3.h
SOURCES += $$PWD/qcustomplot3.cpp
}}
