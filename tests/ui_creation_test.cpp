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

class UiCreationTest : public QObject {
  Q_OBJECT
private:
  ServiceContainer *container_ = nullptr;

private slots:
  void init() { container_ = new ServiceContainer(this); }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  void testNotesPluginWidget() {
    NotesPlugin p;
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
    QVERIFY(w->isVisible() == false);  // Initially hidden
  }

  void testStateMachinePluginWidget() {
    StateMachinePlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
    QVERIFY(p.table() != nullptr);
    QVERIFY(p.summaryLabel() != nullptr);
    QVERIFY(p.detailLabel() != nullptr);
  }

  void testSessionPluginWidget() {
    SessionPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testOverviewPluginWidget() {
    OverviewPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testIoVariablePluginWidget() {
    IoVariablePlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testRtTestPluginWidget() {
    RtTestPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testWatchPluginWidget() {
    WatchPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testExportPluginWidget() {
    ExportPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testOdPluginWidget() {
    OdPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

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
