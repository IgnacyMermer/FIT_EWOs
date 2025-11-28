TEMPLATE = lib
TARGET = HistogramWidget

CONFIG += qt dll release
QT += widgets printsupport

DEFINES += API_ROOT=\"C:/Siemens/Automation/WinCC_OA/3.19/api\"

INCLUDEPATH += C:/Siemens/Automation/WinCC_OA/3.19/api/include/EWO
INCLUDEPATH += .

# Biblioteka EWO (WinCC OA)
win32:LIBS += "C:/Siemens/Automation/WinCC_OA/3.19/api/lib.winnt/ewo.lib"
unix: LIBS += -L$$(API_ROOT)/../bin -lewo

# Główne źródła
SOURCES += \
    HistogramWidget.cxx \
    SinglePlotWidget.cxx \
    qcustomplot.cpp \
    CustomYAxisTicker.cpp

HEADERS += \
    HistogramWidget.hxx \
    SinglePlotWidget.h \
    qcustomplot.h \
    CustomYAxisTicker.h
