#include "MyWidget.hxx"

#include "qcustomplot.h"
#include <QDebug>
#include <QFont>
#include <QGridLayout>
#include <QPainter>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>
#include "CustomYAxisTicker.h"

const QColor OKcolor(0xb0d959);

MyWidget::MyWidget(QWidget *parent) : QWidget(parent) {
  QGridLayout *layout = new QGridLayout(this);
  minXValue = 0;
  maxXValue = 15;
  chargeType = 2;
  unitsType=0;
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 4; ++col) {
      int gridRow = row * 2;
      QLabel *label = new QLabel(this);
      label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
      label->setMinimumHeight(20);
      label->setMaximumHeight(30);
      layout->addWidget(label, gridRow, col);
      labels.append(label);

      QCustomPlot *customPlot = new QCustomPlot(this);
      layout->addWidget(customPlot, gridRow + 1, col);

      customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
      customPlot->axisRect()->setRangeDrag(Qt::Horizontal);
      customPlot->axisRect()->setRangeZoom(Qt::Horizontal);
      customPlot->xAxis->setTickLabelFont(QFont("Arial", 5));
      customPlot->xAxis->setRange(0, 15);
      QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
      intTicker->setTickStep(1.0);
      intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
      customPlot->xAxis->setTicker(intTicker);
      customPlot->xAxis->setNumberPrecision(0);
      customPlot->xAxis->setNumberFormat("f");
      //customPlot->xAxis->setTickLabelRotation(-90);
      customPlot->xAxis->ticker()->setTickCount(5);
      customPlot->yAxis->setScaleType(QCPAxis::stLinear);
      customPlot->yAxis->ticker()->setTickStepStrategy(
          QCPAxisTicker::tssMeetTickCount);
      customPlot->yAxis->ticker()->setTickCount(10);
      customPlot->yAxis->setTickLabelFont(QFont("Arial", 5));

      QCPAxisRect *axisRect = customPlot->axisRect();
      axisRect->setMargins(QMargins(50, 20, 20, 50));
      axisRect->setupFullAxesBox();

      QCPBars *bars1 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
      bars1->setPen(QPen(OKcolor.darker(400)));
      bars1->setBrush(QBrush(OKcolor));
      bars1->setData(QVector<double>(), QVector<double>());
      bars1->setWidth(1);
      barsList1.append(bars1);

      QCPBars *bars2 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
      bars2->setPen(QPen(OKcolor.darker(400)));
      bars2->setBrush(QBrush(OKcolor.darker(200)));
      bars2->setData(QVector<double>(), QVector<double>());
      bars2->setWidth(1);
      barsList2.append(bars2);

      plots.append(customPlot);
    }
  }
  for (QCustomPlot *plot : plots) {
    connect(plot->xAxis,
            static_cast<void (QCPAxis::*)(const QCPRange &)>(
                &QCPAxis::rangeChanged),
            this, &MyWidget::limitXAxisRange);
  }
}

void MyWidget::setPlotLabel(double index, double sum, double mean, double rms,
                            double ps) {
  if (index >= 0 && index < labels.size()) {
    labels[index]->setText(
        QString::asprintf("Σ=%llu μ=%.2f σ=%.*f", static_cast<quint64>(sum),
                          mean, ps == 1. ? 0 : 3, rms * (ps == 1. ? 1e3 : 1)));
  }
}

void MyWidget::setXAxisType(int type, int threshold) {
  if(originalXDataValues.size()>0&&yDataValues1.size()>0
  &&originalXDataValues[0].size()>0&&originalYDataValues1[0].size()>0){
    switch (type) {
      case 0:
        break;
      case 1:
        if (histogramType == 0) {
          xDataValues = originalXDataValues;
          yDataValues1 = originalYDataValues1;
          for (int i = 0; i < plots.size(); ++i) {
            plots[i]->xAxis->setRange(minXValue, maxXValue);
          }
        } 
        else if (histogramType == 1) {
          for (int i = 0; i < plots.size(); ++i) {
            plots[i]->xAxis->setRange(minXValue, maxXValue);
          }
        }
        break;
      case 2:
        // Find the first and last index where the data is above the threshold
        auto trimData = [&](const QVector<double> &data, int &firstIndex,
                            int &lastIndex) -> bool {
          if (data.isEmpty()) {
            return false;
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
          return true;
        };

        // Go through all the data and trim it
        for (int i = 0; i < 12; ++i) {
          int firstIndex, lastIndex;

          // Histogram type 0 = time, 1 = charge
          if (histogramType == 0) {
            if (!trimData(originalYDataValues1[i], firstIndex, lastIndex)) {
              continue; 
            }
            plots[i]->xAxis->setRange(originalXDataValues[i][firstIndex], 
              originalXDataValues[i][lastIndex]);
          } 
          else if (histogramType == 1) {
            if (!trimData(originalYDataValuesSum[i], firstIndex, lastIndex)) {
              continue; 
            }
            plots[i]->xAxis->setRange(originalXDataValues[i][firstIndex], 
              originalXDataValues[i][lastIndex]);
          }
        }
        break;
    }
    adjustYAxisRange();
  }
}

void MyWidget::setYAxisType(int type) {
  yAxisType = type;
  for (int i = 0; i < plots.size(); ++i) {
    plots[i]->yAxis->setScaleType(type == 1 ? QCPAxis::stLogarithmic
                                            : QCPAxis::stLinear);

    QSharedPointer<QCPAxisTicker> linTicker =
        QSharedPointer<QCPAxisTicker>(new CustomYAxisTicker());
    QSharedPointer<QCPAxisTicker> logTicker =
        QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog());

    plots[i]->yAxis->setTicker(type == 1 ? logTicker : linTicker);
  }
  adjustYAxisRange();
}

void MyWidget::setChargeType(int type) {
  chargeType = type;
  QVector<double> emptyVector;
  if (xDataValues.size() > 0 && yDataValues1.size() > 0 &&
  xDataValues[0].size() > 0 && yDataValues1[0].size() > 0 &&
  yDataValues2.size() == yDataValues1.size() &&
  yDataValuesSum.size() == yDataValues1.size() &&
  barsList1.size() == plots.size() &&
  barsList2.size() == plots.size()) {
    for (int i = 0; i < plots.size(); i++) {
      if (chargeType == 0) {
        barsList1[i]->setData(xDataValues[i], yDataValues1[i]);
        barsList2[i]->setData(emptyVector, emptyVector);
        barsList1[i]->setBrush(QBrush(OKcolor.darker(200)));
      } else if (chargeType == 1) {
        barsList1[i]->setData(xDataValues[i], yDataValues2[i]);
        barsList2[i]->setData(emptyVector, emptyVector);
        barsList1[i]->setBrush(QBrush(OKcolor));
      } else if (chargeType == 2){
        barsList1[i]->setData(xDataValues[i], yDataValuesSum[i]);
        barsList2[i]->setData(xDataValues[i], yDataValues1[i]);
        barsList1[i]->setBrush(QBrush(OKcolor));
        barsList2[i]->setBrush(QBrush(OKcolor.darker(200)));
      } else{
        barsList1[i]->setData(xDataValues[i], yDataValues1[i]);
        barsList2[i]->setData(xDataValues[i], yDataValues2[i]);
        barsList1[i]->setBrush(QBrush(OKcolor.darker(200)));
        QColor semiTransparentOK = OKcolor;
        semiTransparentOK.setAlpha(150);
        barsList2[i]->setBrush(QBrush(semiTransparentOK));
      }
    }
    adjustYAxisRange();
  }
}

void MyWidget::setTimeUnits(int type) {
  if(barsList1.size()==0){
    return;
  }
  unitsType=type;
  // Determine the new bin width based on the time unit type.
  double newBinWidth = 1.0;
  if (type == 1) {
    newBinWidth = 1000.0 / 40.0789639 / 30 / 64;
  }
  binWidth = newBinWidth;

  if (plots.isEmpty()) {
    return;
  }

  // Create tickers: one that is custom for integer (default) bins and one standard.
  QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
  intTicker->setTickStep(1.0);
  intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
  QSharedPointer<QCPAxisTicker> floatTicker(new QCPAxisTicker());
  bool defaultBin = (newBinWidth == 1.0);

  for (int i = 0; i < plots.size(); ++i) {
    if (!plots[i]) {
      continue;
    }

    // Scale the current x-axis range by the ratio of new bin width to the current bar width.
    //double currentBarWidth = (barsList1[0] ? barsList1[0]->width() : 1.0);
    double currentBarWidth = 1.0;
    QCPRange newRange = originalXRanges[i] * (newBinWidth / 1.0);
    double newMinXValue = minXValue * (newBinWidth/currentBarWidth);
    // Set the new width for all bars in barsList1.
    for (QCPBars *b : barsList1) {
      if (b)
        b->setWidth(newBinWidth);
    }

    // Update the x-axis ticker and formatting.
    plots[i]->xAxis->setTicker(defaultBin ? intTicker : floatTicker);
    plots[i]->xAxis->setNumberPrecision(defaultBin?0:2);
    plots[i]->xAxis->ticker()->setTickCount(5);
    plots[i]->xAxis->setTickLabelRotation(/*defaultBin ? -90 :*/ 0);
    plots[i]->xAxis->setTickLength(0, defaultBin ? 0 : 1);

    if (xRangeType == 0) {
      plots[i]->xAxis->setRange(newRange);
    }

    // Update the bar data keys and values for barsList1.
    if (i < yDataValues1.size() && !yDataValues1[i].isEmpty()) {
      QSharedPointer<QCPBarsDataContainer> dataContainer = barsList1[i]->data();
      if (dataContainer) {
        QCPBarsDataContainer::iterator iBar = dataContainer->begin();
        for (qint16 binI = 0; binI < yDataValues1[i].size(); ++binI) {
          if (iBar != dataContainer->end()) {
            // Use the new bin width when calculating the key.
            iBar->key = newMinXValue + binI * newBinWidth;
            // Set the value from our stored data.
            iBar->value = static_cast<quint32>(yDataValues1[i][binI]);
            ++iBar;
          }
        }
      }
    }

    QSharedPointer<QCPBarsDataContainer> dataContainer = barsList1[i]->data();
    xDataValues[i].clear();

    for (auto it = dataContainer->constBegin(); it != dataContainer->constEnd(); ++it) {
        xDataValues[i].append(it->key);
    }

    //plots[i]->replot(QCustomPlot::rpQueuedReplot);
  }
  originalXDataValues=xDataValues;
  adjustYAxisRange();
}

void MyWidget::setChargeUnitsValues(float value, bool isMV){
  if(isMV){
    mVperMIP = value;
  }
  else{
    ADCUperMIP = value;
  }
  if(unitsType!=0){
    this->setChargeUnits(unitsType);
  }
}

void MyWidget::setChargeUnits(int type) {
  if(barsList1.size()==0){
    return;
  }
  unitsType=type;

  // Determine the new bin width based on the time unit type.
  double newBinWidth = 1.0;
  if (type == 1) {
    newBinWidth = mVperMIP/ADCUperMIP;
  }
  else if(type == 2){
    newBinWidth = 1./ADCUperMIP;
  }
  binWidth = newBinWidth;

  if (plots.isEmpty()) {
    return;
  }

  // Create tickers: one that is custom for integer (default) bins and one standard.
  QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
  intTicker->setTickStep(1.0);
  intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
  QSharedPointer<QCPAxisTicker> floatTicker(new QCPAxisTicker());
  bool defaultBin = (newBinWidth == 1.0);

  // Update each plot.
  for (int i = 0; i < plots.size(); ++i) {
    if (!plots[i]) {
      continue;
    }

    // Scale the current x-axis range by the ratio of new bin width to the current bar width.
    //double currentBarWidth = (barsList1[0] ? barsList1[0]->width() : 1.0);
    double currentBarWidth = 1.0;
    QCPRange newRange = originalXRanges[i] * (newBinWidth / 1.0);
    double newMinXValue = minXValue * (newBinWidth / currentBarWidth);
    
    // Set the new width for all bars in barsList1.
    for (QCPBars *b : barsList1) {
      if (b)
        b->setWidth(newBinWidth);
    }
    for (QCPBars *b : barsList2) {
      if (b)
        b->setWidth(newBinWidth);
    }

    // Update the x-axis ticker and formatting.
    plots[i]->xAxis->setTicker(defaultBin ? intTicker : floatTicker);
    plots[i]->xAxis->setNumberPrecision(defaultBin?0:2);
    plots[i]->xAxis->ticker()->setTickCount(5);
    plots[i]->xAxis->setTickLabelRotation(/*defaultBin ? -90 :*/ 0);
    plots[i]->xAxis->setTickLength(0, defaultBin ? 0 : 1);
    // Optionally update the x-axis range if xRangeType is 0.
    if (xRangeType == 0) {
      plots[i]->xAxis->setRange(newRange);
    }

    if (chargeType==2 && i < yDataValuesSum.size() && !yDataValuesSum[i].isEmpty()) {
      QSharedPointer<QCPBarsDataContainer> dataContainer1 = barsList1[i]->data();
      QSharedPointer<QCPBarsDataContainer> dataContainer2 = barsList2[i]->data();
      if (dataContainer1&&dataContainer2) {
        QCPBarsDataContainer::iterator iBar1 = dataContainer1->begin();
        QCPBarsDataContainer::iterator iBar2 = dataContainer2->begin();
        for (qint16 binI = 0; binI < yDataValuesSum[i].size(); ++binI) {
          if (iBar1 != dataContainer1->end()) {
            iBar1->key = newMinXValue + binI * newBinWidth;
            iBar1->value = static_cast<quint32>(yDataValuesSum[i][binI]);
            iBar2->key = newMinXValue + binI * newBinWidth;
            iBar2->value = static_cast<quint32>(yDataValues1[i][binI]);
            ++iBar1;
            ++iBar2;
          }
        }
      }
    }
    else if (chargeType==0 && i < yDataValues1.size() && !yDataValues1[i].isEmpty()) {
      QSharedPointer<QCPBarsDataContainer> dataContainer = barsList1[i]->data();
      if (dataContainer) {
        QCPBarsDataContainer::iterator iBar = dataContainer->begin();
        for (qint16 binI = 0; binI < yDataValues1[i].size(); ++binI) {
          if (iBar != dataContainer->end()) {
            iBar->key = newMinXValue + binI * newBinWidth;
            iBar->value = static_cast<quint32>(yDataValues1[i][binI]);
            ++iBar;
          }
        }
      }
    }
    else if (chargeType==1 && i < yDataValues2.size() && !yDataValues2[i].isEmpty()) {
      QSharedPointer<QCPBarsDataContainer> dataContainer = barsList1[i]->data();
      if (dataContainer) {
        QCPBarsDataContainer::iterator iBar = dataContainer->begin();
        for (qint16 binI = 0; binI < yDataValues2[i].size(); ++binI) {
          if (iBar != dataContainer->end()) {
            iBar->key = newMinXValue + binI * newBinWidth;
            iBar->value = static_cast<quint32>(yDataValues2[i][binI]);
            ++iBar;
          }
        }
      }
    }
    else if (chargeType==3 && i < yDataValues1.size() && !yDataValues1[i].isEmpty()
    && i < yDataValues2.size() && !yDataValues2[i].isEmpty()) {
      QSharedPointer<QCPBarsDataContainer> dataContainer1 = barsList1[i]->data();
      QSharedPointer<QCPBarsDataContainer> dataContainer2 = barsList2[i]->data();
      if (dataContainer1&&dataContainer2) {
        QCPBarsDataContainer::iterator iBar1 = dataContainer1->begin();
        QCPBarsDataContainer::iterator iBar2 = dataContainer2->begin();
        for (qint16 binI = 0; binI < std::min(yDataValues1[i].size(),yDataValues2[i].size()); ++binI) {
          if (iBar1 != dataContainer1->end()) {
            iBar1->key = newMinXValue + binI * newBinWidth;
            iBar1->value = static_cast<quint32>(yDataValues1[i][binI]);
            iBar2->key = newMinXValue + binI * newBinWidth;
            iBar2->value = static_cast<quint32>(yDataValues2[i][binI]);
            ++iBar1;
            ++iBar2;
          }
        }
      }
    }

    QSharedPointer<QCPBarsDataContainer> dataContainer = barsList1[i]->data();
    xDataValues[i].clear();

    for (auto it = dataContainer->constBegin(); it != dataContainer->constEnd(); ++it) {
        xDataValues[i].append(it->key);
    }
  }
  originalXDataValues=xDataValues;
  adjustYAxisRange();
}

void MyWidget::setValues(const QVector<double> &newValues) {
  QVector<QVector<double>> xDataList(plots.size());
  QVector<QVector<double>> yDataList1(plots.size());
  QVector<QVector<double>> yDataList2(plots.size());
  QVector<QVector<double>> yDataListSum(plots.size());
  if (newValues.size() < 60000) {
    histogramType = 0;
    double newBinWidth = 1.0;
    if (unitsType == 1) {
      newBinWidth = 1000.0 / 40.0789639 / 30 / 64;
    }
    for (int i = 0; i < 12; i++) {
      QVector<double> xData, yData1, yData2;
      const QVector<double> data1 = newValues.mid(
          i * (maxXValue - minXValue + 1), (maxXValue - minXValue + 1));

      //double step = ((double)(maxXValue - minXValue + 1)) / (data1.size() - 1);
      double step = newBinWidth;
      double newMinXValue = minXValue * (newBinWidth / 1.0);
      for (int j = 0; j < data1.size(); ++j) {
        xData.append(newMinXValue + j * step/*  - step / 2*/);
        yData1.append(data1[j]);
      }

      xDataList[i] = xData;
      yDataList1[i] = yData1;
    }
    xDataValues = xDataList;
    yDataValues1 = yDataList1;
    originalXDataValues = xDataList;
    originalYDataValues1 = yDataList1;

    for (int i = 0; i < plots.size(); ++i) {
      barsList1[i]->setWidth(newBinWidth);
      barsList1[i]->setData(xDataList[i], yDataList1[i]);
      barsList1[i]->setBrush(QBrush(OKcolor.darker(200)));
    }
  } else {
    histogramType = 1;
    double newBinWidth = 1.0;
    if (unitsType == 1) {
      newBinWidth = mVperMIP/ADCUperMIP;
    }
    else if(unitsType == 2){
      newBinWidth = 1./ADCUperMIP;
    }
    for (int i = 0; i < 12; i++) {
      QVector<double> xData, yData1, yData2, yDataSum;
      const QVector<double> data1 = newValues.mid(
          i * (maxXValue - minXValue + 1), (maxXValue - minXValue + 1));
      const QVector<double> data2 = newValues.mid(
          (i + 12) * (maxXValue - minXValue + 1), (maxXValue - minXValue + 1));

      //double step = ((double)(maxXValue - minXValue + 1)) / (data1.size() - 1);
      double step = newBinWidth;
      double newMinXValue = minXValue * (newBinWidth / 1.0);
      for (int j = 0; j < data1.size(); ++j) {
        xData.append(newMinXValue + j * step/* - step / 2*/);
        yData1.append(data1[j]);
        yData2.append(data2[j]);
        yDataSum.append(data1[j] + data2[j]);
      }

      xDataList[i] = xData;
      yDataList1[i] = yData1;
      yDataList2[i] = yData2;
      yDataListSum[i] = yDataSum;
    }
    originalXDataValues = xDataList;
    originalYDataValues1 = yDataList1;
    originalYDataValues2 = yDataList2;
    originalYDataValuesSum = yDataListSum;
    xDataValues = xDataList;
    yDataValues1 = yDataList1;
    yDataValues2 = yDataList2;
    yDataValuesSum = yDataListSum;
    const QVector<double> emptyVector;

    for (int i = 0; i < plots.size(); ++i) {
      if (chargeType == 0) {
        barsList1[i]->setData(xDataList[i], yDataList1[i]);
        barsList2[i]->setData(emptyVector, emptyVector);
        barsList1[i]->setBrush(QBrush(OKcolor.darker(200)));
        barsList1[i]->setWidth(newBinWidth);
      } else if (chargeType == 1) {
        barsList1[i]->setData(xDataList[i], yDataList2[i]);
        barsList2[i]->setData(emptyVector, emptyVector);
        barsList1[i]->setBrush(QBrush(OKcolor));
        barsList1[i]->setWidth(newBinWidth);
      } else if (chargeType == 2){
        barsList1[i]->setData(xDataValues[i], yDataValuesSum[i]);
        barsList2[i]->setData(xDataValues[i], yDataValues1[i]);
        barsList1[i]->setBrush(QBrush(OKcolor));
        barsList2[i]->setBrush(QBrush(OKcolor.darker(200)));
        barsList1[i]->setWidth(newBinWidth);
        barsList2[i]->setWidth(newBinWidth);
      } else{
        barsList1[i]->setData(xDataValues[i], yDataValues1[i]);
        barsList2[i]->setData(xDataValues[i], yDataValues2[i]);
        barsList1[i]->setBrush(QBrush(OKcolor.darker(200)));
        barsList2[i]->setBrush(QBrush(OKcolor));
        barsList1[i]->setWidth(newBinWidth);
        barsList2[i]->setWidth(newBinWidth);
      }
    }
  }

  originalXRanges.clear();
  for (int i = 0; i < plots.size(); ++i) {
      originalXRanges.append(QCPRange(minXValue, maxXValue));
  }


  adjustYAxisRange();
}

void MyWidget::setPsParameter(bool ps) {
  psParameter = ps;
  adjustYAxisRange();
}

void MyWidget::adjustYAxisRange() {
  if(suppressAutoAdjust){
    return;
  }
  for (int i = 0; i < plots.size(); ++i) {
    QCustomPlot *plot = plots[i];
    if (!plot)
      continue;

    QCPRange xRange = plot->xAxis->range();
    double maxY = 0, mean = 0.0, RMS = 0.0;
    uint64_t sum = 0;
    bool hasData = false;

    bool isBarsList1=false;
    for (auto bars : {barsList1[i], barsList2[i]}) {
      if (!bars || bars->data()->isEmpty())
        continue;

      isBarsList1=!isBarsList1;
      hasData = true;
      for (auto it = bars->data()->constBegin(); it != bars->data()->constEnd();
           ++it) {
        if (it->key >= xRange.lower && it->key <= xRange.upper) {
          maxY = qMax(maxY, it->value);
          if(isBarsList1){
            sum += it->value;
            mean += it->value * (it->key);
            RMS += it->value * (it->key) * (it->key);
          }
        }
      }
    }

    if (sum != 0) {
      mean = mean / sum;
      RMS = sqrt((RMS / sum) - pow(mean, 2));  
    }
    setPlotLabel(i, sum, mean, RMS, psParameter && unitsType == 1);

    double minY = (yAxisType == 1) ? 0.8 : 0;
    double yLower = hasData ? qMax(minY, plot->yAxis->range().lower) : 0;
    double yUpper = hasData ? maxY * 1.1 : 1;

    plot->yAxis->setRange(yLower, yUpper);
    plot->replot();
  }
}

void MyWidget::setRange(double min, double max) {
  minXValue = min;
  maxXValue = max;
  for (QCustomPlot *plot : plots) {
    plot->xAxis->setRange(min, max);
    plot->yAxis->setRangeLower(0);
    plot->replot();
  }
}

void MyWidget::limitXAxisRange(const QCPRange &newRange) {
  double minLimit = minXValue;
  double maxLimit = maxXValue;

  QCPAxis *senderAxis = qobject_cast<QCPAxis *>(sender());
  if (!senderAxis)
    return;

  QCustomPlot *senderPlot = senderAxis->parentPlot();
  if (!senderPlot)
    return;

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

void MyWidget::applyFullConfiguration(const QVector<double> &newValues,
                                      int xAxisType,
                                      int threshold,
                                      int yAxisType,
                                      int chargeType,
                                      int timeUnitType,
                                      float chargeUnitValueMV,
                                      float chargeUnitValueADC,
                                      bool ps) {

  suppressAutoAdjust=true;
  psParameter = ps;

  for (QCustomPlot* plot : plots) {
    if (plot) {
      plot->blockSignals(true);
      plot->setUpdatesEnabled(false);
    }
  }

  setValues(newValues);
  setXAxisType(xAxisType, threshold);
  setYAxisType(yAxisType);
  setChargeType(chargeType);
  setTimeUnits(timeUnitType);
  setChargeUnitsValues(chargeUnitValueMV, true);
  setChargeUnitsValues(chargeUnitValueADC, false);
  setPsParameter(ps);

  suppressAutoAdjust=false;

  for (QCustomPlot* plot : plots) {
    if (plot) {
      plot->blockSignals(false);
      plot->setUpdatesEnabled(true);
    }
  }

  adjustYAxisRange();
}
