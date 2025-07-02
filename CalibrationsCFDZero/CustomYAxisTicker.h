#ifndef CUSTOM_Y_AXIS_TICKER_H
#define CUSTOM_Y_AXIS_TICKER_H

#include "qcustomplot.h"

class CustomYAxisTicker : public QCPAxisTickerFixed {
public:
    QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) override;
    double getTickStep(const QCPRange &range) override;
    int getSubTickCount(double tickStep) override;
};

class xAxisTicker : public QCPAxisTickerFixed {
public:
    int &fontSize_px, &axisLength_px;
    xAxisTicker(int &_fontSize_px, int &_axisLength_px);

    double getTickStep(const QCPRange &range) override;
    int getSubTickCount(double tickStep) override;
};

#endif // CUSTOM_Y_AXIS_TICKER_H
