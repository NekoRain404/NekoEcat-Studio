#include "EtherCATVisualizationService.h"
#include "EtherCATAnalyticsService.h"
#include "EtherCATMonitorService.h"
#include "EventBus.h"
#include "infra/EcatClient.h"
#include "NetworkDiagnosticsService.h"

#include <QBrush>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPen>

// EtherCATVisualizationService.cpp — QGraphicsScene-based visualization for EtherCAT data
//
// Implementation notes:
//   - Four view types: topology (master→slave chain), data (bar chart), performance (KPI cards), errors
//   - Color schemes configurable via VisualizationConfig with sensible defaults
//   - Scenes are self-contained QGraphicsScene objects suitable for embedding in views

EtherCATVisualizationService::EtherCATVisualizationService(EventBus* bus, EcatClient* client, QObject* parent)
    : QObject(parent), bus_(bus), client_(client) {
    config_.viewType = QStringLiteral("default");
    config_.layout = QStringLiteral("horizontal");
    config_.animations = false;
    config_.interactions = true;
}

void EtherCATVisualizationService::setConfig(const VisualizationConfig& config) {
    config_ = config;
}

QGraphicsScene* EtherCATVisualizationService::makeScene() {
    auto* scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 600);
    return scene;
}

QGraphicsScene* EtherCATVisualizationService::createTopologyView(const QVector<SlaveInfo>& slaves) {
    auto* scene = makeScene();

    QColor masterColor = config_.colors.value(QStringLiteral("master"), QColor(60, 120, 200));
    QColor slaveColor = config_.colors.value(QStringLiteral("slave"), QColor(80, 180, 80));
    QColor lineColor = config_.colors.value(QStringLiteral("line"), QColor(100, 100, 100));

    auto* masterBox = new QGraphicsRectItem(0, 250, 100, 100);
    masterBox->setBrush(QBrush(masterColor));
    masterBox->setPen(QPen(Qt::black));
    scene->addItem(masterBox);

    auto* masterLabel = new QGraphicsTextItem(QStringLiteral("Master"));
    masterLabel->setPos(20, 280);
    scene->addItem(masterLabel);

    double xPos = 150.0;
    for (const auto& slave : slaves) {
        auto* line = new QGraphicsLineItem(100, 300, xPos, 300);
        line->setPen(QPen(lineColor, 2));
        scene->addItem(line);

        auto* box = new QGraphicsRectItem(xPos, 250, 100, 100);
        box->setBrush(QBrush(slaveColor));
        box->setPen(QPen(Qt::black));
        scene->addItem(box);

        auto* label = new QGraphicsTextItem(QStringLiteral("%1\n%2").arg(slave.position).arg(slave.name));
        label->setPos(xPos + 5, 270);
        scene->addItem(label);

        xPos += 150.0;
    }

    scene->setSceneRect(0, 0, xPos + 50, 600);
    emit viewCreated(QStringLiteral("topology"));
    return scene;
}

QGraphicsScene* EtherCATVisualizationService::createDataView(const QVector<DataPoint>& data) {
    auto* scene = makeScene();

    if (data.isEmpty()) {
        auto* label = new QGraphicsTextItem(QStringLiteral("No data available"));
        label->setPos(300, 280);
        scene->addItem(label);
        emit viewCreated(QStringLiteral("data"));
        return scene;
    }

    double minVal = data.first().value;
    double maxVal = data.first().value;
    for (const auto& dp : data) {
        minVal = qMin(minVal, dp.value);
        maxVal = qMax(maxVal, dp.value);
    }
    double range = maxVal - minVal;
    if (range < 1e-9)
        range = 1.0;

    QColor barColor = config_.colors.value(QStringLiteral("bar"), QColor(60, 140, 220));
    double barWidth = qMax(4.0, 700.0 / data.size());
    double xStep = 750.0 / data.size();

    for (int i = 0; i < data.size(); ++i) {
        double normalized = (data[i].value - minVal) / range;
        double barHeight = normalized * 500;
        double x = 25 + i * xStep;
        double y = 550 - barHeight;

        auto* bar = new QGraphicsRectItem(x, y, barWidth, barHeight);
        bar->setBrush(QBrush(barColor));
        bar->setPen(QPen(Qt::NoPen));
        scene->addItem(bar);
    }

    emit viewCreated(QStringLiteral("data"));
    return scene;
}

QGraphicsScene* EtherCATVisualizationService::createPerformanceView(const PerformanceMetrics& metrics) {
    auto* scene = makeScene();

    QColor goodColor = config_.colors.value(QStringLiteral("good"), QColor(80, 180, 80));
    QColor warnColor = config_.colors.value(QStringLiteral("warning"), QColor(220, 180, 40));
    QColor critColor = config_.colors.value(QStringLiteral("critical"), QColor(220, 60, 60));

    auto addMetric = [&](double x, double y, const QString& name, double value, const QString& unit) {
        QColor color = goodColor;
        if (name.contains(QStringLiteral("jitter"), Qt::CaseInsensitive) ||
            name.contains(QStringLiteral("loss"), Qt::CaseInsensitive)) {
            if (value > 10.0)
                color = critColor;
            else if (value > 1.0)
                color = warnColor;
        }

        auto* box = new QGraphicsRectItem(x, y, 170, 80);
        box->setBrush(QBrush(color));
        box->setPen(QPen(Qt::black));
        scene->addItem(box);

        auto* title = new QGraphicsTextItem(name);
        title->setPos(x + 10, y + 5);
        title->setDefaultTextColor(Qt::white);
        scene->addItem(title);

        auto* val = new QGraphicsTextItem(QStringLiteral("%1 %2").arg(value, 0, 'f', 2).arg(unit));
        val->setPos(x + 10, y + 40);
        val->setDefaultTextColor(Qt::white);
        scene->addItem(val);
    };

    addMetric(20, 20, QStringLiteral("Cycle Time"), metrics.cycleTimeUs, QStringLiteral("us"));
    addMetric(210, 20, QStringLiteral("Jitter"), metrics.jitterUs, QStringLiteral("us"));
    addMetric(400, 20, QStringLiteral("Frame Loss"), metrics.frameLossRate, QStringLiteral("%"));
    addMetric(20, 120, QStringLiteral("SDO Response"), metrics.sdoResponseMs, QStringLiteral("ms"));
    addMetric(210, 120, QStringLiteral("PDO Update"), metrics.pdoUpdateRate, QStringLiteral("Hz"));

    emit viewCreated(QStringLiteral("performance"));
    return scene;
}

QGraphicsScene* EtherCATVisualizationService::createErrorView(const QVector<ErrorInfo>& errors) {
    auto* scene = makeScene();

    if (errors.isEmpty()) {
        auto* label = new QGraphicsTextItem(QStringLiteral("No errors detected"));
        label->setPos(300, 280);
        label->setDefaultTextColor(QColor(80, 180, 80));
        scene->addItem(label);
        emit viewCreated(QStringLiteral("errors"));
        return scene;
    }

    QColor errColor = config_.colors.value(QStringLiteral("error"), QColor(220, 60, 60));

    double y = 10;
    int count = qMin(errors.size(), 20);
    for (int i = 0; i < count; ++i) {
        const auto& err = errors[i];

        auto* box = new QGraphicsRectItem(10, y, 780, 25);
        box->setBrush(QBrush(errColor));
        box->setPen(QPen(Qt::NoPen));
        scene->addItem(box);

        auto* text = new QGraphicsTextItem(
            QStringLiteral("[%1] Port %2: %3 - %4").arg(err.timestampMs).arg(err.port).arg(err.type, err.description));
        text->setPos(15, y);
        text->setDefaultTextColor(Qt::white);
        scene->addItem(text);

        y += 30;
    }

    if (errors.size() > count) {
        auto* more = new QGraphicsTextItem(QStringLiteral("... and %1 more errors").arg(errors.size() - count));
        more->setPos(15, y);
        scene->addItem(more);
    }

    scene->setSceneRect(0, 0, 800, y + 40);
    emit viewCreated(QStringLiteral("errors"));
    return scene;
}
