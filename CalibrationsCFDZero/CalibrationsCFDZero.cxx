#include "CalibrationsCFDZero.hxx"
#include <QPalette>
#include <QTime>
#include <QDebug>
#include <QVariant>


// This macro registers the plugin.
EWO_PLUGIN( CalibrationsCFDZero )
const QColor OKcolor(0xb0d959);

CalibrationsCFDZero::CalibrationsCFDZero(QWidget *parent)
    : BaseExternWidget(parent)
{
    baseWidget = new MyWidget(parent);
}

QWidget* CalibrationsCFDZero::widget() const
{
    return baseWidget;
}

QStringList CalibrationsCFDZero::signalList() const
{
    QStringList list;
    return list;
}

bool CalibrationsCFDZero::methodInterface(const QString &name, QVariant::Type &retVal,
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

QVariant CalibrationsCFDZero::invokeMethod(const QString &name, QList<QVariant> &values, QString &error)
{
    if ( !hasNumArgs(name, values, 1, error) ) return QVariant();
    if (name == "setValues") {
        QString value = values[0].toString();
        int plotIndex = values[1].toInt();
        QVector<QVector<uint>> result;
        QStringList rows = value.split('\n', Qt::SkipEmptyParts);
        for (const QString& row : rows) {
            QStringList values = row.split(',', Qt::SkipEmptyParts);
            QVector<uint> intRow;
            for (const QString& val : values) {
                bool ok = false;
                int number = val.trimmed().toInt(&ok);
                if (ok) {
                    intRow.append(number);
                }
            }
            QVector<quint32> histLine(401);
            for (int k=0; k<4096; ++k) {
                int const idx = k-2048;
                if (idx < -200 || idx > 200) {
                    continue;
                }
                histLine[idx+200] = intRow[k];
            }
            result.append(histLine);
        }
        for(int i = 0, j=0; i < result.size(); ++i, j+=2) {
            baseWidget->setHistogramLine(plotIndex, j, result[i]); 
            if (j+1 != result.size()) {
                baseWidget->setHistogramLine(plotIndex, j+1, result[i]);
            }  
        }
        baseWidget->setAxisRange(-50, 50, -500, 500);
        baseWidget->rescaleDataRanges();
        baseWidget->replot();
        return QVariant();
    }
    else if (name == "setValuesAttenuator") {
        QString value = values[0].toString();

        QVector<QVector<uint>> result;
        QStringList rows = value.split('\n', Qt::SkipEmptyParts);
        for (const QString& row : rows) {
            QStringList values = row.split(',', Qt::SkipEmptyParts);
            if(values.size() != 3) {
                error = "Invalid number of values in row: " + row;
                qWarning() << error;
                return QVariant();
            }
            baseWidget->addPoint(values[0].toDouble(), values[1].toDouble(), values[2]=="1");
        }
        baseWidget->replot();
        return QVariant();
    }
    else if (name == "clearValues") {
        baseWidget->clear();
        baseWidget->replot();
        return QVariant();
    }
    else if (name == "setChargeType") {
        
        return QVariant();
    }
    else if (name == "setYAxisType") {
        
        return QVariant();
    }
    else if (name == "setTimeUnits"){
        
        return QVariant();
    }
    else if (name == "setChargeUnits"){
        
        return QVariant();
    }
    else if (name == "setPlotLabel") {
        
        return QVariant();
    }

    return BaseExternWidget::invokeMethod(name, values, error);
}