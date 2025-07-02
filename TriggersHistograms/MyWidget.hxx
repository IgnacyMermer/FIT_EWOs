#ifndef MYWIDGET_HXX
#define MYWIDGET_HXX

#include <QWidget>
#include <QVector>
#include "qcustomplot.h"

class MyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MyWidget(QWidget *parent = nullptr);

    void setValues(const QVector<double>& newValues, int datasetIndex = 0);
    void setRange(double min, double max);
    void clearValues(int datasetIndex = 0);

    QBrush getBrush() const;
    void setBrush(const QBrush &brush);
    void MyWidget::updateAllPlots();
    void MyWidget::adjustYAxisRange();
    void MyWidget::setXAxisType(int type, int threshold);
    void MyWidget::setYAxisType(int type);
    void MyWidget::setPlotLabel(double index, double sum, double mean, double rms, double ps);

private slots:
    void MyWidget::syncXAxisRange(const QCPRange &newRange);
    void MyWidget::limitXAxisRange(const QCPRange &newRange);

private:
    QVector<QVector<double>> valuesList;
    QList<QCustomPlot*> plots;

    QCustomPlot *customPlot;
    QVector<QCPBars*> barsList;
    QVector<QLabel*> labels;
    QVector<QVector<double>> xDataValues, yDataValues, originalXDataValues, originalYDataValues;
    int minXValue, maxXValue;
    int yAxisType;

};

#endif // MYWIDGET_HXX