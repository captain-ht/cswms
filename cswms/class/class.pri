INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/app
INCLUDEPATH += $$PWD/device
INCLUDEPATH += $$PWD/usercontrol
INCLUDEPATH += $$PWD/projectdatautils
INCLUDEPATH += $$PWD/utils

include ($$PWD/api/api.pri)
include ($$PWD/app/app.pri)
include ($$PWD/device/device.pri)
include ($$PWD/usercontrol/usercontrol.pri)
include ($$PWD/projectdatautils/projectdatautils.pri)
include ($$PWD/utils/utils.pri)
