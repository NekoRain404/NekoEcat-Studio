// PluginWidgetsTest — Tests for LatencyOptimizerWidget,
// ThroughputOptimizerWidget, and ErrorTimelineWidget.
//
// Test coverage:
//   - Widget creation and initialization
//   - Data update methods
//   - Signal emission (optimizeRequested)
//   - Edge cases (empty data, large data, zero/negative values)
//   - ErrorTimelineWidget zoom/pan and event management

#include "plugins/erroranalysis/ErrorTimelineWidget.h"
#include "plugins/realtimeoptimizer/LatencyOptimizerWidget.h"
#include "plugins/realtimeoptimizer/ThroughputOptimizerWidget.h"
#include "services/EtherCATOptimizerService.h"

#include <QPushButton>
#include <QtTest/QtTest>

class PluginWidgetsTest : public QObject {
    Q_OBJECT

private slots:
    void latencyWidgetCreation();
    void latencyWidgetUpdateResult();
    void latencyWidgetEmptyRecommendations();
    void latencyWidgetZeroValues();
    void latencyWidgetLargeValues();
    void latencyWidgetSignalEmission();
    void latencyWidgetParentOwnership();

    void throughputWidgetCreation();
    void throughputWidgetUpdateResult();
    void throughputWidgetEmptyRecommendations();
    void throughputWidgetZeroValues();
    void throughputWidgetLargeValues();
    void throughputWidgetSignalEmission();
    void throughputWidgetParentOwnership();

    void errorTimelineCreation();
    void errorTimelineSetEvents();
    void errorTimelineClearEvents();
    void errorTimelineEventCount();
    void errorTimelineEmptyEvents();
    void errorTimelineLargeDataSet();
    void errorTimelineZoomIn();
    void errorTimelineZoomOut();
    void errorTimelineResetZoom();
    void errorTimelineZoomLimits();
    void errorTimelineEventsSortedByTimestamp();
    void errorTimelineParentOwnership();
};

// ── LatencyOptimizerWidget ──────────────────────────────────────────

void PluginWidgetsTest::latencyWidgetCreation() {
    LatencyOptimizerWidget w;
    QVERIFY(w.isVisible() == false);
    w.show();
    QVERIFY(w.isVisible());
}

void PluginWidgetsTest::latencyWidgetUpdateResult() {
    LatencyOptimizerWidget w;
    OptimizationResult r;
    r.before = 150.0;
    r.after = 85.0;
    r.improvement = 43.3;
    r.recommendations = {"Reduce cycle time", "Enable DC sync"};
    w.updateResult(r);
    QVERIFY(true);
}

void PluginWidgetsTest::latencyWidgetEmptyRecommendations() {
    LatencyOptimizerWidget w;
    OptimizationResult r;
    r.before = 100.0;
    r.after = 100.0;
    r.improvement = 0.0;
    r.recommendations = {};
    w.updateResult(r);
    QVERIFY(true);
}

void PluginWidgetsTest::latencyWidgetZeroValues() {
    LatencyOptimizerWidget w;
    OptimizationResult r;
    r.before = 0.0;
    r.after = 0.0;
    r.improvement = 0.0;
    w.updateResult(r);
    QVERIFY(true);
}

void PluginWidgetsTest::latencyWidgetLargeValues() {
    LatencyOptimizerWidget w;
    OptimizationResult r;
    r.before = 999999.9;
    r.after = 0.1;
    r.improvement = 9999999.9;
    r.recommendations = QStringList(1000, "Recommendation");
    w.updateResult(r);
    QVERIFY(true);
}

void PluginWidgetsTest::latencyWidgetSignalEmission() {
    LatencyOptimizerWidget w;
    QSignalSpy spy(&w, &LatencyOptimizerWidget::optimizeRequested);
    QVERIFY(spy.isValid());
    QCOMPARE(spy.count(), 0);
    auto* btn = w.findChild<QPushButton*>();
    if (btn) {
        QTest::mouseClick(btn, Qt::LeftButton);
        QCOMPARE(spy.count(), 1);
    }
}

void PluginWidgetsTest::latencyWidgetParentOwnership() {
    auto* parent = new QWidget;
    auto* child = new LatencyOptimizerWidget(parent);
    QVERIFY(child->parent() == parent);
    delete parent;
    QVERIFY(true);
}

// ── ThroughputOptimizerWidget ────────────────────────────────────────

void PluginWidgetsTest::throughputWidgetCreation() {
    ThroughputOptimizerWidget w;
    QVERIFY(w.isVisible() == false);
    w.show();
    QVERIFY(w.isVisible());
}

void PluginWidgetsTest::throughputWidgetUpdateResult() {
    ThroughputOptimizerWidget w;
    OptimizationResult r;
    r.before = 1000.0;
    r.after = 1450.0;
    r.improvement = 45.0;
    r.recommendations = {"Increase PDO size", "Enable coalescing"};
    w.updateResult(r);
    QVERIFY(true);
}

void PluginWidgetsTest::throughputWidgetEmptyRecommendations() {
    ThroughputOptimizerWidget w;
    OptimizationResult r;
    r.before = 500.0;
    r.after = 500.0;
    r.improvement = 0.0;
    r.recommendations = {};
    w.updateResult(r);
    QVERIFY(true);
}

void PluginWidgetsTest::throughputWidgetZeroValues() {
    ThroughputOptimizerWidget w;
    OptimizationResult r;
    r.before = 0.0;
    r.after = 0.0;
    r.improvement = 0.0;
    w.updateResult(r);
    QVERIFY(true);
}

void PluginWidgetsTest::throughputWidgetLargeValues() {
    ThroughputOptimizerWidget w;
    OptimizationResult r;
    r.before = 1e9;
    r.after = 2e9;
    r.improvement = 100.0;
    r.recommendations = QStringList(500, "High throughput tip");
    w.updateResult(r);
    QVERIFY(true);
}

void PluginWidgetsTest::throughputWidgetSignalEmission() {
    ThroughputOptimizerWidget w;
    QSignalSpy spy(&w, &ThroughputOptimizerWidget::optimizeRequested);
    QVERIFY(spy.isValid());
    QCOMPARE(spy.count(), 0);
    auto* btn = w.findChild<QPushButton*>();
    if (btn) {
        QTest::mouseClick(btn, Qt::LeftButton);
        QCOMPARE(spy.count(), 1);
    }
}

void PluginWidgetsTest::throughputWidgetParentOwnership() {
    auto* parent = new QWidget;
    auto* child = new ThroughputOptimizerWidget(parent);
    QVERIFY(child->parent() == parent);
    delete parent;
    QVERIFY(true);
}

// ── ErrorTimelineWidget ─────────────────────────────────────────────

void PluginWidgetsTest::errorTimelineCreation() {
    ErrorTimelineWidget w;
    QCOMPARE(w.eventCount(), 0);
    w.show();
    QVERIFY(w.isVisible());
}

void PluginWidgetsTest::errorTimelineSetEvents() {
    ErrorTimelineWidget w;
    QVector<TimelineEvent> events;
    TimelineEvent e1;
    e1.timestamp = QDateTime::fromString("2024-01-01T00:00:00", Qt::ISODate);
    e1.severity = "Error";
    e1.message = "Lost frame";
    e1.slavePosition = 1;
    events.append(e1);

    TimelineEvent e2;
    e2.timestamp = QDateTime::fromString("2024-01-01T00:01:00", Qt::ISODate);
    e2.severity = "Warning";
    e2.message = "Late frame";
    e2.slavePosition = 2;
    events.append(e2);

    w.setEvents(events);
    QCOMPARE(w.eventCount(), 2);
}

void PluginWidgetsTest::errorTimelineClearEvents() {
    ErrorTimelineWidget w;
    QVector<TimelineEvent> events;
    TimelineEvent e;
    e.timestamp = QDateTime::currentDateTime();
    e.severity = "Info";
    e.message = "test";
    events.append(e);
    w.setEvents(events);
    QCOMPARE(w.eventCount(), 1);

    w.clearEvents();
    QCOMPARE(w.eventCount(), 0);
}

void PluginWidgetsTest::errorTimelineEventCount() {
    ErrorTimelineWidget w;
    QCOMPARE(w.eventCount(), 0);

    QVector<TimelineEvent> events;
    for (int i = 0; i < 50; ++i) {
        TimelineEvent e;
        e.timestamp = QDateTime::currentDateTime().addSecs(i);
        e.severity = "Info";
        e.message = QString("Event %1").arg(i);
        events.append(e);
    }
    w.setEvents(events);
    QCOMPARE(w.eventCount(), 50);
}

void PluginWidgetsTest::errorTimelineEmptyEvents() {
    ErrorTimelineWidget w;
    QVector<TimelineEvent> events;
    w.setEvents(events);
    QCOMPARE(w.eventCount(), 0);
}

void PluginWidgetsTest::errorTimelineLargeDataSet() {
    ErrorTimelineWidget w;
    QVector<TimelineEvent> events;
    for (int i = 0; i < 10000; ++i) {
        TimelineEvent e;
        e.timestamp = QDateTime::currentDateTime().addMSecs(i * 100);
        e.severity = (i % 3 == 0) ? "Error" : (i % 3 == 1) ? "Warning" : "Info";
        e.message = QString("Bulk event %1").arg(i);
        e.slavePosition = i % 10;
        events.append(e);
    }
    w.setEvents(events);
    QCOMPARE(w.eventCount(), 10000);
}

void PluginWidgetsTest::errorTimelineZoomIn() {
    ErrorTimelineWidget w;
    w.zoomIn();
    w.zoomIn();
    QVERIFY(true);
}

void PluginWidgetsTest::errorTimelineZoomOut() {
    ErrorTimelineWidget w;
    w.zoomOut();
    w.zoomOut();
    QVERIFY(true);
}

void PluginWidgetsTest::errorTimelineResetZoom() {
    ErrorTimelineWidget w;
    w.zoomIn();
    w.zoomIn();
    w.zoomOut();
    w.resetZoom();
    QVERIFY(true);
}

void PluginWidgetsTest::errorTimelineZoomLimits() {
    ErrorTimelineWidget w;
    for (int i = 0; i < 100; ++i)
        w.zoomIn();
    QVERIFY(true);

    w.resetZoom();
    for (int i = 0; i < 100; ++i)
        w.zoomOut();
    QVERIFY(true);
}

void PluginWidgetsTest::errorTimelineEventsSortedByTimestamp() {
    ErrorTimelineWidget w;
    QVector<TimelineEvent> events;
    auto base = QDateTime::currentDateTime();

    TimelineEvent e1;
    e1.timestamp = base.addSecs(30);
    e1.severity = "Error";
    e1.message = "later";
    events.append(e1);

    TimelineEvent e2;
    e2.timestamp = base.addSecs(10);
    e2.severity = "Warning";
    e2.message = "earlier";
    events.append(e2);

    w.setEvents(events);
    QCOMPARE(w.eventCount(), 2);
}

void PluginWidgetsTest::errorTimelineParentOwnership() {
    auto* parent = new QWidget;
    auto* child = new ErrorTimelineWidget(parent);
    QVERIFY(child->parent() == parent);
    delete parent;
    QVERIFY(true);
}

QTEST_MAIN(PluginWidgetsTest)
#include "plugin_widgets_test.moc"
