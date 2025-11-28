#include "SinglePlotWidget.h"
#include "CustomYAxisTicker.h"
#include <QVBoxLayout>
#include <cmath>
#include <QtMath>


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

void SinglePlotWidget::setValues(const QVector<QVector<double>> &data) {

    auto calculateSum = [&](const QVector<QVector<double>> &allDataSets){
        int maxLength = 0;
        for (const auto &vec : allDataSets) {
            if (vec.size() > maxLength) {
                maxLength = vec.size();
            }
        }
        QVector<double> sumResult(maxLength, 0.0);
        for (const auto &vec : allDataSets) {
            for (int i = 0; i < vec.size(); ++i) {
                sumResult[i] += vec[i];
            }
        }
        return sumResult;
    };

    if(data.isEmpty()) return;
    
    double newBinWidth = 1.0;
    if (data.size()==1 && unitsType == 1) { // Time
        newBinWidth = 1000.0 / 40.0789639 / 30 / 64;
    } else if (data.size()==2) { // Charge
        if (unitsType == 1) newBinWidth = mVperMIP / ADCUperMIP;
        else if (unitsType == 2) newBinWidth = 1.0 / ADCUperMIP;
    }

    double newMinXValue = minXValue * (newBinWidth / 1.0);
    double step = newBinWidth;

    xData.clear();
    yData.clear();

    for(int i = 0; i < data.size(); ++i){
        QVector<double> assignedYData;
        for (int j = 0; j < data[i].size(); ++j) {
            xData.append(newMinXValue + j * step);
            assignedYData.append(data[i][j]);
        }
        yData.append(assignedYData);
    }

    originalXData = xData;
    originalYData = yData;
    yDataSum = calculateSum(data);
    originalXRange = QCPRange(minXValue, maxXValue);

    bars1->setWidth(newBinWidth);
    bars2->setWidth(newBinWidth);

    setChargeType(chargeType);
    setXAxisType(xAxisType, threshold, minBin, maxBin);
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

void SinglePlotWidget::setXAxisType(int type, int thresholdParam, int minBin, int maxBin) {
    xAxisType = type;
    threshold = thresholdParam;

    if (originalXData.isEmpty() || originalYData.isEmpty() || originalYData[0].isEmpty()) return;

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
        const auto& yDataToTrim = (histogramType == 0) ? originalYData[0] : originalYData[1];
        if (trimData(yDataToTrim, firstIndex, lastIndex) && lastIndex > firstIndex) {
            plot->xAxis->setRange(originalXData[firstIndex], originalXData[lastIndex]);
        } else {
             plot->xAxis->setRange(originalXRange);
        }
    } else if (type == 3){ // for gauss fit display 
        plot->xAxis->setRange(minBin, maxBin);
    }
    if(type==1){
        plot->axisRect()->setRangeDrag(Qt::Orientations(0));
        plot->axisRect()->setRangeZoom(Qt::Orientations(0));
    }
    else{
        plot->axisRect()->setRangeDrag(Qt::Horizontal);
        plot->axisRect()->setRangeZoom(Qt::Horizontal);
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
        bars1->setData(xData, yData[0]);
        bars2->setData(emptyVector, emptyVector);
        bars1->setBrush(QBrush(OKcolor.darker(200)));
    } else if (chargeType == 1) {
        bars1->setData(xData, yData[1]);
        bars2->setData(emptyVector, emptyVector);
        bars1->setBrush(QBrush(OKcolor));
    } else if (chargeType == 2) {
        bars1->setData(xData, yDataSum);
        bars2->setData(xData, yData[0]);
        bars1->setBrush(QBrush(OKcolor));
        bars2->setBrush(QBrush(OKcolor.darker(200)));
    } else { 
        bars1->setData(xData, yData[0]);
        bars2->setData(xData, yData[1]);
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
        bool firstIter=true;
        for (auto it = bars->data()->constBegin(); it != bars->data()->constEnd(); ++it) {
            if(firstIter){
                firstIter=false;
            }
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
    if (!originalYData.isEmpty()) {
        setValues(originalYData);
    }
}

void SinglePlotWidget::setChargeUnits(int type) {
    unitsType = type;
    if (!originalYData.isEmpty()) {
       setValues(originalYData);
    }
}

void SinglePlotWidget::setChargeUnitsValues(float value, bool isMV) {
    if (isMV) mVperMIP = value;
    else ADCUperMIP = value;
    if (unitsType != 0 && !originalYData.isEmpty()) {
        setValues(originalYData);
    }
}

void SinglePlotWidget::setPsParameter(bool ps) {
    psParameter = ps;
    adjustYAxisRange();
}

void SinglePlotWidget::setFitRange(double min, double max)
{
    fitMin = min;
    fitMax = max;
}

void SinglePlotWidget::fitHistogram()
{
    if (xData.isEmpty() || yData.isEmpty())
        return;

    // --- compute mean and sigma (moments inside fit range)
    double sum = 0, sumX = 0, sumX2 = 0;
    for (int i = 0; i < xData.size(); ++i) {
        double x = xData[i];
        if (x < fitMin || x > fitMax) continue;

        double y = yData[0][i];
        sum   += y;
        sumX  += x * y;
        sumX2 += x * x * y;
    }

    if (sum <= 0) return;

    double mean  = sumX / sum;
    double sigma = qSqrt(sumX2 / sum - mean * mean);

    // --- Gaussian curve points
    QVector<double> gaussX, gaussY;
    double maxY = *std::max_element(yData[0].begin(), yData[0].end());
    for (int i = 0; i < 200; ++i) {
        double x = fitMin + i * (fitMax - fitMin) / 200.0;
        double y = maxY * qExp(-0.5 * qPow((x - mean) / sigma, 2));
        gaussX.push_back(x);
        gaussY.push_back(y);
    }

    // --- Background linear fit (exclude ±2σ region around peak)
    double excludeMin = mean - 2*sigma;
    double excludeMax = mean + 2*sigma;

    double S=0, Sx=0, Sy=0, Sxx=0, Sxy=0;
    for (int i=0; i<xData.size(); ++i) {
        double x = xData[i];
        if (x < fitMin || x > fitMax) continue;
        if (x > excludeMin && x < excludeMax) continue; // skip peak region

        double y = yData[0][i];
        S   += 1;
        Sx  += x;
        Sy  += y;
        Sxx += x*x;
        Sxy += x*y;
    }

    double p0=0, p1=0;
    double denom = (S*Sxx - Sx*Sx);
    if (fabs(denom) > 1e-12) {
        p0 = (Sy*Sxx - Sx*Sxy) / denom;
        p1 = (S*Sxy - Sx*Sy) / denom;
    }

    // --- Background line points
    QVector<double> backX, backY;
    for (int i=0; i<200; ++i) {
        double x = fitMin + i*(fitMax - fitMin)/200.0;
        double y = p0 + p1*x;
        backX.push_back(x);
        backY.push_back(y);
    }

    // --- Draw Gaussian (red)
    if (!fitGraph) {
        fitGraph = plot->addGraph();
        fitGraph->setPen(QPen(Qt::red, 2));
    }
    fitGraph->setData(gaussX, gaussY);

    // --- Draw Background (blue dashed)
    if (!backGraph) {
        backGraph = plot->addGraph();
        QPen pen(Qt::blue, 2, Qt::DashLine);
        backGraph->setPen(pen);
    }
    backGraph->setData(backX, backY);

    plot->replot();

    // --- Show results in label
    label->setText(QString("Σ=%.0f  μ=%.2f  σ=%.2f  p0=%.2f  p1=%.3f")
                   .arg(sum).arg(mean).arg(sigma).arg(p0).arg(p1));
}
