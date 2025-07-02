#ifndef __TEMPLATE__H_
#define __TEMPLATE__H_

#include <BaseExternWidget.hxx>
#include <QPen>
#include <QBrush>

//--------------------------------------------------------------------------------
// this is the real widget (an ordinary Qt widget), which can also use Q_PROPERTY

class MyWidget : public QWidget
{
  Q_OBJECT

  // TODO example properties
  Q_PROPERTY( QStringList values READ getValues WRITE setValues DESIGNABLE false )
  Q_PROPERTY( QPen pen           READ getPen    WRITE setPen    DESIGNABLE false )
  Q_PROPERTY( QBrush brush       READ getBrush  WRITE setBrush  DESIGNABLE false )

  public:
    MyWidget(QWidget *parent);

    // TODO example of a string list property
    QStringList getValues() const { return values; }
    void setValues(const QStringList &list) { values = list; }

    // a QPen property used on a line drawn inside this widget
    const QPen &getPen() const { return pen; }
    void setPen(const QPen &p) { pen = p; update(); }

    // a QBrush property used as the background of this widget
    QBrush getBrush() const;
    void setBrush(const QBrush &brush);

  signals:
    // TODO example of a 2 signals this widget emits
    void clicked1();
    void clicked2();

  protected:
    virtual void paintEvent(QPaintEvent *event);

  private:
    // TODO example of a string list property
    QStringList values;
    QPen pen;
};

//--------------------------------------------------------------------------------
// this is the EWO interface class

class EWO_EXPORT _TEMPLATE_ : public BaseExternWidget
{
  Q_OBJECT

  public:
    _TEMPLATE_(QWidget *parent);

    virtual QWidget *widget() const;

    virtual QStringList signalList() const;

    virtual bool methodInterface(const QString &name, QVariant::Type &retVal,
                                 QList<QVariant::Type> &args) const;

    virtual QStringList methodList() const;

  public slots:
    virtual QVariant invokeMethod(const QString &name, QList<QVariant> &values, QString &error);

  private slots:
    // TODO the slots you need to redirect to the generic "signal" signal
    void clicked1();
    void clicked2();

  private:
    MyWidget *baseWidget;
};

#endif
