#ifndef HISTOGRAMPLOT_HXX
#define HISTOGRAMPLOT_HXX

#include <QWidget>
#include <QVector>

class SinglePlotWidget;

class HistogramPlot : public QWidget {
    Q_OBJECT

public:
    explicit HistogramPlot(QWidget *parent = nullptr);

    void setValues(const QVector<double> &newValues);
    void setRange(double min, double max);
    void setXAxisType(int type, int thresholdParam, int minBin=0, int maxBin=0);
    void setYAxisType(int type);
    void setTimeUnits(int type);
    void setChargeUnits(int type);
    void setChargeType(int type);
    void setPsParameter(bool ps);
    void setChargeUnitsValues(float value, bool isMV);

    void setPlotLabel(int index, double sum, double mean, double rms,
                      double ps, double sum2 = 0.0, double mean2 = 0.0, 
                      double rms2 = 0.0);
    void setFitRange(double min, double max);
    void fitHistogram();
    void applyFullConfiguration(const QVector<double> &newValues,
                                int xAxisTypeParam, int thresholdParam,
                                int yAxisTypeParam, int chargeTypeParam,
                                int timeUnitTypeParam, int chargeUnitTypeParam,
                                float chargeUnitValueMVParam, float chargeUnitValueADCParam,
                                bool psParam);
    

private:
    QList<SinglePlotWidget *> plotWidgets;
    int minXValue = 0, maxXValue = 15;
    bool doFitFlag = false;

};

#endif // HISTOGRAMPLOT_HXX