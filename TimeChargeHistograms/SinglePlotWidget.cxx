#include "SinglePlotWidget.h"
#include "CustomYAxisTicker.h"
#include <QVBoxLayout>
#include <cmath>

const QColor OKcolor(0xb0d959);

SinglePlotWidget::SinglePlotWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    label = new QLabel(this);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    label->setMinimumHeight(20);
    label->setMaximumHeight(30);
    layout->addWidget(label);

    plot = new QCustomPlot(this);
    layout->addWidget(plot);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plot->axisRect()->setRangeDrag(Qt::Horizontal);
    plot->axisRect()->setRangeZoom(Qt::Horizontal);
    plot->xAxis->setTickLabelFont(QFont("Arial", 5));
    plot->xAxis->setRange(0, 15);
    QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
    intTicker->setTickStep(1.0);
    intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    plot->xAxis->setTicker(intTicker);
    plot->xAxis->setNumberPrecision(0);
    plot->xAxis->setNumberFormat("f");
    plot->xAxis->ticker()->setTickCount(5);
    plot->yAxis->setScaleType(QCPAxis::stLinear);
    plot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    plot->yAxis->ticker()->setTickCount(10);
    plot->yAxis->setTickLabelFont(QFont("Arial", 5));

    plot->axisRect()->setMargins(QMargins(50, 20, 20, 50));
    plot->axisRect()->setupFullAxesBox();

    bars1 = new QCPBars(plot->xAxis, plot->yAxis);
    bars1->setPen(QPen(OKcolor.darker(400)));
    bars1->setBrush(QBrush(OKcolor));
    bars1->setWidth(1);

    bars2 = new QCPBars(plot->xAxis, plot->yAxis);
    bars2->setPen(QPen(OKcolor.darker(400)));
    bars2->setBrush(QBrush(OKcolor.darker(200)));
    bars2->setWidth(1);

    connect(plot->xAxis, static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &SinglePlotWidget::limitXAxisRange);
}

void SinglePlotWidget::setValues(const QVector<double> &data1, const QVector<double> &data2) {
    histogramType = data2.isEmpty() ? 0 : 1;
    
    double newBinWidth = 1.0;
    if (histogramType == 0 && unitsType == 1) { // Time
        newBinWidth = 1000.0 / 40.0789639 / 30 / 64;
    } else if (histogramType == 1) { // Charge
        if (unitsType == 1) newBinWidth = mVperMIP / ADCUperMIP;
        else if (unitsType == 2) newBinWidth = 1.0 / ADCUperMIP;
    }

    double newMinXValue = minXValue * (newBinWidth / 1.0);
    double step = newBinWidth;

    xData.clear();
    yData1.clear();
    yData2.clear();
    yDataSum.clear();

    for (int j = 0; j < data1.size(); ++j) {
        xData.append(newMinXValue + j * step);
        yData1.append(data1[j]);
        if (!data2.isEmpty() && j < data2.size()) {
            yData2.append(data2[j]);
            yDataSum.append(data1[j] + data2[j]);
        }
    }

    originalXData = xData;
    originalYData1 = yData1;
    originalYData2 = yData2;
    originalYDataSum = yDataSum;
    originalXRange = QCPRange(minXValue, maxXValue);

    bars1->setWidth(newBinWidth);
    bars2->setWidth(newBinWidth);

    setChargeType(chargeType);
    setXAxisType(xAxisType, threshold);
}


void SinglePlotWidget::setInitialRange(double min, double max) {
    minXValue = min;
    maxXValue = max;
    plot->xAxis->setRange(min, max);
    plot->yAxis->setRangeLower(0);
    plot->replot();
}

void SinglePlotWidget::setPlotLabel(double sum, double mean, double rms, double ps, double sum2, double mean2, double rms2) {
    if (chargeType == 3) {
        label->setMinimumHeight(30);
        label->setMaximumHeight(45);
        label->setStyleSheet("font-size: 10pt;");
        label->setText(
            QString::asprintf("Σ=%llu μ=%.2f σ=%.*f\nΣ=%llu μ=%.2f σ=%.*f", 
                static_cast<quint64>(sum), mean, ps == 1. ? 0 : 3, rms * (ps == 1. ? 1e3 : 1),
                static_cast<quint64>(sum2), mean2, ps == 1. ? 0 : 3, rms2 * (ps == 1. ? 1e3 : 1))
        );
    } else {
        label->setMinimumHeight(20);
        label->setMaximumHeight(30);
        label->setStyleSheet("font-size: 13pt;");
        label->setText(
            QString::asprintf("Σ=%llu μ=%.2f σ=%.*f", static_cast<quint64>(sum),
                              mean, ps == 1. ? 0 : 3, rms * (ps == 1. ? 1e3 : 1)));
    }
}

void SinglePlotWidget::setXAxisType(int type, int thresholdParam) {
    xAxisType = type;
    threshold = thresholdParam;

    if (originalXData.isEmpty() || (originalYData1.isEmpty() && originalYDataSum.isEmpty())) return;

    if (type == 1) {
        plot->xAxis->setRange(originalXRange);
    } else if (type == 2) {
        auto trimData = [&](const QVector<double> &data, int &firstIndex, int &lastIndex) {
            if (data.isEmpty()) return false;
            firstIndex = 0;
            lastIndex = data.size() - 1;
            while (firstIndex < lastIndex) {
                if (data[firstIndex] < threshold) firstIndex++;
                else if (data[lastIndex] < threshold) lastIndex--;
                else break;
            }
            return true;
        };

        int firstIndex, lastIndex;
        const auto& yDataToTrim = (histogramType == 0) ? originalYData1 : originalYDataSum;
        if (trimData(yDataToTrim, firstIndex, lastIndex) && lastIndex > firstIndex) {
            plot->xAxis->setRange(originalXData[firstIndex], originalXData[lastIndex]);
        } else {
             plot->xAxis->setRange(originalXRange);
        }
    }
    adjustYAxisRange();
}


void SinglePlotWidget::setYAxisType(int type) {
    yAxisType = type;
    plot->yAxis->setScaleType(type == 1 ? QCPAxis::stLogarithmic : QCPAxis::stLinear);

    QSharedPointer<QCPAxisTicker> linTicker = QSharedPointer<QCPAxisTicker>(new CustomYAxisTicker());
    QSharedPointer<QCPAxisTicker> logTicker = QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog());

    plot->yAxis->setTicker(type == 1 ? logTicker : linTicker);
    adjustYAxisRange();
}

void SinglePlotWidget::setChargeType(int type) {
    chargeType = type;
    QVector<double> emptyVector;

    if (xData.isEmpty()) return;

    if (chargeType == 0) {
        bars1->setData(xData, yData1);
        bars2->setData(emptyVector, emptyVector);
        bars1->setBrush(QBrush(OKcolor.darker(200)));
    } else if (chargeType == 1) {
        bars1->setData(xData, yData2);
        bars2->setData(emptyVector, emptyVector);
        bars1->setBrush(QBrush(OKcolor));
    } else if (chargeType == 2) {
        bars1->setData(xData, yDataSum);
        bars2->setData(xData, yData1);
        bars1->setBrush(QBrush(OKcolor));
        bars2->setBrush(QBrush(OKcolor.darker(200)));
    } else { 
        bars1->setData(xData, yData1);
        bars2->setData(xData, yData2);
        bars1->setBrush(QBrush(OKcolor.darker(200)));
        QColor semiTransparentOK = OKcolor;
        semiTransparentOK.setAlpha(150);
        bars2->setBrush(QBrush(semiTransparentOK));
    }
    adjustYAxisRange();
}

void SinglePlotWidget::setUpdatesAndSignals(bool enabled) {
    plot->setUpdatesEnabled(enabled);
    plot->blockSignals(!enabled);
}

void SinglePlotWidget::adjustYAxisRange() {
    QCPRange xRange = plot->xAxis->range();
    double maxY = 0;
    double mean = 0.0, RMS = 0.0;
    double mean2 = 0.0, RMS2 = 0.0;
    uint64_t sum = 0, sum2 = 0;
    bool hasData = false;
    
    auto processBars = [&](QCPBars* bars, uint64_t& current_sum, double& current_mean, double& current_rms) {
        if (!bars || bars->data()->isEmpty()) return;
        hasData = true;
        for (auto it = bars->data()->constBegin(); it != bars->data()->constEnd(); ++it) {
            if (it->key >= xRange.lower && it->key <= xRange.upper) {
                maxY = qMax(maxY, it->value);
                current_sum += it->value;
                current_mean += it->value * it->key;
                current_rms += it->value * it->key * it->key;
            }
        }
    };

    if (chargeType == 3) {
        processBars(bars1, sum, mean, RMS);
        processBars(bars2, sum2, mean2, RMS2);
    } else {
        processBars(bars1, sum, mean, RMS);
    }

    if (sum != 0) {
        mean /= sum;
        RMS = sqrt(qMax(0.0, (RMS / sum) - pow(mean, 2)));
    }
    if (sum2 != 0) {
        mean2 /= sum2;
        RMS2 = sqrt(qMax(0.0, (RMS2 / sum2) - pow(mean2, 2)));
    }

    setPlotLabel(sum, mean, RMS, psParameter && unitsType == 1, sum2, mean2, RMS2);

    double minY = (yAxisType == 1) ? 0.8 : 0;
    double yLower = hasData ? qMax(minY, plot->yAxis->range().lower) : 0;
    double yUpper = hasData ? maxY * 1.1 : 1;
    if (yLower >= yUpper) yUpper = yLower + 1;

    plot->yAxis->setRange(yLower, yUpper);
    plot->replot();
}

void SinglePlotWidget::limitXAxisRange(const QCPRange &newRange) {
    if (blockAutoRangeAdjust) return;

    QCPRange fixedRange = newRange;
    double minLimit = originalXRange.lower;
    double maxLimit = originalXRange.upper;

    if (fixedRange.lower < minLimit) fixedRange.lower = minLimit;
    if (fixedRange.upper > maxLimit) fixedRange.upper = maxLimit;
    if (fixedRange.size() > (maxLimit - minLimit)) {
        fixedRange = originalXRange;
    }

    blockAutoRangeAdjust = true;
    plot->xAxis->setRange(fixedRange);
    blockAutoRangeAdjust = false;
    
    adjustYAxisRange();
}

void SinglePlotWidget::setTimeUnits(int type) {
    unitsType = type;
    if (!originalYData1.isEmpty()) {
        setValues(originalYData1, originalYData2);
    }
}

void SinglePlotWidget::setChargeUnits(int type) {
    unitsType = type;
    if (!originalYData1.isEmpty()) {
       setValues(originalYData1, originalYData2);
    }
}

void SinglePlotWidget::setChargeUnitsValues(float value, bool isMV) {
    if (isMV) mVperMIP = value;
    else ADCUperMIP = value;
    if (unitsType != 0 && !originalYData1.isEmpty()) {
         setValues(originalYData1, originalYData2);
    }
}

void SinglePlotWidget::setPsParameter(bool ps) {
    psParameter = ps;
    adjustYAxisRange();
}