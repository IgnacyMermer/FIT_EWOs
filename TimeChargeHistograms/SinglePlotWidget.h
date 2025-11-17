#ifndef SINGLEPLOTWIDGET_H
#define SINGLEPLOTWIDGET_H

#include "qcustomplot.h"
#include <QWidget>
#include <QVector>
#include <QLabel>
class SinglePlotWidget : public QWidget {
    Q_OBJECT

public:
    explicit SinglePlotWidget(QWidget *parent = nullptr);

    void setValues(const QVector<double> &data1, const QVector<double> &data2 = QVector<double>());
    void setInitialRange(double min, double max);
    void setXAxisType(int type, int threshold, int minBin=0, int maxBin=0);
    void setYAxisType(int type);
    void setTimeUnits(int type);
    void setChargeUnits(int type);
    void setChargeType(int type);
    void setPsParameter(bool ps);
    void setChargeUnitsValues(float value, bool isMV);
    void adjustYAxisRange();
    void setUpdatesAndSignals(bool enabled);
    void setFitRange(double min, double max);
    void fitHistogram();
    void setPlotLabel(double sum, double mean, double rms, double ps, 
                      double sum2 = 0.0, double mean2 = 0.0, double rms2 = 0.0);

private slots:
    void limitXAxisRange(const QCPRange &newRange);

private:
    
    QLabel *label;
    QCustomPlot *plot;
    QCPBars *bars1;
    QCPBars *bars2;
    QCPGraph* fitGraph = nullptr;
    QCPGraph* backGraph = nullptr;
    int chargeType = 2;
    int yAxisType = 0;
    int xAxisType = 0;
    int unitsType = 0;
    int histogramType = 0; // 0 = time, 1 = charge
    int threshold = 0, minBin = 0, maxBin = 0;
    bool psParameter = false;
    bool blockAutoRangeAdjust = false;
    double minXValue = 0, maxXValue = 15;
    float mVperMIP = 7.5, ADCUperMIP = 16.0;
    double fitMin = 0.0;
    double fitMax = 0.0;
    QVector<double> xData, yData1, yData2, yDataSum;
    QVector<double> originalXData, originalYData1, originalYData2, originalYDataSum;
    QCPRange originalXRange;
};

#endif // SINGLEPLOTWIDGET_H