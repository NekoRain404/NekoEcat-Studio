#pragma once

// SlaveNodeItem — QGraphicsItem representing a single EtherCAT slave node
// in the topology graph. Displays position, name, vendor ID, and state
// with color-coded indication.

#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QRectF>
#include <QString>

class SlaveNodeItem : public QGraphicsObject {
    Q_OBJECT
public:
    enum { Type = QGraphicsItem::UserType + 1 };
    int type() const override { return Type; }

    explicit SlaveNodeItem(int position, const QString& name, const QString& state, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    int position() const { return position_; }
    QString slaveName() const { return name_; }
    QString state() const { return state_; }

    void setState(const QString& state);
    void setVendorId(const QString& vendorId);

signals:
    void clicked(int position);
    void doubleClicked(int position);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    static QColor colorForState(const QString& state);

    int position_;
    QString name_;
    QString state_;
    QString vendorId_;
};
