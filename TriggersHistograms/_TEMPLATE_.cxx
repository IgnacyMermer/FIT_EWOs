#include <_TEMPLATE_.hxx>

// TODO change to what you need
#include <QPushButton>
#include <QLayout>
#include <QDateTime>
#include <QPolygon>
#include <QPainter>
#include <QMap>

//--------------------------------------------------------------------------------

EWO_PLUGIN( _TEMPLATE_ )

//--------------------------------------------------------------------------------
// Here comes the implementation of the widget itself.
// The widget itself is a normal Qt widget and does not need to know
// anything about WinCC OA or the EWO interface
//--------------------------------------------------------------------------------

MyWidget::MyWidget(QWidget *parent)
  : QWidget(parent)
{
  // TODO here come your widget specific things
  QPushButton *b;
  QVBoxLayout *l = new QVBoxLayout(this);
  l->addWidget(b = new QPushButton("HELLO 1"));
  connect(b, SIGNAL(clicked()), this, SIGNAL(clicked1()));

  l->addWidget(b = new QPushButton("HELLO 2"));
  connect(b, SIGNAL(clicked()), this, SIGNAL(clicked2()));
}

//--------------------------------------------------------------------------------

QBrush MyWidget::getBrush() const
{
  return palette().brush(backgroundRole());
}

//--------------------------------------------------------------------------------

void MyWidget::setBrush(const QBrush &brush)
{
  QPalette pal = palette();
  pal.setBrush(backgroundRole(), brush);
  setPalette(pal);
}

//--------------------------------------------------------------------------------
// show how to do custom painting in the widget

void MyWidget::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setPen(pen);
  painter.drawLine(10, 10, width() - 10, 10);
}

//--------------------------------------------------------------------------------
// Here comes the implementation of the EWO interface class
// The EWO interface class is just a wrapper around a normal Qt widget
// and it gives WinCC OA a standard interface to communicate with it.
// In our example we use our own "MyWidget"
//--------------------------------------------------------------------------------

_TEMPLATE_::_TEMPLATE_(QWidget *parent)
  : BaseExternWidget(parent)
{
  // the widget will be deleted by the QWidget parent
  // Don't do it in destructor
  baseWidget = new MyWidget(parent);

  // TODO here come your app specific things
  // we connect the signals from our widget to our interface slots
  // so we can forward them to WinCC OA
  connect(baseWidget, SIGNAL(clicked1()), this, SLOT(clicked1()));
  connect(baseWidget, SIGNAL(clicked2()), this, SLOT(clicked2()));
}

//--------------------------------------------------------------------------------

QWidget *_TEMPLATE_::widget() const
{
  return baseWidget;
}

//--------------------------------------------------------------------------------

QStringList _TEMPLATE_::signalList() const
{
  QStringList list;

  // TODO for every signal you can emit, add one entry with the correct signature
  // See BaseExternWidget.hxx
  list.append("clicked1()");
  list.append("clicked2(time t)");

  return list;
}

//--------------------------------------------------------------------------------

QStringList _TEMPLATE_::methodList() const
{
  QStringList list;

  // TODO for every method you can invoke on your EWO, add one entry with the correct signature
  // See BaseExternWidget.hxx
  list.append("void changeColor(string color)");

  // example to transfer more complex datatypes from/to WinCC OA
  list.append("void setFloatValues(dyn_float dyn)");
  list.append("dyn_float getFloatValues()");

  // WinCC OA passes a QPoint array (QPolygon) by using a dyn_dyn_int, where each item contains
  // 2 ints (x and y value); for this to work, we also need to implement the methodInterface()
  // so that WinCC OA knows to convert the dyn_dyn_int into a QPolygon and not
  // into a simple QList<QVariant>
  list.append("void setPoints(dyn_dyn_int points)");

  // show how to use a dyn_anytype (passed as a QList<QVariant>)
  list.append("void setDynAnytype(dyn_anytype values)");

  // even a QMap can be used
  list.append("void setMap(mapping map)");

  return list;
}

//--------------------------------------------------------------------------------
// this method is optional, but as the WinCC OA user will pass a string
// from the WinCC OA color-database as a string, we must tell WinCC OA that we want to
// have a QColor object and not a QString here.
// Doing so allows to pass any static WinCC OA color either by name or whatever
// syntax WinCC OA provides from a CTRL script and the string from the script
// will be converted into a correct QColor.
// The same needs to be done with other datatypes where Qt uses special classes
// but the WinCC OA CTRL scripting language represents these differently, e.g.
// a QColor as a string, a QBrush as a string, a QPoint as a dyn_int, ...

bool _TEMPLATE_::methodInterface(const QString &name, QVariant::Type &retVal,
                                 QList<QVariant::Type> &args) const
{
  // TODO provide interface if needed
  if ( name == "changeColor" )
  {
    retVal = QVariant::Invalid;  // we return void

    // we only have 1 argument, which is a color
    args.append(QVariant::Color);
    return true;
  }

  if ( name == "setPoints" )
  {
    retVal = QVariant::Invalid;  // we return void

    // we only have 1 argument, which is a QPolygon
    args.append(QVariant::Polygon);
    return true;
  }

  return false;
}

//--------------------------------------------------------------------------------

QVariant _TEMPLATE_::invokeMethod(const QString &name, QList<QVariant> &values, QString &error)
{
  // TODO check for the method to invoke
  // all methods here are only examples and usually
  // not useful as they are

  if ( name == "changeColor" )
  {
    if ( !hasNumArgs(name, values, 1, error) ) return QVariant();

    // do what is supposed to happen ...
    QPalette palette;
    palette.setColor(baseWidget->backgroundRole(), values[0].value<QColor>());
    baseWidget->setPalette(palette);

    // this method returns void
    return QVariant();
  }

  if ( name == "setFloatValues" )
  {
    if ( !hasNumArgs(name, values, 1, error) ) return QVariant();

    // shows how to get a QList holding double values
    QList<QVariant> list = values[0].toList();
    foreach (QVariant item, list)
      qWarning("%f", item.toDouble());

    // this method returns void
    return QVariant();
  }

  if ( name == "getFloatValues" )
  {
    QList<QVariant> list;

    // shows how to return a QList holding double values
    for (int i = 0; i < 10; i++)
      list.append(QVariant(static_cast<double>(i)));

    return list;
  }

  if ( name == "setPoints" )
  {
    if ( !hasNumArgs(name, values, 1, error) ) return QVariant();

    QPolygon list = values[0].value<QPolygon>();
    foreach (QPoint item, list)
      qWarning("x=%d, y=%d", item.x(), item.y());

    // this method returns void
    return QVariant();
  }

  if ( name == "setDynAnytype" )
  {
    if ( !hasNumArgs(name, values, 1, error) ) return QVariant();

    QList<QVariant> list = values[0].toList();
    foreach (QVariant item, list)
      qWarning("datatype: %s", item.typeName());

    // this method returns void
    return QVariant();
  }

  if ( name == "setMap" )
  {
    if ( !hasNumArgs(name, values, 1, error) ) return QVariant();

    QMap<QString, QVariant> map = values[0].toMap();
    foreach (QVariant item, map)
      qWarning("datatype: %s", item.typeName());

    // this method returns void
    return QVariant();
  }

  return BaseExternWidget::invokeMethod(name, values, error);
}


//--------------------------------------------------------------------------------

void _TEMPLATE_::clicked1()
{
  // TODO this is how you emit the generic "signal" signal
  // See BaseExternWidget.hxx
  emit signal("clicked1");
}

//--------------------------------------------------------------------------------

void _TEMPLATE_::clicked2()
{
  // TODO this is how you emit the generic "signal" signal including an argument
  // See BaseExternWidget.hxx
  emit signal("clicked2", QVariant(QTime()));
}

//--------------------------------------------------------------------------------
