// UiCreationTest — Tests for UI widget creation across plugins
//
// Test coverage:
//   - Widget creation for Notes, StateMachine, Session, Overview
//   - Widget creation for IoVariable, RtTest, Watch, Export, OD plugins
//   - Table widget initial state validation

#include <QTest>
#include <QApplication>
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include "plugins/notes/NotesPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "plugins/session/SessionPlugin.h"
#include "plugins/overview/OverviewPlugin.h"
#include "plugins/iovariable/IoVariablePlugin.h"
#include "plugins/rttest/RtTestPlugin.h"
#include "plugins/watch/WatchPlugin.h"
#include "plugins/export/ExportPlugin.h"
#include "plugins/od/OdPlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class UiCreationTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  ServiceContainer *container_ = nullptr;

private slots:
  // Setup: create ServiceContainer
  void init() {
    client_ = new EcatClient(this);
    container_ = new ServiceContainer(client_, new EventBus(this), this);
  }
  // Teardown: destroy ServiceContainer
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  // NotesPlugin widget is non-null and initially hidden
  void testNotesPluginWidget() {
    NotesPlugin p;
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
    QVERIFY(w->isVisible() == false);  // Initially hidden
  }

  // StateMachinePlugin widget, table, and labels are non-null
  void testStateMachinePluginWidget() {
    StateMachinePlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
    QVERIFY(p.table() != nullptr);
    QVERIFY(p.summaryLabel() != nullptr);
    QVERIFY(p.detailLabel() != nullptr);
  }

  // SessionPlugin widget is non-null
  void testSessionPluginWidget() {
    SessionPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  // OverviewPlugin widget is non-null
  void testOverviewPluginWidget() {
    OverviewPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  // IoVariablePlugin widget is non-null
  void testIoVariablePluginWidget() {
    IoVariablePlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  // RtTestPlugin widget is non-null
  void testRtTestPluginWidget() {
    RtTestPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  // WatchPlugin widget is non-null
  void testWatchPluginWidget() {
    WatchPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  // ExportPlugin widget is non-null
  void testExportPluginWidget() {
    ExportPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  // OdPlugin widget is non-null
  void testOdPluginWidget() {
    OdPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  // StateMachinePlugin table starts with zero rows and columns
  void testPluginTableCreation() {
    StateMachinePlugin p(container_);
    QTableWidget *table = p.table();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 0);
    QCOMPARE(table->columnCount(), 0);
  }
};

QTEST_MAIN(UiCreationTest)
#include "ui_creation_test.moc"
