#pragma once

#include <QVector>
#include <QWidget>

struct DiagramNode {
    double x;
    double y;
    QString name;
    QString type;
};

class DiagramWidget : public QWidget {
    Q_OBJECT
public:
    explicit DiagramWidget(QWidget* parent = nullptr);

    void addNode(double x, double y, const QString& name, const QString& type);
    void clearNodes();
    int nodeCount() const;
    const QVector<DiagramNode>& nodes() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<DiagramNode> nodes_;
};
