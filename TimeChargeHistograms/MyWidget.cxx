#include "MyWidget.hxx"
#include "SinglePlotWidget.h"
#include <QGridLayout>

MyWidget::MyWidget(QWidget *parent) : QWidget(parent) {
    QGridLayout *layout = new QGridLayout(this);

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 4; ++col) {
            SinglePlotWidget *plotWidget = new SinglePlotWidget(this);
            layout->addWidget(plotWidget, row, col);
            plotWidgets.append(plotWidget);
        }
    }
}

void MyWidget::setValues(const QVector<double> &newValues) {
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

void MyWidget::setRange(double min, double max) {
    minXValue = min;
    maxXValue = max;
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setInitialRange(min, max);
    }
}

void MyWidget::setXAxisType(int type, int thresholdParam) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setXAxisType(type, thresholdParam);
    }
}

void MyWidget::setYAxisType(int type) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setYAxisType(type);
    }
}

void MyWidget::setChargeType(int type) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setChargeType(type);
    }
}

void MyWidget::setTimeUnits(int type) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setTimeUnits(type);
    }
}

void MyWidget::setChargeUnits(int type) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setChargeUnits(type);
    }
}

void MyWidget::setChargeUnitsValues(float value, bool isMV) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setChargeUnitsValues(value, isMV);
    }
}

void MyWidget::setPsParameter(bool ps) {
    for (SinglePlotWidget *widget : plotWidgets) {
        widget->setPsParameter(ps);
    }
}

void MyWidget::setPlotLabel(int index, double sum, double mean, double rms,
                            double ps, double sum2, double mean2, double rms2) {
    if (index >= 0 && index < plotWidgets.size()) {
        plotWidgets[index]->setPlotLabel(sum, mean, rms, ps, sum2, mean2, rms2);
    }
}


void MyWidget::applyFullConfiguration(const QVector<double> &newValues,
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