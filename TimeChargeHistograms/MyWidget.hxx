#ifndef MYWIDGET_HXX
#define MYWIDGET_HXX

#include <QWidget>
#include <QVector>

class SinglePlotWidget;

class MyWidget : public QWidget {
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = nullptr);

    void setValues(const QVector<double> &newValues);
    void setRange(double min, double max);
    void setXAxisType(int type, int thresholdParam);
    void setYAxisType(int type);
    void setTimeUnits(int type);
    void setChargeUnits(int type);
    void setChargeType(int type);
    void setPsParameter(bool ps);
    void setChargeUnitsValues(float value, bool isMV);

    void setPlotLabel(int index, double sum, double mean, double rms,
                      double ps, double sum2 = 0.0, double mean2 = 0.0, 
                      double rms2 = 0.0);
    
    void applyFullConfiguration(const QVector<double> &newValues,
                                int xAxisTypeParam, int thresholdParam,
                                int yAxisTypeParam, int chargeTypeParam,
                                int timeUnitTypeParam, int chargeUnitTypeParam,
                                float chargeUnitValueMVParam, float chargeUnitValueADCParam,
                                bool psParam);

private:
    QList<SinglePlotWidget *> plotWidgets;
    int minXValue = 0, maxXValue = 15;
};

#endif // MYWIDGET_HXX