#include "SlaveNodeItem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QFontMetrics>

static constexpr qreal kNodeWidth = 180.0;
static constexpr qreal kNodeHeight = 80.0;
static constexpr qreal kCornerRadius = 6.0;

SlaveNodeItem::SlaveNodeItem(int position, const QString &name,
                             const QString &state, QGraphicsItem *parent)
    : QGraphicsObject(parent), position_(position), name_(name), state_(state) {
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setAcceptHoverEvents(true);
}

QRectF SlaveNodeItem::boundingRect() const {
  return {0, 0, kNodeWidth, kNodeHeight};
}

void SlaveNodeItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *) {
  Q_UNUSED(option);
  painter->setRenderHint(QPainter::Antialiasing, true);

  const QRectF rect(0, 0, kNodeWidth, kNodeHeight);
  const QColor stateColor = colorForState(state_);

  // Background
  painter->setBrush(isSelected() ? stateColor.lighter(130) : QColor(45, 45, 48));
  painter->setPen(QPen(stateColor, isSelected() ? 2.5 : 1.5));
  painter->drawRoundedRect(rect, kCornerRadius, kCornerRadius);

  // State indicator bar at top
  painter->setBrush(stateColor);
  painter->setPen(Qt::NoPen);
  painter->drawRoundedRect(QRectF(0, 0, kNodeWidth, 4), kCornerRadius, kCornerRadius);
  painter->drawRect(QRectF(0, 2, kNodeWidth, 2));

  // Text
  painter->setPen(QColor(220, 220, 220));
  QFont font;
  font.setPointSize(9);
  painter->setFont(font);

  // Position number
  const QString posText = QString("#%1").arg(position_);
  painter->drawText(QRectF(8, 8, 40, 20), Qt::AlignLeft | Qt::AlignVCenter, posText);

  // State label
  painter->setPen(stateColor);
  font.setBold(true);
  painter->setFont(font);
  painter->drawText(QRectF(kNodeWidth - 60, 8, 52, 20),
                    Qt::AlignRight | Qt::AlignVCenter, state_);
  font.setBold(false);
  painter->setFont(font);

  // Slave name
  painter->setPen(QColor(200, 200, 200));
  font.setPointSize(10);
  painter->setFont(font);
  const QFontMetrics fm(font);
  const QString elidedName = fm.elidedText(name_, Qt::ElideRight,
                                            static_cast<int>(kNodeWidth - 20));
  painter->drawText(QRectF(8, 30, kNodeWidth - 16, 22),
                    Qt::AlignLeft | Qt::AlignVCenter, elidedName);

  // Vendor ID
  if (!vendorId_.isEmpty()) {
    painter->setPen(QColor(140, 140, 140));
    font.setPointSize(8);
    painter->setFont(font);
    painter->drawText(QRectF(8, 54, kNodeWidth - 16, 18),
                      Qt::AlignLeft | Qt::AlignVCenter, vendorId_);
  }
}

void SlaveNodeItem::setState(const QString &state) {
  if (state_ == state) return;
  state_ = state;
  update();
}

void SlaveNodeItem::setVendorId(const QString &vendorId) {
  vendorId_ = vendorId;
  update();
}

void SlaveNodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsObject::mousePressEvent(event);
  emit clicked(position_);
}

void SlaveNodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsObject::mouseDoubleClickEvent(event);
  emit doubleClicked(position_);
}

QColor SlaveNodeItem::colorForState(const QString &state) {
  const QString s = state.toUpper();
  if (s == "OP")        return QColor(76, 175, 80);
  if (s == "SAFEOP")    return QColor(33, 150, 243);
  if (s == "SAFE-OP")   return QColor(33, 150, 243);
  if (s == "PREOP")     return QColor(255, 193, 7);
  if (s == "PRE-OP")    return QColor(255, 193, 7);
  if (s == "INIT")      return QColor(244, 67, 54);
  if (s == "ERROR")     return QColor(244, 67, 54);
  return QColor(158, 158, 158);
}
