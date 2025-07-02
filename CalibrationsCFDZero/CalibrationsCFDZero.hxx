#ifndef _CALIBRATIONSCFDZERO_H_
#define _CALIBRATIONSCFDZERO_H_

#include <BaseExternWidget.hxx>
#include <QPen>
#include <QBrush>
#include <QPushButton>
#include "qcustomplot.h"
#include "MyWidget.hxx"

class EWO_EXPORT CalibrationsCFDZero : public BaseExternWidget
{
    Q_OBJECT

public:
    CalibrationsCFDZero(QWidget *parent);

    virtual QWidget *widget() const;

    virtual QStringList signalList() const;

    // This function tells WinCC which methods are available and what argument types they expect.
    virtual bool methodInterface(const QString &name, QVariant::Type &retVal,
                                 QList<QVariant::Type> &args) const;

    // Optionally update the method list for introspection purposes.
    QStringList methodList() const override
    {
        return { "void setRange(int, int)",
                 "void setValues(QVariantList)" };
    }

public slots:
    virtual QVariant invokeMethod(const QString &name, QList<QVariant> &values, QString &error);

private:
    MyWidget *baseWidget;
    QList<QCustomPlot *> plots;

//signals:
  //  bool signalInterface(const QString &name, QList<QVariant::Type> &args) const;
};

#endif