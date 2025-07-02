#include "MyWidget.hxx"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QFont>
#include <QDebug>
#include <cmath>
#include <QRandomGenerator>
#include "qcustomplot.h"

static inline double roundDownOrder(double v) { return pow(10, floor(log10(fabs(v)))); }
static inline double roundUpOrder(double v) { return pow(10, floor(log10(fabs(v)) + (v > 0 ? 1 : 0))); }
static inline double roundUpPlace(double v) { double b = roundDownOrder(v); return (floor(v / b) + (v > 0 ? 1 : -1)) * b; }
static inline double roundUpStep(double v) { double b = roundDownOrder(v), r = v / b; return r > 2 ? (r > 5 ? 10 * b : 5 * b) : 2 * b; }

const QColor OKcolor(0xb0d959);

MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
{

    QVBoxLayout *layout = new QVBoxLayout(this);
    minXValue = 0;
    maxXValue = 15;
    for (int i = 0; i < 3; ++i) {
        
        QLabel *label = new QLabel(this);
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        label->setMinimumHeight(20);
        label->setMaximumHeight(30);
        layout->addWidget(label);
        labels.append(label);

        QCustomPlot *customPlot = new QCustomPlot(this);
        layout->addWidget(customPlot);

        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        customPlot->axisRect()->setRangeDrag(Qt::Horizontal);
        customPlot->axisRect()->setRangeZoom(Qt::Horizontal);
        customPlot->xAxis->setTickLabelFont(QFont("Arial", 8));
        customPlot->xAxis->setRange(0, 15);

        QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
        intTicker->setTickStep(1.0);
        intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
        customPlot->xAxis->setTicker(intTicker);
        customPlot->xAxis->setNumberPrecision(0);
        customPlot->xAxis->setNumberFormat("f");
        customPlot->xAxis->ticker()->setTickCount(5);
        customPlot->yAxis->setScaleType(QCPAxis::stLinear);
        customPlot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
        customPlot->yAxis->ticker()->setTickCount(10);
        
        QCPAxisRect *axisRect = customPlot->axisRect();
        axisRect->setMargins(QMargins(50, 20, 20, 50));
        axisRect->setupFullAxesBox();

        QCPBars *bars = new QCPBars(customPlot->xAxis, customPlot->yAxis);
        bars->setPen(QPen(OKcolor.darker(400)));
        bars->setBrush(QBrush(OKcolor.darker(200)));
        bars->setWidth(1);

        barsList.append(bars);
        plots.append(customPlot);
    }

    valuesList.resize(3);

    for (QCustomPlot* plot : plots) {
        //connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(adjustYAxisRange()));
        //connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(limitXAxisRange(QCPRange)));
        connect(plot->xAxis, static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), 
        this, &MyWidget::limitXAxisRange);
    }
}

void MyWidget::setPlotLabel(double index, double sum, double mean, double rms, double ps)
{
    if (index >= 0 && index < labels.size()) {
        labels[index]->setText(QString::asprintf("Σ=%llu μ=%.2f σ=%.*f",
                                                static_cast<quint64>(sum), mean, 
                                                ps==1. ? 0 : 3, rms * (ps==1. ? 1e3 : 1)));
    }
}

QBrush MyWidget::getBrush() const
{
    return palette().brush(backgroundRole());
}

void MyWidget::setBrush(const QBrush &brush)
{
    QPalette pal = palette();
    pal.setBrush(backgroundRole(), brush);
    setPalette(pal);
}

void MyWidget::setValues(const QVector<double>& newValues, int datasetIndex)
{
    QVector<QVector<double>> xDataList(3);
    QVector<QVector<double>> yDataList(3);
    QVector<double> yRangeValues(3);

    for(int z = 0; z < 3; z++) {
        int maxYValue = 1;

        //if (z >= valuesList.size()) {
            if(z==0&&newValues.size()<10000){
                valuesList[z] = QVector<double>(maxXValue - minXValue + 1, 0.0);
            }
            else{    
                valuesList[z] = newValues.mid((newValues.size()<10000?(z-1):z) * (maxXValue-minXValue+1), (maxXValue-minXValue+1));
            }
        /*} else {
            QVector<double>& current = valuesList[z];
            if(z==0&&newValues.size()<10000){
                current = QVector<double>(maxXValue - minXValue + 1, 0.0);
            }
            else{    
                valuesList[z] = newValues.mid(z * (maxXValue-minXValue+1), (maxXValue-minXValue+1));
            }
            for (int i = 0; i < (maxXValue-minXValue+1); ++i) {
                if (i < current.size()){
                    current[i] = newValues[z * (maxXValue-minXValue+1) + i];
                } else {
                    current.append(newValues[z * (maxXValue-minXValue+1) + i]);
                }
                maxYValue = qMax<double>(maxYValue, current[i]);
            }
        }*/

        yRangeValues[z] = maxYValue * 1.1;
        
        QVector<double> xData, yData;
        const QVector<double>& data = valuesList[z];

        double step = data.size() > 1 ? (maxXValue-minXValue+1) / (data.size() - 1) : 1;
        for (int i = 0; i < data.size(); ++i) {
            xData.append(i * step);
            yData.append(data[i]);
        }

        xDataList[z] = xData;
        yDataList[z] = yData;
    }

    originalXDataValues = xDataList;
    originalYDataValues = yDataList;

    xDataValues=xDataList;
    yDataValues=yDataList;

    for (int i = 0; i < 3; ++i) {
        barsList[i]->setData(xDataList[i], yDataList[i]);
        //plots[i]->yAxis->setRange(0, yRangeValues[i]);
        //plots[i]->replot();
    }
    adjustYAxisRange();
}

void MyWidget::setRange(double min, double max)
{
    minXValue = min;
    maxXValue = max;
    for (QCustomPlot* plot : plots) {
        plot->xAxis->setRange(min, max);
        plot->replot();
    }
}

void MyWidget::clearValues(int datasetIndex)
{
    if (datasetIndex < valuesList.size()) {
        valuesList[datasetIndex].clear();
        QVector<double> xData, yData;
        for (int i = 0; i < plots.size(); ++i) {
            barsList[i]->setData(xData, yData);
            plots[i]->replot();
        }
    }
}

void MyWidget::syncXAxisRange(const QCPRange &newRange)
{
    for (int i = 1; i < plots.size(); ++i) {
        plots[i]->blockSignals(true);
        plots[i]->xAxis->setRange(newRange);
        plots[i]->blockSignals(false);
        plots[i]->replot();
    }

    updateAllPlots();
}

class CustomYAxisTicker : public QCPAxisTickerFixed {
public:
    QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) override {
        Q_UNUSED(locale)
        Q_UNUSED(formatChar)
        Q_UNUSED(precision)
        return fabs(tick) < 999.5 ? QString::asprintf("%3.0f", tick) : 
                                    QString::asprintf("%.0e", tick).replace("e+0", "e");
    }

    double getTickStep(const QCPRange &range) override {
        return roundDownOrder(range.upper * 0.9);
    }

    int getSubTickCount(double tickStep) override {
        return tickStep > 1 ? 9 : 0;
    }

private:
    double roundDownOrder(double value) {
        return pow(10, floor(log10(value)));
    }
};

class xAxisTicker : public QCPAxisTickerFixed {
public:
    int &fontSize_px, &axisLength_px;

    xAxisTicker(int &_fontSize_px, int &_axisLength_px)
        : fontSize_px(_fontSize_px), axisLength_px(_axisLength_px) {} 

    double getTickStep(const QCPRange &range) override {
        return range.size() > axisLength_px / fontSize_px 
            ? roundUpStep(range.size() * fontSize_px / axisLength_px) 
            : 1;
    }

    int getSubTickCount(double tickStep) override {
        return tickStep / roundDownOrder(tickStep) - 1;
    }

private:
    double roundDownOrder(double value) {
        return pow(10, floor(log10(value)));
    }

    double roundUpStep(double value) {
        return pow(10, ceil(log10(value)));
    }
};

void MyWidget::setYAxisType(int type) {
    yAxisType=type;
    for (int i = 0; i < plots.size(); ++i) {
        plots[i]->yAxis->setScaleType(type == 1 ? QCPAxis::stLogarithmic : QCPAxis::stLinear);

        QSharedPointer<QCPAxisTicker> linTicker = QSharedPointer<QCPAxisTicker>(new CustomYAxisTicker());
        QSharedPointer<QCPAxisTicker> logTicker = QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog());

        plots[i]->yAxis->setTicker(type == 1 ? logTicker : linTicker);
    }
    adjustYAxisRange();
}

void MyWidget::updateAllPlots()
{
    for (int datasetIndex = 0; datasetIndex < valuesList.size(); ++datasetIndex) {
        QVector<double> xData, yData;
        const QVector<double>& data = valuesList[datasetIndex];

        QCPRange xRange = plots[0]->xAxis->range();
        if (data.size() > 1) {
            double step = (maxXValue-minXValue+1) / (data.size() - 1);
            for (int i = 0; i < data.size(); ++i) {
                xData.append(0 + i * step);
                yData.append(data[i]);
            }
        } else if (data.size() == 1) {
            xData.append(0);
            yData.append(data[0]);
        }

        for (int i = 0; i < plots.size(); ++i) {
            barsList[i]->setData(xData, yData);
            plots[i]->replot();
        }
    }
}

void MyWidget::adjustYAxisRange()
{
    for (int i = 0; i < plots.size(); ++i) {
        QCustomPlot *plot = plots[i];
        QCPRange xRange = plot->xAxis->range();

        double minY = std::numeric_limits<double>::max();
        double maxY = std::numeric_limits<double>::lowest();
        double mean=0.0, RMS=0.0;
        uint64_t sum=0; 

        QCPBars *bars = barsList[i];
        QSharedPointer<QCPDataContainer<QCPBarsData>> dataContainer = bars->data();

        if (dataContainer->size() == 0) {
            continue;
        }

        for (auto it = bars->data()->constBegin(); it != bars->data()->constEnd(); ++it) {
            double x = it->key;
            double y = it->value;

            if (x >= xRange.lower && x <= xRange.upper) {
                minY = qMin(minY, y);
                maxY = qMax(maxY, y);
                sum+=y;
                mean+=y*x;
                RMS+=y*x*x;
            }
        }

        if (minY == std::numeric_limits<double>::max() || maxY == std::numeric_limits<double>::lowest()) {
            continue;
        }

        if(sum!=0){
            mean=mean/sum;
            RMS=sqrt((RMS/sum)-pow(mean,2));
        }
        setPlotLabel(i, sum, mean, RMS, 0);

        double newMinY = qMax(0.0, minY - (maxY - minY) * 0.1);
        double newMaxY = maxY + (maxY - minY) * 0.1;

        plot->yAxis->setRange(newMinY, newMaxY);
        plot->replot();
    }
}

void MyWidget::setXAxisType(int type, int threshold) {
    if(originalXDataValues.size()>0&&originalXDataValues[0].size()>0){
        switch (type) {
            case 0:
                xDataValues = originalXDataValues;
                yDataValues = originalYDataValues;
                for (int i = 0; i < plots.size(); ++i) {
                    barsList[i]->setData(xDataValues[i], yDataValues[i]);
                    plots[i]->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
                }
                break;
            case 1:
                xDataValues = originalXDataValues;
                yDataValues = originalYDataValues;
                for (int i = 0; i < plots.size(); ++i) {
                    plots[i]->setInteractions(QCP::iNone);
                    plots[i]->xAxis->setRange(minXValue, maxXValue);
                }
                break;
            case 2:
                auto trimData = [&](const QVector<double> &data, int &firstIndex,
                    int &lastIndex) -> bool {
                    if (data.isEmpty()) {
                        return false; // Return false if data is empty
                    }

                    firstIndex = 0;
                    lastIndex = data.size() - 1;
                    while (firstIndex < lastIndex) {
                        if (data[firstIndex] < threshold) {
                            firstIndex++;
                        } else if (data[lastIndex] < threshold) {
                            lastIndex--;
                        } else {
                            break;
                        }
                    }
                    return true; // Valid data was found
                };
                for (int i = 0; i < plots.size(); ++i) {
                    int firstIndex, lastIndex;
                    if (!trimData(originalYDataValues[i], firstIndex, lastIndex)) {
                        continue; // Skip if empty
                    }
                    xDataValues[i] = originalXDataValues[i].mid(firstIndex, lastIndex - firstIndex + 1);
                    yDataValues[i] = originalYDataValues[i].mid(firstIndex, lastIndex - firstIndex + 1);

                    barsList[i]->setData(xDataValues[i], yDataValues[i]);

                    if (!originalXDataValues[i].isEmpty()) {
                        plots[i]->xAxis->setRange(originalXDataValues[i][firstIndex], originalXDataValues[i][lastIndex]);
                    }
                }
                
                break;
        }
        adjustYAxisRange();
    }
}

void MyWidget::limitXAxisRange(const QCPRange &newRange)
{
    double minLimit = 0;
    double maxLimit = 3564;

    QCPAxis* senderAxis = qobject_cast<QCPAxis*>(sender());
    if (!senderAxis) return;

    QCustomPlot* senderPlot = senderAxis->parentPlot();
    if (!senderPlot) return;

    QCPRange fixedRange = newRange;

    if (fixedRange.lower < minLimit)
        fixedRange.lower = minLimit;
    if (fixedRange.upper > maxLimit)
        fixedRange.upper = maxLimit;

    if (fixedRange.size() > (maxLimit - minLimit)) {
        fixedRange.lower = minLimit;
        fixedRange.upper = maxLimit;
    }

    senderPlot->blockSignals(true);
    senderPlot->xAxis->setRange(fixedRange);
    senderPlot->blockSignals(false);
    senderPlot->replot();
    adjustYAxisRange();
}
