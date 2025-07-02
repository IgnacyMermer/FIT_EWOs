#include "CustomYAxisTicker.h"
#include <cmath>

QString CustomYAxisTicker::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) {
    Q_UNUSED(locale)
    Q_UNUSED(formatChar)
    Q_UNUSED(precision)
    return fabs(tick) < 999.5 ? QString::asprintf("%3.0f", tick)
                              : QString::asprintf("%.0e", tick).replace("e+0", "e");
}

double CustomYAxisTicker::getTickStep(const QCPRange &range) {
    return pow(10, floor(log10(range.upper * 0.9)));
}

int CustomYAxisTicker::getSubTickCount(double tickStep) {
    return tickStep > 1 ? 9 : 0;
}

xAxisTicker::xAxisTicker(int &_fontSize_px, int &_axisLength_px)
    : fontSize_px(_fontSize_px), axisLength_px(_axisLength_px) {}

double xAxisTicker::getTickStep(const QCPRange &range) {
    return range.size() > axisLength_px / fontSize_px ? pow(10, ceil(log10(range.size() * fontSize_px / axisLength_px)))
                                                      : 1;
}

int xAxisTicker::getSubTickCount(double tickStep) {
    return tickStep / pow(10, floor(log10(tickStep))) - 1;
}
