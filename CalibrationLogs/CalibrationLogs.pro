TEMPLATE = lib

TARGET = CalibrationLogs

CONFIG += qt dll release
QT += widgets printsupport

DEFINES += API_ROOT=\"C:/Siemens/Automation/WinCC_OA/3.19/api\"

#INCLUDEPATH += . $$(API_ROOT)/include/EWO
INCLUDEPATH += C:/Siemens/Automation/WinCC_OA/3.19/api/include/EWO

unix: LIBS += -L$$(API_ROOT)/../bin -lewo
#win32:LIBS += -L$$(API_ROOT)/lib.winnt -lewo
win32:LIBS += "C:/Siemens/Automation/WinCC_OA/3.19/api/lib.winnt/ewo.lib"

HEADERS = CalibrationLogs.hxx
SOURCES = CalibrationLogs.cxx
