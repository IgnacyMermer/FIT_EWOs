#include "SinglePlotWidget.h"
#include "HistogramWidget.hxx"
#include <QPalette>
#include <QTime>
#include <QDebug>
#include <QVariant>

// This macro registers the plugin.
EWO_PLUGIN( HistogramWidget )

HistogramWidget::HistogramWidget(QWidget *parent)
    : BaseExternWidget(parent)
{
    baseWidget = new SinglePlotWidget(parent);
}

QWidget* HistogramWidget::widget() const
{
    return baseWidget;
}

QStringList HistogramWidget::signalList() const
{
    QStringList list;
    return list;
}

bool HistogramWidget::methodInterface(const QString &name, QVariant::Type &retVal,
                                    QList<QVariant::Type> &args) const
{
    qWarning()<<"methodInterface";
    if (name == "changeColor") {
        retVal = QVariant::Invalid;
        args.append(QVariant::Color);
        return true;
    }
    if (name == "setPoints") {
        retVal = QVariant::Invalid;
        args.append(QVariant::Polygon);
        return true;
    }
    if (name == "setRange") {
        retVal = QVariant::Invalid;
        args.append(QVariant::Int);
        args.append(QVariant::Int);
        return true;
    }
    if (name == "setValues") {
        retVal = QVariant::Invalid;
        args.append(QVariant::List);
        return true;
    }
    
    if (name == "setFitRange") {
        retVal = QVariant::Invalid;
        args.append(QVariant::Double);
        args.append(QVariant::Double);
        return true;
    }
    
    if (name == "fitHistogram") {
        retVal = QVariant::Invalid;
        return true;
    }
    return false;
}

QVariant HistogramWidget::invokeMethod(const QString &name, QList<QVariant> &values, QString &error)
{
    if ( !hasNumArgs(name, values, 1, error) ) return QVariant();
    if (name == "setValues") {
        QVector<QVector<double>> newValuesList;
        // Expected a list of lists of ints
        for (int i = 0; i < values.size(); ++i) {
            QVariant arg = values[i];
            if (!arg.canConvert<QVariantList>()) {
                error = "Expected a list of ints for dataset " + QString::number(i);
                return QVariant();
            }
            QVariantList list = arg.toList();
            QVector<double> newValues;
            for (const QVariant &v : list) {
                newValues.append(v.toDouble());
            }
            newValuesList.append(newValues);
        }
        static_cast<SinglePlotWidget*>(baseWidget)->setValues(newValuesList);
        return QVariant();
    }
    else if (name == "setRange") {
        if (values.size() != 2) {
            error = "Expected a list of 2 ints";
            return QVariant();
        }
        int min = values[0].toInt();
        int max = values[1].toInt();
        static_cast<SinglePlotWidget*>(baseWidget)->setInitialRange(min, max);
        return QVariant();
    }
    else if (name == "setXAxisType") {
        if (values.size() != 2) {
            error = "Expected a list of 2 ints";
            return QVariant();
        }
        int type = values[0].toInt();
        int threshold = values[1].toInt();
        static_cast<SinglePlotWidget*>(baseWidget)->setXAxisType(type, threshold);
        return QVariant();
    }
    else if (name == "setChargeType") {
        if (values.size() != 1) {
            error = "Expected a list of 1 int";
            return QVariant();
        }
        int type = values[0].toInt();
        static_cast<SinglePlotWidget*>(baseWidget)->setChargeType(type);
        return QVariant();
    }
    else if (name == "setYAxisType") {
        if (values.size() != 1) {
            error = "Expected a list of 1 int";
            return QVariant();
        }
        int type = values[0].toInt();
        static_cast<SinglePlotWidget*>(baseWidget)->setYAxisType(type);
        return QVariant();
    }
    else if (name == "setTimeUnits"){
        if (values.size() != 1) {
            error = "Expected a list of 1 int";
            return QVariant();
        }
        int type = values[0].toInt();
        static_cast<SinglePlotWidget*>(baseWidget)->setTimeUnits(type);
        return QVariant();
    }
    else if (name == "setChargeUnits"){
        if (values.size() != 1) {
            error = "Expected a list of 1 int";
            return QVariant();
        }
        int type = values[0].toInt();
        static_cast<SinglePlotWidget*>(baseWidget)->setChargeUnits(type);
        return QVariant();
    }
    else if (name == "setMVPerMIP"){
        if (values.size() != 1) {
            error = "Expected a list of 1 float";
            return QVariant();
        }
        double value = values[0].toDouble();
        static_cast<SinglePlotWidget*>(baseWidget)->setChargeUnitsValues(value, true);
        return QVariant();
    }
    else if (name == "setADCUperMIP"){
        if (values.size() != 1) {
            error = "Expected a list of 1 float";
            return QVariant();
        }
        double value = values[0].toDouble();
        static_cast<SinglePlotWidget*>(baseWidget)->setChargeUnitsValues(value, false);
        return QVariant();
    }
    else if (name == "setPlotLabel") {
        if (values.size() != 5) {
            error = "Expected a list of 5 double";
            return QVariant();
        }
        double index = values[0].toDouble();
        double sum = values[1].toDouble();
        double mean = values[2].toDouble();
        double rms = values[3].toDouble();
        double ps = values[4].toDouble();
        static_cast<SinglePlotWidget*>(baseWidget)->setPlotLabel(index, sum, mean, rms, ps);
        return QVariant();
    }
    /*else if (name == "applyFullConfiguration") {
        if (values.size() != 10) {
            error = "Expected 10 arguments: data (list), xAxisType, threshold, yAxisType, chargeType, timeUnitType, chargeUnitType, chargeUnitValue, isMV, psParam";
            return QVariant();
        }
        QVector<double> newValues;
        for (const QVariant &v : values[0].toList()) {
            newValues.append(v.toDouble());
        }
        int xAxisType = values[1].toInt();
        int threshold = values[2].toInt();
        int yAxisType = values[3].toInt();
        int chargeType = values[4].toInt();
        int timeUnitType = values[5].toInt();
        int chargeUnitType = values[6].toInt();
        float chargeUnitValueMV = static_cast<float>(values[7].toDouble());
        float chargeUnitValueADC = static_cast<float>(values[8].toDouble());
        bool psParam = values[9].toBool();

        static_cast<SinglePlotWidget*>(baseWidget)->applyFullConfiguration(
            newValues, xAxisType, threshold, yAxisType, chargeType, 
            timeUnitType, chargeUnitType, chargeUnitValueMV, chargeUnitValueADC, 
            psParam);

        return QVariant();
    }*/
    else if (name == "setPSParameter") {
        if (values.size() != 1) {
            error = "Expected a list of 1 bool";
            return QVariant();
        }
        bool psParam = values[0].toBool();
        static_cast<SinglePlotWidget*>(baseWidget)->setPsParameter(psParam);
        return QVariant();
    }
    
    else if (name == "setFitRange") {
        if (values.size() != 2) {
           error = "Expected 2 doubles for fit range";
           return QVariant();
        }   
        double fitMin = values[0].toDouble();
        double fitMax = values[1].toDouble();
        static_cast<SinglePlotWidget*>(baseWidget)->setFitRange(fitMin, fitMax);
        return QVariant();
    }

    else if (name == "fitHistogram") {
        static_cast<SinglePlotWidget*>(baseWidget)->fitHistogram();
        return QVariant();
    }
 
    return BaseExternWidget::invokeMethod(name, values, error);
}