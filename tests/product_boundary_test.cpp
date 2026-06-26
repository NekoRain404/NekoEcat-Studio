#include <QFile>
#include <QRegularExpression>
#include <QString>
#include <QTest>

class ProductBoundaryTest : public QObject {
  Q_OBJECT

private slots:
  void experimentalServicesAreOptInByDefault();
  void experimentalSourcesAreCompileGuarded();
  void experimentalPluginsAreCompileGuarded();

private:
  static QString readTextFile(const QString &path);
};

QString ProductBoundaryTest::readTextFile(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qFatal("Unable to open %s", qPrintable(path));
  }
  return QString::fromUtf8(file.readAll());
}

void ProductBoundaryTest::experimentalServicesAreOptInByDefault() {
  const QString cmake = readTextFile(
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/CMakeLists.txt"));
  const QRegularExpression optionPattern(QStringLiteral(
      R"(^\s*option\s*\(\s*ECAT_EXPERIMENTAL_SERVICES\b.*\b(ON|OFF)\s*\)\s*$)"),
      QRegularExpression::MultilineOption);
  const QRegularExpressionMatch match = optionPattern.match(cmake);

  QVERIFY2(match.hasMatch(), "ECAT_EXPERIMENTAL_SERVICES option is missing.");
  QCOMPARE(match.captured(1), QStringLiteral("OFF"));
}

void ProductBoundaryTest::experimentalSourcesAreCompileGuarded() {
  const QString cmake = readTextFile(
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/CMakeLists.txt"));
  const QStringList sourcePaths = {
      QStringLiteral("plugins/cloudmanager/CloudManagerPlugin.cpp"),
      QStringLiteral("plugins/edgecomputing/EdgeComputingPlugin.cpp"),
      QStringLiteral("plugins/aiassistant/AIAssistantPlugin.cpp"),
      QStringLiteral("plugins/digitaltwinstudio/DigitalTwinStudioPlugin.cpp"),
      QStringLiteral("plugins/blockchainexplorer/BlockchainExplorerPlugin.cpp"),
      QStringLiteral("plugins/quantumsecurity/QuantumSecurityPlugin.cpp"),
      QStringLiteral("services/EtherCATCloudService.cpp"),
      QStringLiteral("services/EtherCATEdgeService.cpp"),
      QStringLiteral("services/EtherCATAIService.cpp"),
      QStringLiteral("services/EtherCATDigitalTwinService.cpp"),
      QStringLiteral("services/EtherCATBlockchainService.cpp"),
      QStringLiteral("services/EtherCATQuantumService.cpp"),
      QStringLiteral("services/WorkflowComplianceService.cpp"),
      QStringLiteral("services/WorkflowCertificationService.cpp"),
      QStringLiteral("services/WorkflowDeploymentService.cpp"),
      QStringLiteral("services/WorkflowReplicationService.cpp"),
      QStringLiteral("services/WorkflowCloudService.cpp"),
      QStringLiteral("services/WorkflowEdgeService.cpp"),
      QStringLiteral("services/WorkflowAIService.cpp"),
      QStringLiteral("services/WorkflowDigitalTwinService.cpp"),
      QStringLiteral("services/WorkflowBlockchainService.cpp"),
      QStringLiteral("services/WorkflowQuantumService.cpp"),
  };

  for (const QString &sourcePath : sourcePaths) {
    const qsizetype sourceIndex = cmake.indexOf(sourcePath);
    QVERIFY2(sourceIndex >= 0,
             qPrintable(QStringLiteral("Missing source entry for %1")
                            .arg(sourcePath)));

    const qsizetype guardIndex = cmake.lastIndexOf(
        QStringLiteral("if(ECAT_EXPERIMENTAL_SERVICES)"), sourceIndex);
    const qsizetype endGuardIndex =
        cmake.lastIndexOf(QStringLiteral("endif()"), sourceIndex);
    QVERIFY2(guardIndex > endGuardIndex,
             qPrintable(QStringLiteral("%1 must be inside "
                                       "ECAT_EXPERIMENTAL_SERVICES CMake guard")
                            .arg(sourcePath)));
  }
}

void ProductBoundaryTest::experimentalPluginsAreCompileGuarded() {
  const QString mainWindow = readTextFile(
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/MainWindow.cpp"));
  const QStringList pluginTypes = {
      QStringLiteral("CloudManagerPlugin"),
      QStringLiteral("EdgeComputingPlugin"),
      QStringLiteral("AIAssistantPlugin"),
      QStringLiteral("DigitalTwinStudioPlugin"),
      QStringLiteral("BlockchainExplorerPlugin"),
      QStringLiteral("QuantumSecurityPlugin"),
  };

  for (const QString &pluginType : pluginTypes) {
    const QString registration =
        QStringLiteral("registerPlugin(new %1").arg(pluginType);
    const qsizetype registrationIndex = mainWindow.indexOf(registration);
    QVERIFY2(registrationIndex >= 0,
             qPrintable(QStringLiteral("Missing registration for %1")
                            .arg(pluginType)));

    const qsizetype guardIndex = mainWindow.lastIndexOf(
        QStringLiteral("#ifdef ECAT_EXPERIMENTAL_SERVICES"), registrationIndex);
    const qsizetype endGuardIndex =
        mainWindow.lastIndexOf(QStringLiteral("#endif"), registrationIndex);
    QVERIFY2(guardIndex > endGuardIndex,
             qPrintable(QStringLiteral("%1 registration must be inside "
                                       "ECAT_EXPERIMENTAL_SERVICES guard")
                            .arg(pluginType)));
  }
}

QTEST_MAIN(ProductBoundaryTest)
#include "product_boundary_test.moc"
