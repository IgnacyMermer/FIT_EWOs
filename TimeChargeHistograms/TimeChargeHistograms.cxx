#include "TimeChargeHistograms.hxx"
#include "MyWidget.hxx"
#include <QPalette>
#include <QTime>
#include <QDebug>
#include <QVariant>

// This macro registers the plugin.
EWO_PLUGIN( TimeChargeHistograms )

TimeChargeHistograms::TimeChargeHistograms(QWidget *parent)
    : BaseExternWidget(parent)
{
    baseWidget = new MyWidget(parent);
}

QWidget* TimeChargeHistograms::widget() const
{
    return baseWidget;
}

QStringList TimeChargeHistograms::signalList() const
{
    QStringList list;
    return list;
}

bool TimeChargeHistograms::methodInterface(const QString &name, QVariant::Type &retVal,
                                    QList<QVariant::Type> &args) const
{
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
    return false;
}

QVariant TimeChargeHistograms::invokeMethod(const QString &name, QList<QVariant> &values, QString &error)
{
    if ( !hasNumArgs(name, values, 1, error) ) return QVariant();
    if (name == "setValues") {
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
            static_cast<MyWidget*>(baseWidget)->setValues(newValues);
        }
        return QVariant();
    }
    else if (name == "setRange") {
        if (values.size() != 2) {
            error = "Expected a list of 2 ints";
            return QVariant();
        }
        int min = values[0].toInt();
        int max = values[1].toInt();
        static_cast<MyWidget*>(baseWidget)->setRange(min, max);
        return QVariant();
    }
    else if (name == "setXAxisType") {
        if (values.size() != 2) {
            error = "Expected a list of 2 ints";
            return QVariant();
        }
        int type = values[0].toInt();
        int threshold = values[1].toInt();
        static_cast<MyWidget*>(baseWidget)->setXAxisType(type, threshold);
        return QVariant();
    }
    else if (name == "setChargeType") {
        if (values.size() != 1) {
            error = "Expected a list of 1 int";
            return QVariant();
        }
        int type = values[0].toInt();
        static_cast<MyWidget*>(baseWidget)->setChargeType(type);
        return QVariant();
    }
    else if (name == "setYAxisType") {
        if (values.size() != 1) {
            error = "Expected a list of 1 int";
            return QVariant();
        }
        int type = values[0].toInt();
        static_cast<MyWidget*>(baseWidget)->setYAxisType(type);
        return QVariant();
    }
    else if (name == "setTimeUnits"){
        if (values.size() != 1) {
            error = "Expected a list of 1 int";
            return QVariant();
        }
        int type = values[0].toInt();
        static_cast<MyWidget*>(baseWidget)->setTimeUnits(type);
        return QVariant();
    }
    else if (name == "setChargeUnits"){
        if (values.size() != 1) {
            error = "Expected a list of 1 int";
            return QVariant();
        }
        int type = values[0].toInt();
        static_cast<MyWidget*>(baseWidget)->setChargeUnits(type);
        return QVariant();
    }
    else if (name == "setMVPerMIP"){
        if (values.size() != 1) {
            error = "Expected a list of 1 float";
            return QVariant();
        }
        double value = values[0].toDouble();
        static_cast<MyWidget*>(baseWidget)->setChargeUnitsValues(value, true);
        return QVariant();
    }
    else if (name == "setADCUperMIP"){
        if (values.size() != 1) {
            error = "Expected a list of 1 float";
            return QVariant();
        }
        double value = values[0].toDouble();
        static_cast<MyWidget*>(baseWidget)->setChargeUnitsValues(value, false);
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
        static_cast<MyWidget*>(baseWidget)->setPlotLabel(index, sum, mean, rms, ps);
        return QVariant();
    }
    else if (name == "applyFullConfiguration") {
        if (values.size() != 9) {
            error = "Expected 9 arguments: data (list), xAxisType, threshold, yAxisType, chargeType, timeUnitType, chargeUnitValue, isMV, psParam";
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
        float chargeUnitValueMV = static_cast<float>(values[6].toDouble());
        float chargeUnitValueADC = static_cast<float>(values[7].toDouble());
        bool psParam = values[8].toBool();

        static_cast<MyWidget*>(baseWidget)->applyFullConfiguration(
            newValues, xAxisType, threshold, yAxisType, chargeType, 
            timeUnitType, chargeUnitValueMV, chargeUnitValueADC, psParam);

        return QVariant();
    }
    else if (name == "setPSParameter") {
        if (values.size() != 1) {
            error = "Expected a list of 1 bool";
            return QVariant();
        }
        bool psParam = values[0].toBool();
        static_cast<MyWidget*>(baseWidget)->setPsParameter(psParam);
        return QVariant();
    }

    return BaseExternWidget::invokeMethod(name, values, error);
}