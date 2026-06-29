// ConsistencyPluginTest — Tests for ConsistencyPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Consistency table view updates
//   - Issue count aggregation
//   - Filter controls existence

#include <QTest>
#include <QTableWidget>
#include "plugins/consistency/ConsistencyPlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "adapters/ConsistencyTableAdapter.h"
#include "infra/EcatClient.h"

class ConsistencyPluginTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  ServiceContainer *container_ = nullptr;

private slots:
  void initTestCase() {
    client_ = new EcatClient(this);
    container_ = new ServiceContainer(client_, new EventBus(this), this);
  }

  // Verify plugin id, display names (EN/ZH)
  void testIdentity() {
    ConsistencyPlugin p(container_);
    QCOMPARE(p.id(), QString("consistency"));
    QCOMPARE(p.displayName(), QString("Consistency"));
    QCOMPARE(p.displayNameZh(), QString("一致性"));
  }

  // Verify default tab order value
  void testDefaultOrder() {
    ConsistencyPlugin p(container_);
    QCOMPARE(p.defaultOrder(), 67);
  }

  // Verify plugin is visible by default
  void testVisible() {
    ConsistencyPlugin p(container_);
    QVERIFY(p.visible());
  }

  // Verify main widget is created
  void testWidgetNotNull() {
    ConsistencyPlugin p(container_);
    QVERIFY(p.widget() != nullptr);
  }

  // Verify consistency table widget is created
  void testConsistencyTableNotNull() {
    ConsistencyPlugin p(container_);
    QVERIFY(p.consistencyTable() != nullptr);
  }

  // Verify table row count after updating with sample rows
  void testUpdateConsistencyView() {
    ConsistencyPlugin p(container_);
    QList<QStringList> rows;
    rows << QStringList{"Error", "Topology", "#1 Slave1",
                        "Baseline missing", "Online", "Missing",
                        "Check cabling"};
    rows << QStringList{"Warning", "I/O", "#2 0x6000:01",
                        "No value", "Evidence", "Missing",
                        "Refresh Watch"};
    rows << QStringList{"Ready", "Project", "Current evidence",
                        "No issues", "Consistent", "Consistent",
                        "Continue"};
    p.updateConsistencyView(rows);
    QCOMPARE(p.consistencyTable()->rowCount(), 3);
  }

  // Verify issue counts are correctly aggregated from table rows
  void testConsistencyIssueCounts() {
    ConsistencyPlugin p(container_);
    QList<QStringList> rows;
    rows << QStringList{"Error", "Topology", "#1", "E", "E", "A", "A"};
    rows << QStringList{"Warning", "I/O", "#2", "E", "E", "A", "A"};
    rows << QStringList{"Info", "I/O", "#3", "E", "E", "A", "A"};
    rows << QStringList{"Ready", "Project", "#4", "E", "E", "A", "A"};
    p.updateConsistencyView(rows);
    const ConsistencyIssueCounts counts = p.consistencyIssueCounts();
    QCOMPARE(counts.errors, 1);
    QCOMPARE(counts.warnings, 1);
    QCOMPARE(counts.infos, 1);
    QCOMPARE(counts.ready, 1);
  }

  // Verify filter, scope filter, and summary label widgets exist
  void testFilterControlsNotNull() {
    ConsistencyPlugin p(container_);
    QVERIFY(p.consistencyFilter() != nullptr);
    QVERIFY(p.consistencyScopeFilter() != nullptr);
    QVERIFY(p.consistencySummaryLabel() != nullptr);
  }
};

QTEST_MAIN(ConsistencyPluginTest)
#include "consistency_plugin_test.moc"
