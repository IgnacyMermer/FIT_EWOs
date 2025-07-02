#include "TriggersHistograms.hxx"
#include "MyWidget.hxx"
#include <QPalette>
#include <QTime>
#include <QDebug>
#include <QVariant>

// This macro registers the plugin.
EWO_PLUGIN( TriggersHistograms )

TriggersHistograms::TriggersHistograms(QWidget *parent)
    : BaseExternWidget(parent)
{
    // Create the custom widget (which is now implemented in MyWidget.*)
    baseWidget = new MyWidget(parent);

    // Connect the widget’s signals to the TriggersHistograms slots
    //connect(baseWidget, SIGNAL(clicked1()), this, SLOT(clicked1()));
    //connect(baseWidget, SIGNAL(clicked2()), this, SLOT(clicked2()));
}

QWidget* TriggersHistograms::widget() const
{
    return baseWidget;
}

QStringList TriggersHistograms::signalList() const
{
    QStringList list;
    //list.append("clicked1()");
    //list.append("clicked2(time t)");
    return list;
}

bool TriggersHistograms::methodInterface(const QString &name, QVariant::Type &retVal,
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

QVariant TriggersHistograms::invokeMethod(const QString &name, QList<QVariant> &values, QString &error)
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
            // Call the widget’s method
            static_cast<MyWidget*>(baseWidget)->setValues(newValues, i);
        }
        return QVariant();
    }
    else if (name == "setRange") {
        // Expected a list of 2 ints
        if (values.size() != 2) {
            error = "Expected a list of 2 ints";
            return QVariant();
        }
        int min = values[0].toInt();
        int max = values[1].toInt();
        // Call the widget’s method
        static_cast<MyWidget*>(baseWidget)->setRange(min, max);
        return QVariant();
    }
    else if (name == "clearValues") {
        for(int i=0; i<3; i++){
            static_cast<MyWidget*>(baseWidget)->setPlotLabel(i,0,0,0,0);
        }
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
    else if (name == "setYAxisType") {
        if (values.size() != 1) {
            error = "Expected a list of 1 int";
            return QVariant();
        }
        int type = values[0].toInt();
        static_cast<MyWidget*>(baseWidget)->setYAxisType(type);
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

    return BaseExternWidget::invokeMethod(name, values, error);
}