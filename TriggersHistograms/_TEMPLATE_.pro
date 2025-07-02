TEMPLATE = lib

TARGET = _TEMPLATE_

CONFIG += qt dll release
QT += widgets

DEFINES += API_ROOT=\"C:/Siemens/Automation/WinCC_OA/3.19/api\"

INCLUDEPATH += . $$(API_ROOT)/include/EWO

unix: LIBS += -L$$(API_ROOT)/../bin -lewo
#win32:LIBS += -L$$(API_ROOT)/lib.winnt -lewo
win32:LIBS += "C:/Siemens/Automation/WinCC_OA/3.19/api/lib.winnt/ewo.lib"

HEADERS = _TEMPLATE_.hxx
SOURCES = _TEMPLATE_.cxx

win32:RC_FILE = VersInfo.rc
