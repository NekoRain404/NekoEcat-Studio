#include <QFile>
#include <QRegularExpression>
#include <QString>
#include <QTest>

class ProductBoundaryTest : public QObject {
  Q_OBJECT

private slots:
  void experimentalServicesAreOptInByDefault();
  void experimentalSourcesAreCompileGuarded();
  void experimentalIncludeDirsAreCompileGuarded();
  void experimentalTestsAreOptInByDefault();
  void experimentalPluginsAreCompileGuarded();
  void readmeMarksExperimentalWorkspaces();

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

void ProductBoundaryTest::experimentalIncludeDirsAreCompileGuarded() {
  const QString cmake = readTextFile(
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/CMakeLists.txt"));
  const QStringList includeDirs = {
      QStringLiteral("${CMAKE_CURRENT_SOURCE_DIR}/plugins/cloudmanager"),
      QStringLiteral("${CMAKE_CURRENT_SOURCE_DIR}/plugins/edgecomputing"),
      QStringLiteral("${CMAKE_CURRENT_SOURCE_DIR}/plugins/aiassistant"),
      QStringLiteral("${CMAKE_CURRENT_SOURCE_DIR}/plugins/digitaltwinstudio"),
      QStringLiteral("${CMAKE_CURRENT_SOURCE_DIR}/plugins/blockchainexplorer"),
      QStringLiteral("${CMAKE_CURRENT_SOURCE_DIR}/plugins/quantumsecurity"),
  };

  for (const QString &includeDir : includeDirs) {
    const qsizetype includeIndex = cmake.indexOf(includeDir);
    QVERIFY2(includeIndex >= 0,
             qPrintable(QStringLiteral("Missing include entry for %1")
                            .arg(includeDir)));

    const qsizetype guardIndex = cmake.lastIndexOf(
        QStringLiteral("if(ECAT_EXPERIMENTAL_SERVICES)"), includeIndex);
    const qsizetype endGuardIndex =
        cmake.lastIndexOf(QStringLiteral("endif()"), includeIndex);
    QVERIFY2(guardIndex > endGuardIndex,
             qPrintable(QStringLiteral("%1 must be inside "
                                       "ECAT_EXPERIMENTAL_SERVICES CMake guard")
                            .arg(includeDir)));
  }
}

void ProductBoundaryTest::experimentalTestsAreOptInByDefault() {
  const QString cmake = readTextFile(
      QStringLiteral(SOURCE_ROOT "/tests/CMakeLists.txt"));
  const QStringList testTargets = {
      QStringLiteral("cloudmanager_plugin_test"),
      QStringLiteral("edgecomputing_plugin_test"),
      QStringLiteral("aiassistant_plugin_test"),
      QStringLiteral("ethercat_cloud_service_test"),
      QStringLiteral("ethercat_edge_service_test"),
      QStringLiteral("ethercat_ai_service_test"),
      QStringLiteral("ethercat_cloud_performance_test"),
      QStringLiteral("ethercat_edge_performance_test"),
      QStringLiteral("ethercat_ai_performance_test"),
      QStringLiteral("ethercat_digital_twin_performance_test"),
      QStringLiteral("ethercat_blockchain_performance_test"),
      QStringLiteral("ethercat_quantum_performance_test"),
      QStringLiteral("ethercat_digital_twin_service_test"),
      QStringLiteral("ethercat_blockchain_service_test"),
      QStringLiteral("ethercat_quantum_service_test"),
      QStringLiteral("digitaltwinstudio_plugin_test"),
      QStringLiteral("blockchainexplorer_plugin_test"),
      QStringLiteral("quantumsecurity_plugin_test"),
      QStringLiteral("workflow_cloud_service_test"),
      QStringLiteral("workflow_edge_service_test"),
      QStringLiteral("workflow_ai_service_test"),
      QStringLiteral("workflow_cloud_service_performance_test"),
      QStringLiteral("workflow_edge_service_performance_test"),
      QStringLiteral("workflow_ai_service_performance_test"),
      QStringLiteral("workflow_digital_twin_service_test"),
      QStringLiteral("workflow_blockchain_service_test"),
      QStringLiteral("workflow_quantum_service_test"),
  };

  for (const QString &testTarget : testTargets) {
    const QString declaration = QStringLiteral("add_executable(%1").arg(testTarget);
    const qsizetype targetIndex = cmake.indexOf(declaration);
    QVERIFY2(targetIndex >= 0,
             qPrintable(QStringLiteral("Missing test target %1").arg(testTarget)));

    const qsizetype guardIndex = cmake.lastIndexOf(
        QStringLiteral("if(ECAT_EXPERIMENTAL_SERVICES)"), targetIndex);
    const qsizetype endGuardIndex =
        cmake.lastIndexOf(QStringLiteral("endif()"), targetIndex);
    QVERIFY2(guardIndex > endGuardIndex,
             qPrintable(QStringLiteral("%1 must be inside "
                                       "ECAT_EXPERIMENTAL_SERVICES CMake guard")
                            .arg(testTarget)));
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

void ProductBoundaryTest::readmeMarksExperimentalWorkspaces() {
  const QString readme = readTextFile(QStringLiteral(SOURCE_ROOT "/README.md"));
  QVERIFY2(readme.contains(QStringLiteral("Experimental note:")),
           "README must explicitly state that AI/Blockchain/Quantum/Cloud/"
           "Edge/Digital Twin surfaces are experimental and opt-in.");

  const QStringList workspaceNames = {
      QStringLiteral("Digital Twin Studio"),
      QStringLiteral("Blockchain Explorer"),
      QStringLiteral("Quantum Security"),
  };

  for (const QString &workspaceName : workspaceNames) {
    const QRegularExpression rowPattern(QStringLiteral(
        R"(^\|\s*%1\b[^\n]*\b(Experimental|实验)\b[^\n]*$)")
                                            .arg(QRegularExpression::escape(workspaceName)),
                                        QRegularExpression::MultilineOption);
    QVERIFY2(rowPattern.match(readme).hasMatch(),
             qPrintable(QStringLiteral("README workspace row for %1 must be marked "
                                       "Experimental/实验.")
                            .arg(workspaceName)));
  }
}

QTEST_MAIN(ProductBoundaryTest)
#include "product_boundary_test.moc"
