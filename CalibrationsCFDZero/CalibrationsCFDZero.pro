TEMPLATE = lib

TARGET = CalibrationsCFDZero

CONFIG += qt dll release
QT += widgets printsupport

DEFINES += API_ROOT=\"C:/Siemens/Automation/WinCC_OA/3.19/api\"

#INCLUDEPATH += . $$(API_ROOT)/include/EWO
INCLUDEPATH += C:/Siemens/Automation/WinCC_OA/3.19/api/include/EWO

unix: LIBS += -L$$(API_ROOT)/../bin -lewo
#win32:LIBS += -L$$(API_ROOT)/lib.winnt -lewo
win32:LIBS += "C:/Siemens/Automation/WinCC_OA/3.19/api/lib.winnt/ewo.lib"

HEADERS = CalibrationsCFDZero.hxx
SOURCES = CalibrationsCFDZero.cxx

SOURCES += qcustomplot.cpp
HEADERS += qcustomplot.h

SOURCES += CustomYAxisTicker.cpp
HEADERS += CustomYAxisTicker.h

SOURCES += MyWidget.cpp
HEADERS += MyWidget.hxx
