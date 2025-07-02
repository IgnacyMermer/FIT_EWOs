#ifndef MYWIDGET_HXX
#define MYWIDGET_HXX

#include "qcustomplot.h"
#include <QVector>
#include <QWidget>

class MyWidget : public QCustomPlot {
  Q_OBJECT
public:
  explicit MyWidget(QWidget *parent = nullptr);

  void setInitialSteps(int steps);
  void setHistogramLine(int i, int j, QVector<quint32> x);
  void addPoint(double x, double y, bool dots = false);
  void setTitles(const std::array<double, 3>& adcs);
  void setAxisRange(double xMin, double xMax, double yMin, double yMax);
  void rescaleDataRanges();
  void clear();

private slots:

private:
  QVector<QVector<double>> valuesList;
  QList<QCustomPlot *> plots;
  int xRangeType, chargeType, yAxisType;
  int chargeUnitsType;
  double binWidth = 1.0;
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


  QList<QCPColorMap* > _colorMapsTime;
  QList<QCPAxisRect* > _axisRects;
  QVector<double> _x0;
  QVector<double> _y0;
  QVector<double> _x1;
  QVector<double> _y1;
};

#endif // MYWIDGET_HXX