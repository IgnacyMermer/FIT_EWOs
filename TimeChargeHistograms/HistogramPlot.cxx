#include "HistogramPlot.hxx"
#include "SinglePlotWidget.h"
#include <QGridLayout>

HistogramPlot::HistogramPlot(QWidget *parent) : QWidget(parent) {
    QGridLayout *layout = new QGridLayout(this);

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 4; ++col) {
            SinglePlotWidget *plotWidget = new SinglePlotWidget(this);
            layout->addWidget(plotWidget, row, col);
            plotWidgets.append(plotWidget);
        }
    }
}

void HistogramPlot::setValues(const QVector<double> &newValues) {
    int numPlots = plotWidgets.size();
    if (numPlots == 0) return;

    int pointsPerPlot = maxXValue - minXValue + 1;
    if (pointsPerPlot <= 0) return;

    if (newValues.size() < 2 * numPlots * pointsPerPlot) {
        for (int i = 0; i < numPlots; ++i) {
            int startIndex = i * pointsPerPlot;
            if (startIndex + pointsPerPlot <= newValues.size()) {
                const QVector<double> data1 = newValues.mid(startIndex, pointsPerPlot);
                plotWidgets[i]->setValues(data1);
            }
        }
    } 
    else {
        for (int i = 0; i < numPlots; ++i) {
            int startIndex1 = i * pointsPerPlot;
            int startIndex2 = (i + numPlots) * pointsPerPlot;
            if (startIndex2 + pointsPerPlot <= newValues.size()) {
                const QVector<double> data1 = newValues.mid(startIndex1, pointsPerPlot);
                const QVector<double> data2 = newValues.mid(startIndex2, pointsPerPlot);
                plotWidgets[i]->setValues(data1, data2);
            }
        }
    }
}

void HistogramPlot::setRange(double min, double max) {
    minXValue = min;
    maxXValue = max;
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setInitialRange(min, max);
    }
}

void HistogramPlot::setXAxisType(int type, int thresholdParam, int minBin, int maxBin) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setXAxisType(type, thresholdParam, minBin, maxBin);
    }
}

void HistogramPlot::setYAxisType(int type) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setYAxisType(type);
    }
}

void HistogramPlot::setChargeType(int type) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setChargeType(type);
    }
}

void HistogramPlot::setTimeUnits(int type) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setTimeUnits(type);
    }
}

void HistogramPlot::setChargeUnits(int type) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setChargeUnits(type);
    }
}

void HistogramPlot::setChargeUnitsValues(float value, bool isMV) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setChargeUnitsValues(value, isMV);
    }
}

void HistogramPlot::setPsParameter(bool ps) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setPsParameter(ps);
    }
}

void HistogramPlot::setPlotLabel(int index, double sum, double mean, double rms,
                            double ps, double sum2, double mean2, double rms2) {
    if (index >= 0 && index < plotWidgets.size()) {
        plotWidgets[index]->setPlotLabel(sum, mean, rms, ps, sum2, mean2, rms2);
    }
}

void HistogramPlot::setFitRange(double min, double max) {
    for (SinglePlotWidget *widget : plotWidgets) { 
        widget->setFitRange(min,max);
    }
}

void HistogramPlot::fitHistogram() {
    for (SinglePlotWidget *widget : plotWidgets) {
        if (widget) {
            widget->fitHistogram();
        }
    }
}

void HistogramPlot::applyFullConfiguration(const QVector<double> &newValues,
                                      int xAxisTypeParam, int thresholdParam,
                                      int yAxisTypeParam, int chargeTypeParam,
                                      int timeUnitTypeParam, int chargeUnitTypeParam,
                                      float chargeUnitValueMVParam, float chargeUnitValueADCParam,
                                      bool psParam) {
    for (SinglePlotWidget* widget : plotWidgets) {
        widget->setUpdatesAndSignals(false);
    }

    setChargeUnitsValues(chargeUnitValueMVParam, true);
    setChargeUnitsValues(chargeUnitValueADCParam, false);
    setTimeUnits(timeUnitTypeParam);
    setChargeUnits(chargeUnitTypeParam);
    setYAxisType(yAxisTypeParam);
    setPsParameter(psParam);
    setChargeType(chargeTypeParam);
    setValues(newValues);
    setXAxisType(xAxisTypeParam, thresholdParam);

    for (SinglePlotWidget* widget : plotWidgets) {
        widget->setUpdatesAndSignals(true);
        widget->adjustYAxisRange();
    }
}