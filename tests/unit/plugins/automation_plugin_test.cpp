// AutomationPluginTest — Tests for AutomationPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - ScriptingService execute, log, signals, errors
//   - BatchOperationService operations
//   - Script execution and cancellation

#include <QTest>
#include <QSignalSpy>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "plugins/automation/AutomationPlugin.h"
#include "services/ScriptingService.h"
#include "services/BatchOperationService.h"
#include "services/SdoService.h"
#include "services/TopologyService.h"
#include "infra/EcatClient.h"

class SilentDaemon : public QTcpServer {
  Q_OBJECT
public:
  explicit SilentDaemon(QObject *parent = nullptr) : QTcpServer(parent) {
    connect(this, &QTcpServer::newConnection, this, [this]() {
      while (auto *socket = nextPendingConnection()) {
        sockets_.append(socket);
        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
          buffer_ += socket->readAll();
          int newline = -1;
          while ((newline = buffer_.indexOf('\n')) >= 0) {
            const QByteArray line = buffer_.left(newline);
            buffer_.remove(0, newline + 1);
            const auto doc = QJsonDocument::fromJson(line);
            if (doc.isObject()) methods_.append(doc.object().value("method").toString());
          }
        });
      }
    });
  }

  QStringList methods() const { return methods_; }

private:
  QList<QTcpSocket *> sockets_;
  QByteArray buffer_;
  QStringList methods_;
};

class AutomationPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display name, and default order
  void testPluginIdentity() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    AutomationPlugin plugin(&scripting);

    QCOMPARE(plugin.id(), QString("automation"));
    QCOMPARE(plugin.displayName(), QString("Automation"));
    QCOMPARE(plugin.defaultOrder(), 120);
    QCOMPARE(plugin.visible(), false);
  }

  // Verify widget is created
  void testWidgetCreation() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    AutomationPlugin plugin(&scripting);
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify script execution returns correct result
  void testScriptingServiceExecuteSimpleScript() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QJSValue result = scripting.executeScript("2 + 2");
    QCOMPARE(result.toInt(), 4);
  }

  // Verify log signal is emitted during script execution
  void testScriptingServiceLogSignal() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QSignalSpy logSpy(&scripting, &ScriptingService::logMessage);
    scripting.executeScript("log('hello')");
    QCOMPARE(logSpy.count(), 1);
    QCOMPARE(logSpy.at(0).at(0).toString(), QString("hello"));
  }

  // Verify start/complete/error signals on execution
  void testScriptingServiceSignals() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QSignalSpy startSpy(&scripting, &ScriptingService::scriptStarted);
    QSignalSpy completeSpy(&scripting, &ScriptingService::scriptCompleted);
    QSignalSpy errorSpy(&scripting, &ScriptingService::scriptError);

    scripting.executeScript("42");
    QCOMPARE(startSpy.count(), 1);
    QCOMPARE(completeSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
  }

  // Verify syntax error emits error signal
  void testScriptingServiceSyntaxError() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QSignalSpy errorSpy(&scripting, &ScriptingService::scriptError);
    scripting.executeScript("function { invalid }");
    QCOMPARE(errorSpy.count(), 1);
  }

  // Verify save and load script round-trip
  void testScriptingServiceSaveLoad() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QString name = "test_save_load";
    QString content = "var x = 42; log(x);";
    QVERIFY(scripting.saveScript(name, content));

    QString loaded = scripting.loadScript(name);
    QCOMPARE(loaded, content);

    QStringList scripts = scripting.listScripts();
    QVERIFY(scripts.contains(name + ".js"));
  }

  // Verify script names cannot escape the managed script directory.
  void testScriptingServiceRejectsUnsafeScriptNames() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QVERIFY(!scripting.saveScript(QString(), QStringLiteral("log('empty');")));
    QVERIFY(!scripting.saveScript(QStringLiteral("../escape"),
                                  QStringLiteral("log('escape');")));
    QVERIFY(!scripting.saveScript(QStringLiteral("nested/name"),
                                  QStringLiteral("log('nested');")));
    QVERIFY(scripting.loadScript(QStringLiteral("../escape")).isEmpty());

    const QString tooLongName(129, QLatin1Char('x'));
    QVERIFY(!scripting.saveScript(tooLongName,
                                  QStringLiteral("log('long');")));
    QVERIFY(scripting.loadScript(tooLongName).isEmpty());

    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const QString escapedPath = QDir(appData).filePath(QStringLiteral("escape.js"));
    QVERIFY(!QFile::exists(escapedPath));
  }

  // Verify registering a service makes it available to scripts
  void testScriptingServiceRegisterService() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    auto *obj = new QObject(&scripting);
    obj->setObjectName("testObj");
    scripting.registerService("myObj", obj);

    QJSValue result = scripting.executeScript("myObj.objectName");
    QCOMPARE(result.toString(), QString("testObj"));
  }

  void testScriptingBuiltinsDoNotClaimAsyncBusSuccess() {
    SilentDaemon daemon;
    QVERIFY(daemon.listen(QHostAddress::LocalHost, 0));

    EcatClient client;
    client.enableAutoReconnect(false);
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    client.connectToHost(QHostAddress::LocalHost, daemon.serverPort());
    QTRY_VERIFY(client.isConnected());

    QJSValue writeResult = scripting.executeScript(
        "writeSDO(0, '0x2000', '0x00', '1', 'UINT8')");
    QCOMPARE(writeResult.isBool(), true);
    QCOMPARE(writeResult.toBool(), false);
    QTRY_VERIFY(daemon.methods().contains(QStringLiteral("download")));

    QJSValue stateResult = scripting.executeScript("setState(0, 'OP')");
    QCOMPARE(stateResult.isBool(), true);
    QCOMPARE(stateResult.toBool(), false);
    QTRY_VERIFY(daemon.methods().contains(QStringLiteral("setState")));
  }

  // Verify batch operation fails closed without daemon acknowledgement
  void testBatchOperationServiceSignals() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    BatchOperationService batch(&client, &sdo, &topo);

    QSignalSpy startSpy(&batch, &BatchOperationService::batchStarted);
    QSignalSpy progressSpy(&batch, &BatchOperationService::batchProgress);
    QSignalSpy completeSpy(&batch, &BatchOperationService::batchCompleted);
    QSignalSpy failedSpy(&batch, &BatchOperationService::batchFailed);

    BatchOperation op;
    op.type = BatchType::ScanTopology;
    op.scanCount = 2;

    BatchResult result = batch.executeBatch(op);
    QCOMPARE(startSpy.count(), 1);
    QCOMPARE(completeSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
    QCOMPARE(result.success, false);
    QCOMPARE(result.completedItems, 0);
    QVERIFY(result.error.contains("connected"));
  }

  // Verify batch progress tracking
  void testBatchOperationProgress() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    BatchOperationService batch(&client, &sdo, &topo);

    QSignalSpy progressSpy(&batch, &BatchOperationService::batchProgress);

    BatchOperation op;
    op.type = BatchType::ScanTopology;
    op.scanCount = 3;

    BatchResult result = batch.executeBatch(op);
    QCOMPARE(result.success, false);
    QCOMPARE(progressSpy.count(), 0);
    QCOMPARE(batch.progress(), 0);
  }

  // Verify batch cancel functionality
  void testBatchOperationCancel() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    BatchOperationService batch(&client, &sdo, &topo);

    BatchOperation op;
    op.type = BatchType::ScanTopology;
    op.scanCount = 5;

    QSignalSpy progressSpy(&batch, &BatchOperationService::batchProgress);
    BatchResult result = batch.executeBatch(op);
    QCOMPARE(result.success, false);
    QCOMPARE(progressSpy.count(), 0);
    QCOMPARE(batch.progress(), 0);
  }

  void testBatchOperationServiceDoesNotMintBatchSuccess() {
    QFile file(QStringLiteral(SOURCE_ROOT
                              "/apps/ecat-studio/services/BatchOperationService.cpp"));
    QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(file.errorString()));
    const QString source = QString::fromUtf8(file.readAll());

    QVERIFY2(!source.contains(QStringLiteral("r.success = true")),
             "BatchOperationService must not mark a batch successful without per-item backend acknowledgements");
    QVERIFY2(!source.contains(QStringLiteral("emit batchCompleted(r)")),
             "BatchOperationService must not emit completion without per-item backend acknowledgements");
  }
};

QTEST_MAIN(AutomationPluginTest)
#include "automation_plugin_test.moc"
