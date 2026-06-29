// LogicAnalyzerPluginTest — Tests for LogicAnalyzerPlugin
//
// Test coverage:
//   - Plugin identity and default order
//   - Widget creation and visibility
//   - Start fails closed when TraceService has no capture backend
//   - Protocol decode fails closed without captured evidence
//   - Source does not keep random synthetic-data dependencies

#include <QFile>
#include <QTest>
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include "services/TraceService.h"
#include "plugins/logicanalyzer/LogicAnalyzerPlugin.h"

class LogicAnalyzerPluginTest : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() {
    service = new TraceService(this);
    plugin = new LogicAnalyzerPlugin(service, this);
  }

  void cleanupTestCase() {
    delete plugin;
    delete service;
  }

  // Verify plugin id, display names, order, visibility
  void testPluginIdentity() {
    QCOMPARE(plugin->id(), QString("logicanalyzer"));
    QCOMPARE(plugin->displayName(), QString("Logic Analyzer"));
    QCOMPARE(plugin->defaultOrder(), 185);
    QVERIFY(plugin->visible());
  }

  // Widget is created and not null
  // Check widget is created
  void testWidgetNotNull() {
    QVERIFY(plugin->widget() != nullptr);
  }

  void testStartDoesNotShowRunningWithoutTraceBackend() {
    QWidget *widget = plugin->widget();
    auto *start = widget->findChild<QPushButton *>(QString(), Qt::FindDirectChildrenOnly);
    const auto buttons = widget->findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
      if (button->text() == QStringLiteral("Start")) {
        start = button;
        break;
      }
    }
    QVERIFY(start != nullptr);

    QTest::mouseClick(start, Qt::LeftButton);

    QVERIFY(!service->isTracing());
    const auto labels = widget->findChildren<QLabel *>();
    bool sawStoppedStatus = false;
    for (const QLabel *label : labels) {
      QVERIFY(label->text() != QStringLiteral("Status: Running"));
      if (label->text().contains(QStringLiteral("Stopped")) ||
          label->text().contains(QStringLiteral("backend"))) {
        sawStoppedStatus = true;
      }
    }
    QVERIFY(sawStoppedStatus);
    QVERIFY(start->isEnabled());
  }

  void testDecodeProtocolShowsEvidenceRequirement() {
    QWidget *widget = plugin->widget();
    QPushButton *decode = nullptr;
    const auto buttons = widget->findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
      if (button->text() == QStringLiteral("Decode Protocol")) {
        decode = button;
        break;
      }
    }
    QVERIFY(decode != nullptr);

    QTest::mouseClick(decode, Qt::LeftButton);

    bool sawDecodeStatus = false;
    const auto labels = widget->findChildren<QLabel *>();
    for (const QLabel *label : labels) {
      QVERIFY(!label->text().contains(QStringLiteral("Not implemented")));
      if (label->text().contains(QStringLiteral("Protocol decode")) &&
          (label->text().contains(QStringLiteral("capture")) ||
           label->text().contains(QStringLiteral("evidence")) ||
           label->text().contains(QStringLiteral("backend")))) {
        sawDecodeStatus = true;
      }
    }
    QVERIFY(sawDecodeStatus);
  }

  void testSourceDoesNotContainSyntheticLogicDependencies() {
    QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/logicanalyzer/LogicAnalyzerPlugin.cpp"));
    QVERIFY(source.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString text = QString::fromUtf8(source.readAll());

    QVERIFY2(!text.contains(QStringLiteral("QRandomGenerator")),
             "Logic analyzer must not keep random synthetic capture dependencies");
    QVERIFY2(!text.contains(QStringLiteral("Not implemented yet")),
             "Stable logic analyzer UI must not expose unfinished placeholder text.");
  }

private:
  TraceService *service = nullptr;
  LogicAnalyzerPlugin *plugin = nullptr;
};

QTEST_MAIN(LogicAnalyzerPluginTest)
#include "logicanalyzer_plugin_test.moc"
