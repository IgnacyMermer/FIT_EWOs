#ifndef MYWIDGET_HXX
#define MYWIDGET_HXX

#include "qcustomplot.h"
#include <QVector>
#include <QWidget>

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
  void setPlotLabel(double index, double sum, double mean, double rms,
                              double ps, double sum2=0.0, double mean2=0.0, 
                              double rms2=0.0);
  void setChargeUnitsValues(float value, bool isMV); 
  void applyFullConfiguration(const QVector<double> &newValues,
                                      int xAxisTypeParam,
                                      int thresholdParam,
                                      int yAxisTypeParam,
                                      int chargeTypeParam,
                                      int timeUnitTypeParam,
                                      int chargeUnitTypeParam,
                                      float chargeUnitValueMVParam,
                                      float chargeUnitValueADCParam,
                                      bool psParam);

private slots:
  void adjustYAxisRange();
  void limitXAxisRange(const QCPRange &newRange);

private:
  QVector<QVector<double>> valuesList;
  QList<QCustomPlot *> plots;
  int xRangeType, chargeType, yAxisType, xAxisType, threshold;
  int unitsType;
  bool blockAutoRangeAdjust = false;
  double binWidth = 1.0;
  bool suppressAutoAdjust=false, psParameter=false;
  int histogramType = 0; // 0 = time, 1 = charge

  QCustomPlot *customPlot;
  QVector<QCPBars *> barsList1, barsList2;
  QVector<QLabel *> labels;
  QVector<QVector<double>> xDataValues, yDataValues1, yDataValues2,
      yDataValuesSum;
  QVector<QVector<double>> originalXDataValues, originalYDataValues1,
      originalYDataValues2, originalYDataValuesSum;
  QVector<double> yRangeValues;
  int minXValue, maxXValue;
  float mVperMIP=7.5, ADCUperMIP=16.0;
  QVector<QCPRange> originalXRanges;

};

#endif // MYWIDGET_HXX