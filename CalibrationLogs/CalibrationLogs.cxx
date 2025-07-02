#include "CalibrationLogs.hxx"
#include <QPalette>
#include <QTime>
#include <QDebug>
#include <QVariant>
#include <QScrollBar>


// This macro registers the plugin.
EWO_PLUGIN( CalibrationLogs )
const QColor OKcolor(0xb0d959);

CalibrationLogs::CalibrationLogs(QWidget *parent)
    : BaseExternWidget(parent)
{
    baseWidget = new QPlainTextEdit(parent);
    baseWidget->setReadOnly(true);
    baseWidget->setUndoRedoEnabled(false);
    baseWidget->setTextInteractionFlags(Qt::NoTextInteraction);

}

QWidget* CalibrationLogs::widget() const
{
    return baseWidget;
}

QStringList CalibrationLogs::signalList() const
{
    QStringList list;
    return list;
}

bool CalibrationLogs::methodInterface(const QString &name, QVariant::Type &retVal,
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

QVariant CalibrationLogs::invokeMethod(const QString &name, QList<QVariant> &values, QString &error)
{
    if ( !hasNumArgs(name, values, 1, error) ) return QVariant();
    if (name == "appendLogs") {
        baseWidget->appendPlainText(values[0].toString());
        QScrollBar *sb = baseWidget->verticalScrollBar();
        sb->setValue(sb->maximum());
        return QVariant();
    }
    else if (name == "clearLogs") {
        baseWidget->clear();
        return QVariant();
    }

    return BaseExternWidget::invokeMethod(name, values, error);
}