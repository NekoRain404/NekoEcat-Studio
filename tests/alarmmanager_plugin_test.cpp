// AlarmManagerPluginTest — Tests for AlarmManagerPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Alarm rules CRUD
//   - Alarm records CRUD
//   - Notification channels
//   - Escalation policies

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include "plugins/alarmmanager/AlarmManagerPlugin.h"

class AlarmManagerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, and default order
  void testPluginIdentity() {
    AlarmManagerPlugin plugin;
    QCOMPARE(plugin.id(), QString("alarmmanager"));
    QCOMPARE(plugin.displayName(), QString("Alarm Manager"));
    QCOMPARE(plugin.displayNameZh(), QString("告警管理器"));
    QCOMPARE(plugin.defaultOrder(), 240);
    QCOMPARE(plugin.visible(), true);
  }
  // Verify widget is created
  void testWidgetCreation() {
    AlarmManagerPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }
  // Verify initial counts are zero
  void testInitialState() {
    AlarmManagerPlugin plugin;
    QCOMPARE(plugin.ruleCount(), 0);
    QCOMPARE(plugin.recordCount(), 0);
    QCOMPARE(plugin.channelCount(), 0);
    QCOMPARE(plugin.escalationPolicyCount(), 0);
  }
  // Verify adding a rule increments count
  void testAddRule() {
    AlarmManagerPlugin plugin;
    AlarmManagerPlugin::AlarmRule rule;
    rule.id = "r1";
    rule.name = "High Temp";
    rule.condition = "temperature > 80";
    rule.severity = "warning";
    rule.enabled = true;
    rule.createdAt = QDateTime::currentDateTime();
    plugin.addRule(rule);
    QCOMPARE(plugin.ruleCount(), 1);
  }
  // Verify removing a rule decrements count
  void testRemoveRule() {
    AlarmManagerPlugin plugin;
    AlarmManagerPlugin::AlarmRule rule;
    rule.id = "r1";
    rule.name = "High Temp";
    rule.condition = "temperature > 80";
    rule.severity = "warning";
    rule.enabled = true;
    rule.createdAt = QDateTime::currentDateTime();
    plugin.addRule(rule);
    QCOMPARE(plugin.ruleCount(), 1);
    plugin.removeRule(0);
    QCOMPARE(plugin.ruleCount(), 0);
  }
  // Verify adding a record increments count
  void testAddRecord() {
    AlarmManagerPlugin plugin;
    AlarmManagerPlugin::AlarmRecord record;
    record.ruleId = "r1";
    record.message = "Temperature exceeded";
    record.severity = "warning";
    record.channel = "log";
    record.timestamp = QDateTime::currentDateTime();
    record.acknowledged = false;
    plugin.addRecord(record);
    QCOMPARE(plugin.recordCount(), 1);
  }
  // Verify removing a record decrements count
  void testRemoveRecord() {
    AlarmManagerPlugin plugin;
    AlarmManagerPlugin::AlarmRecord record;
    record.ruleId = "r1";
    record.message = "Temperature exceeded";
    record.severity = "warning";
    record.channel = "log";
    record.timestamp = QDateTime::currentDateTime();
    record.acknowledged = false;
    plugin.addRecord(record);
    QCOMPARE(plugin.recordCount(), 1);
    plugin.removeRecord(0);
    QCOMPARE(plugin.recordCount(), 0);
  }
  // Verify acknowledging a record emits signal
  void testAcknowledgeRecord() {
    AlarmManagerPlugin plugin;
    AlarmManagerPlugin::AlarmRecord record;
    record.ruleId = "r1";
    record.message = "Temperature exceeded";
    record.severity = "warning";
    record.channel = "log";
    record.timestamp = QDateTime::currentDateTime();
    record.acknowledged = false;
    plugin.addRecord(record);
    QSignalSpy spy(&plugin, &AlarmManagerPlugin::alarmAcknowledged);
    plugin.acknowledgeRecord(0);
    QCOMPARE(spy.count(), 1);
  }
  // Verify adding a notification channel
  void testAddChannel() {
    AlarmManagerPlugin plugin;
    plugin.addChannel("email");
    QCOMPARE(plugin.channelCount(), 1);
  }
  // Verify removing a notification channel
  void testRemoveChannel() {
    AlarmManagerPlugin plugin;
    plugin.addChannel("email");
    QCOMPARE(plugin.channelCount(), 1);
    plugin.removeChannel(0);
    QCOMPARE(plugin.channelCount(), 0);
  }
  // Verify adding an escalation policy
  void testAddEscalationPolicy() {
    AlarmManagerPlugin plugin;
    AlarmManagerPlugin::EscalationPolicy policy;
    policy.id = "e1";
    policy.name = "Escalate to admin";
    policy.delaySeconds = 300;
    policy.targetChannel = "email";
    policy.severity = "critical";
    plugin.addEscalationPolicy(policy);
    QCOMPARE(plugin.escalationPolicyCount(), 1);
  }
  // Verify removing an escalation policy
  void testRemoveEscalationPolicy() {
    AlarmManagerPlugin plugin;
    AlarmManagerPlugin::EscalationPolicy policy;
    policy.id = "e1";
    policy.name = "Escalate to admin";
    policy.delaySeconds = 300;
    policy.targetChannel = "email";
    policy.severity = "critical";
    plugin.addEscalationPolicy(policy);
    QCOMPARE(plugin.escalationPolicyCount(), 1);
    plugin.removeEscalationPolicy(0);
    QCOMPARE(plugin.escalationPolicyCount(), 0);
  }
  // Verify tabs widget exists
  void testTabs() {
    AlarmManagerPlugin plugin;
    QVERIFY(plugin.tabs() != nullptr);
  }
  // Verify rules table widget exists
  void testRulesTable() {
    AlarmManagerPlugin plugin;
    QVERIFY(plugin.rulesTable() != nullptr);
  }
  // Verify history table widget exists
  void testHistoryTable() {
    AlarmManagerPlugin plugin;
    QVERIFY(plugin.historyTable() != nullptr);
  }
  // Verify channels table widget exists
  void testChannelsTable() {
    AlarmManagerPlugin plugin;
    QVERIFY(plugin.channelsTable() != nullptr);
  }
  // Verify escalation table widget exists
  void testEscalationTable() {
    AlarmManagerPlugin plugin;
    QVERIFY(plugin.escalationTable() != nullptr);
  }
  // Verify export alarm data produces non-empty JSON
  void testExportAlarmData() {
    AlarmManagerPlugin plugin;
    AlarmManagerPlugin::AlarmRule rule;
    rule.id = "r1";
    rule.name = "High Temp";
    rule.condition = "temperature > 80";
    rule.severity = "warning";
    rule.enabled = true;
    rule.createdAt = QDateTime::currentDateTime();
    plugin.addRule(rule);
    AlarmManagerPlugin::AlarmRecord record;
    record.ruleId = "r1";
    record.message = "Temperature exceeded";
    record.severity = "warning";
    record.channel = "log";
    record.timestamp = QDateTime::currentDateTime();
    record.acknowledged = false;
    plugin.addRecord(record);
    QString json = plugin.exportAlarmData();
    QVERIFY(!json.isEmpty());
  }
  // Verify status label widget exists
  void testStatusLabel() {
    AlarmManagerPlugin plugin;
    QVERIFY(plugin.statusLabel() != nullptr);
  }
};

QTEST_MAIN(AlarmManagerPluginTest)
#include "alarmmanager_plugin_test.moc"
