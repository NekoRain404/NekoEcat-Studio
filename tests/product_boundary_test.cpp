#include <QFile>
#include <QDirIterator>
#include <QRegularExpression>
#include <QSet>
#include <QString>
#include <QTest>

class ProductBoundaryTest : public QObject {
  Q_OBJECT

private slots:
  void experimentalServicesAreOptInByDefault();
  void experimentalSourcesAreCompileGuarded();
  void experimentalIncludeDirsAreCompileGuarded();
  void experimentalTestsAreOptInByDefault();
  void experimentalTestSourcesAreCompileGuarded();
  void experimentalPluginsAreCompileGuarded();
  void readmeMarksExperimentalWorkspaces();
  void docsMarkExperimentalSurfaces();
  void publicDocsDoNotUseStaleReleaseNumbers();
  void publicDocsDoNotAdvertiseStaleProjectStats();
  void digitalTwinPluginHasSingleCanonicalPathAndId();
  void managerPluginsHaveSingleCanonicalPathAndId();
  void pluginIdsAreUniqueAcrossSourceTree();
  void productPluginSourcesAreRegisteredOrExperimental();

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
      QStringLiteral("workflow_compliance_service_test"),
      QStringLiteral("workflow_certification_service_test"),
      QStringLiteral("workflow_deployment_service_test"),
      QStringLiteral("workflow_deployment_performance_test"),
      QStringLiteral("workflow_replication_service_test"),
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

void ProductBoundaryTest::experimentalTestSourcesAreCompileGuarded() {
  const QString cmake = readTextFile(
      QStringLiteral(SOURCE_ROOT "/tests/CMakeLists.txt"));
  const QStringList sourcePaths = {
      QStringLiteral("../apps/ecat-studio/plugins/cloudmanager/CloudManagerPlugin.cpp"),
      QStringLiteral("../apps/ecat-studio/plugins/edgecomputing/EdgeComputingPlugin.cpp"),
      QStringLiteral("../apps/ecat-studio/plugins/aiassistant/AIAssistantPlugin.cpp"),
      QStringLiteral("../apps/ecat-studio/plugins/digitaltwinstudio/DigitalTwinStudioPlugin.cpp"),
      QStringLiteral("../apps/ecat-studio/plugins/blockchainexplorer/BlockchainExplorerPlugin.cpp"),
      QStringLiteral("../apps/ecat-studio/plugins/quantumsecurity/QuantumSecurityPlugin.cpp"),
      QStringLiteral("../apps/ecat-studio/services/EtherCATCloudService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/EtherCATEdgeService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/EtherCATAIService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/EtherCATDigitalTwinService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/EtherCATBlockchainService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/EtherCATQuantumService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/WorkflowComplianceService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/WorkflowCertificationService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/WorkflowDeploymentService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/WorkflowReplicationService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/WorkflowCloudService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/WorkflowEdgeService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/WorkflowAIService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/WorkflowDigitalTwinService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/WorkflowBlockchainService.cpp"),
      QStringLiteral("../apps/ecat-studio/services/WorkflowQuantumService.cpp"),
  };

  for (const QString &sourcePath : sourcePaths) {
    qsizetype sourceIndex = cmake.indexOf(sourcePath);
    QVERIFY2(sourceIndex >= 0,
             qPrintable(QStringLiteral("Missing test source entry for %1")
                            .arg(sourcePath)));

    while (sourceIndex >= 0) {
      const qsizetype guardIndex = cmake.lastIndexOf(
          QStringLiteral("if(ECAT_EXPERIMENTAL_SERVICES)"), sourceIndex);
      const qsizetype endGuardIndex =
          cmake.lastIndexOf(QStringLiteral("endif()"), sourceIndex);
      QVERIFY2(guardIndex > endGuardIndex,
               qPrintable(QStringLiteral("%1 test source must be inside "
                                         "ECAT_EXPERIMENTAL_SERVICES CMake guard")
                              .arg(sourcePath)));

      sourceIndex = cmake.indexOf(sourcePath, sourceIndex + sourcePath.size());
    }
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

void ProductBoundaryTest::docsMarkExperimentalSurfaces() {
  const QStringList docPaths = {
      QStringLiteral(SOURCE_ROOT "/docs/ARCHITECTURE.md"),
      QStringLiteral(SOURCE_ROOT "/docs/PLUGIN_GUIDE.md"),
  };
  const QStringList surfaceNames = {
      QStringLiteral("CloudManagerPlugin"),
      QStringLiteral("EdgeComputingPlugin"),
      QStringLiteral("AIAssistantPlugin"),
      QStringLiteral("DigitalTwinStudioPlugin"),
      QStringLiteral("BlockchainExplorerPlugin"),
      QStringLiteral("QuantumSecurityPlugin"),
      QStringLiteral("EtherCATCloudService"),
      QStringLiteral("EtherCATEdgeService"),
      QStringLiteral("EtherCATAIService"),
      QStringLiteral("EtherCATDigitalTwinService"),
      QStringLiteral("EtherCATBlockchainService"),
      QStringLiteral("EtherCATQuantumService"),
  };

  for (const QString &docPath : docPaths) {
    const QString doc = readTextFile(docPath);
    for (const QString &surfaceName : surfaceNames) {
      if (!doc.contains(surfaceName)) {
        continue;
      }

      const QRegularExpression rowPattern(
          QStringLiteral(R"(^\|[^\n]*\b%1\b[^\n]*\b(Experimental|opt-in|实验)\b[^\n]*$)")
              .arg(QRegularExpression::escape(surfaceName)),
          QRegularExpression::MultilineOption);
      QVERIFY2(rowPattern.match(doc).hasMatch(),
               qPrintable(QStringLiteral("%1 must mark %2 as Experimental/opt-in.")
                              .arg(docPath, surfaceName)));
    }
  }
}

void ProductBoundaryTest::publicDocsDoNotUseStaleReleaseNumbers() {
  const QStringList docPaths = {
      QStringLiteral(SOURCE_ROOT "/docs/USER_MANUAL.md"),
      QStringLiteral(SOURCE_ROOT "/docs/INSTALLATION.md"),
      QStringLiteral(SOURCE_ROOT "/docs/DEVELOPER_GUIDE.md"),
  };

  for (const QString &docPath : docPaths) {
    const QString doc = readTextFile(docPath);
    QVERIFY2(!doc.contains(QStringLiteral("v3.5.0")),
             qPrintable(QStringLiteral("%1 must not advertise stale v3.5.0 release artifacts.")
                            .arg(docPath)));
    QVERIFY2(!doc.contains(QStringLiteral("3.5.0")),
             qPrintable(QStringLiteral("%1 must not advertise stale 3.5.0 release artifacts.")
                            .arg(docPath)));
  }
}

void ProductBoundaryTest::publicDocsDoNotAdvertiseStaleProjectStats() {
  const QStringList docPaths = {
      QStringLiteral(SOURCE_ROOT "/README.md"),
      QStringLiteral(SOURCE_ROOT "/docs/ARCHITECTURE.md"),
      QStringLiteral(SOURCE_ROOT "/docs/PLUGIN_GUIDE.md"),
      QStringLiteral(SOURCE_ROOT "/docs/PROJECT_OVERVIEW.md"),
      QStringLiteral(SOURCE_ROOT "/docs/TWINCAT_BENCHMARK_REVIEW.md"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/AGENTS.md"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/infra/AGENTS.md"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/workspaces/AGENTS.md"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/MainWindow.cpp"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/MainWindow.h"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/workspaces/MainWindowManual.cpp"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/workspaces/MainWindowIncludes.h"),
  };
  const QStringList staleClaims = {
      QStringLiteral("325+ registered tests"),
      QStringLiteral("325+ 个注册测试"),
      QStringLiteral("325+ 测试"),
      QStringLiteral("**Total registered** | **325+**"),
      QStringLiteral("| **注册测试** | 325+ |"),
      QStringLiteral("| **测试数量** | 325+ |"),
      QStringLiteral("| **源文件总数** | 10,532 |"),
      QStringLiteral("| **测试文件** | 281 |"),
      QStringLiteral("测试套件（281 个文件）"),
      QStringLiteral("286 个 CTest"),
      QStringLiteral("286 CTest entries"),
      QStringLiteral("285 个 CTest"),
      QStringLiteral("285 CTest entries"),
      QStringLiteral("285/285 CTest"),
      QStringLiteral("默认稳定注册测试** | 285"),
      QStringLiteral("| **Stable default registered** | **286**"),
      QStringLiteral("| **默认稳定注册测试** | 286 |"),
      QStringLiteral("97 个独立插件"),
      QStringLiteral("插件（97 个）"),
      QStringLiteral("90 个插件目录"),
      QStringLiteral("工作区插件（90 个目录"),
      QStringLiteral("90+ 插件"),
      QStringLiteral("50+ plugins"),
      QStringLiteral("50+ workspace plugins"),
      QStringLiteral("50+ repeated includes"),
      QStringLiteral("322+ 测试"),
      QStringLiteral("361 个测试源文件"),
      QStringLiteral("测试套件（361 个测试源文件"),
      QStringLiteral("| **测试文件** | 361 |"),
      QStringLiteral("| **测试源文件** | 361 |"),
      QStringLiteral("285 个默认稳定 CTest"),
      QStringLiteral("106K+ 行代码"),
      QStringLiteral("| **总代码行数** | 106K+ |"),
      QStringLiteral("FoE 存根"),
      QStringLiteral("无法进行固件更新"),
      QStringLiteral("80+ 存根服务"),
      QStringLiteral("80+ other services"),
      QStringLiteral("100+ domain-specific services"),
      QStringLiteral("120+ services total"),
      QStringLiteral("130+ 个领域服务"),
      QStringLiteral("130+ domain services"),
      QStringLiteral("| **扩展服务** | 20+ |"),
      QStringLiteral("| **工作流服务** | 20+ |"),
      QStringLiteral("无原生 EtherCAT API"),
      QStringLiteral("实现原生 IgH API"),
      QStringLiteral("实现原生 IgH ecrt API"),
      QStringLiteral("实现原生 ecrt API"),
      QStringLiteral("NekoEcat-Studio-v3.7.0-linux-x86_64"),
      QStringLiteral("**版本**: 3.7.0"),
      QStringLiteral("| **测试通过率** | 100% |"),
      QStringLiteral("| **通过率** | 100% |"),
      QStringLiteral("| **构建警告** | 0 |"),
      QStringLiteral("全部验证通过"),
      QStringLiteral("CMake 3.22+"),
      QStringLiteral("TwinCAT 95/100 vs NekoEcat 68/100"),
      QStringLiteral("达到 85%"),
      QStringLiteral("达到 70%"),
      QStringLiteral("达到 50%"),
      QStringLiteral("达到 75%"),
      QStringLiteral("达到 20%"),
      QStringLiteral("代码质量行业领先"),
      QStringLiteral("代码质量**超越行业标准"),
      QStringLiteral("1264 entries"),
      QStringLiteral("100% coverage"),
      QStringLiteral("8 languages supported"),
      QStringLiteral("8 语言 1264 条目"),
      QStringLiteral("MainWindow 的 partial 实现（31 个文件）"),
      QStringLiteral("UI 主题（12 个 .qss 文件）"),
      QStringLiteral("i18n 翻译文件（8 种语言）"),
      QStringLiteral("31 个 MainWindow 分部文件"),
      QStringLiteral("目标是超越 TwinCAT"),
      QStringLiteral("逐步超越传统工程软件"),
      QStringLiteral("无限的扩展可能"),
      QStringLiteral("独特的竞争优势"),
      QStringLiteral("最好的 EtherCAT 调试工具"),
      QStringLiteral("架构优秀、代码质量高"),
      QStringLiteral("向真正现场软件演进的 EtherCAT 工程平台"),
      QStringLiteral("moving toward practical field use"),
      QStringLiteral("real workstation: not a toy UI"),
      QStringLiteral("面向真实 EtherCAT 调试现场"),
      QStringLiteral("real commissioning work"),
      QStringLiteral("逐步上线"),
      QStringLiteral("controlled bring-up"),
      QStringLiteral("面向现场效率"),
      QStringLiteral("Built for field speed"),
      QStringLiteral("对标 TwinCAT 级别"),
      QStringLiteral("比传统调试面板更完整"),
      QStringLiteral("More than a debug panel"),
      QStringLiteral("Optimized for production"),
      QStringLiteral("强制使用 ecrt API"),
      QStringLiteral("需要最高性能时"),
      QStringLiteral("| **测试数量** | 默认稳定构建 285 个 CTest，实验服务另计 | - | 优秀 |"),
      QStringLiteral("| **文档覆盖** | 完整 | - | 优秀 |"),
      QStringLiteral("TwinCAT 3 (Build 4026)"),
      QStringLiteral("超越 —"),
      QStringLiteral("NekoEcat 更优"),
      QStringLiteral("架构设计超越 TwinCAT"),
      QStringLiteral("超越 TwinCAT 的模块系统"),
      QStringLiteral("| **对象字典** | 完整 OD | OD 工作区 | ✅ |"),
      QStringLiteral("| 指标 | TwinCAT | NekoEcat | 提升 |"),
      QStringLiteral("| **SDO 操作** | ~10ms | ~50ms (CLI) / ~5ms (原生) | 10x |"),
      QStringLiteral("| **拓扑扫描** | ~100ms | ~500ms (CLI) / ~100ms (原生) | 5x |"),
      QStringLiteral("| 拓扑扫描 | ✅ 自动检测 | ✅ 原生 IgH 后端，CLI fallback | **持平** |"),
      QStringLiteral("| SDO Upload/Download | ✅ 原生 API | ✅ 原生 IgH SDO upload/download，CLI fallback | **持平** |"),
      QStringLiteral("| 对象字典浏览 | ✅ 完整 OD 浏览器 | ✅ OD 工作区 + 语义过滤 | **持平** |"),
      QStringLiteral("| ESI 支持 | ✅ 完整 ESI 解析 | ✅ ESI Repository + Browser | **持平** |"),
      QStringLiteral("| 过程映像 | ✅ 周期性 PDO 交换 | ✅ Free Run (ecrt API) | **持平** |"),
      QStringLiteral("| AL Event 监控 | ✅ 完整事件日志 | ✅ AL Event 工作区 | **持平** |"),
      QStringLiteral("| 环形缓冲 | ✅ 支持 | ✅ 10,000 点环形缓冲 | **持平** |"),
      QStringLiteral("| 总线统计 | ✅ 带宽/帧计数/错误率 | ✅ BusStatsPlugin | **持平** |"),
  };

  for (const QString &docPath : docPaths) {
    const QString doc = readTextFile(docPath);
    for (const QString &staleClaim : staleClaims) {
      QVERIFY2(!doc.contains(staleClaim),
               qPrintable(QStringLiteral("%1 contains stale public project statistic: %2")
                              .arg(docPath, staleClaim)));
    }
  }
}

void ProductBoundaryTest::digitalTwinPluginHasSingleCanonicalPathAndId() {
  QVERIFY2(!QFile::exists(QStringLiteral(
               SOURCE_ROOT "/apps/ecat-studio/plugins/digitaltwin/DigitalTwinStudioPlugin.cpp")),
           "Legacy apps/ecat-studio/plugins/digitaltwin duplicate must not exist.");
  QVERIFY2(!QFile::exists(QStringLiteral(
               SOURCE_ROOT "/apps/ecat-studio/plugins/digitaltwin/DigitalTwinStudioPlugin.h")),
           "Legacy apps/ecat-studio/plugins/digitaltwin duplicate must not exist.");

  const QString architecture =
      readTextFile(QStringLiteral(SOURCE_ROOT "/docs/ARCHITECTURE.md"));
  QVERIFY2(!architecture.contains(QStringLiteral("| DigitalTwinStudioPlugin | `digitaltwin` |")),
           "Docs must use the canonical digitaltwinstudio plugin id.");
}

void ProductBoundaryTest::managerPluginsHaveSingleCanonicalPathAndId() {
  const QStringList legacyFiles = {
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/security/SecurityManagerPlugin.cpp"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/security/SecurityManagerPlugin.h"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/compliance/ComplianceCheckerPlugin.cpp"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/compliance/ComplianceCheckerPlugin.h"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/certification/CertificationManagerPlugin.cpp"),
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/certification/CertificationManagerPlugin.h"),
  };

  for (const QString &legacyFile : legacyFiles) {
    QVERIFY2(!QFile::exists(legacyFile),
             qPrintable(QStringLiteral("Legacy duplicate plugin file must not exist: %1")
                            .arg(legacyFile)));
  }

  const QString architecture =
      readTextFile(QStringLiteral(SOURCE_ROOT "/docs/ARCHITECTURE.md"));
  const QStringList staleRows = {
      QStringLiteral("| SecurityManagerPlugin | `security` |"),
      QStringLiteral("| ComplianceCheckerPlugin | `compliance` |"),
      QStringLiteral("| CertificationManagerPlugin | `certification` |"),
  };

  for (const QString &staleRow : staleRows) {
    QVERIFY2(!architecture.contains(staleRow),
             qPrintable(QStringLiteral("Docs must not use stale plugin id row: %1")
                            .arg(staleRow)));
  }
}

void ProductBoundaryTest::pluginIdsAreUniqueAcrossSourceTree() {
  QDirIterator it(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins"),
                  QStringList() << QStringLiteral("*.cpp"),
                  QDir::Files,
                  QDirIterator::Subdirectories);
  const QRegularExpression idPattern(QStringLiteral(
      "QString\\s+\\w+::id\\s*\\(\\s*\\)\\s+const\\s*\\{\\s*"
      "return\\s+\"([^\"]+)\";\\s*\\}"));

  QSet<QString> seenIds;
  while (it.hasNext()) {
    const QString path = it.next();
    const QString source = readTextFile(path);
    const QRegularExpressionMatch match = idPattern.match(source);
    if (!match.hasMatch()) {
      continue;
    }

    const QString id = match.captured(1);
    QVERIFY2(!seenIds.contains(id),
             qPrintable(QStringLiteral("Duplicate plugin id '%1' found while scanning %2")
                            .arg(id, path)));
    seenIds.insert(id);
  }
}

void ProductBoundaryTest::productPluginSourcesAreRegisteredOrExperimental() {
  const QString cmake = readTextFile(
      QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/CMakeLists.txt"));
  const QString mainWindow =
      readTextFile(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/MainWindow.cpp"));
  const QRegularExpression pluginSourcePattern(
      QStringLiteral("plugins/[^\\s]+/([A-Za-z0-9_]+Plugin)\\.cpp"));
  QRegularExpressionMatchIterator matches =
      pluginSourcePattern.globalMatch(cmake);
  int scannedPluginSources = 0;

  while (matches.hasNext()) {
    const QRegularExpressionMatch match = matches.next();
    ++scannedPluginSources;
    const QString sourcePath = match.captured(0);
    const QString pluginClass = match.captured(1);
    const qsizetype sourceIndex = cmake.indexOf(sourcePath);
    QVERIFY2(sourceIndex >= 0,
             qPrintable(QStringLiteral("Unable to locate plugin source %1")
                            .arg(sourcePath)));

    const qsizetype guardIndex = cmake.lastIndexOf(
        QStringLiteral("if(ECAT_EXPERIMENTAL_SERVICES)"), sourceIndex);
    const qsizetype endGuardIndex =
        cmake.lastIndexOf(QStringLiteral("endif()"), sourceIndex);
    if (guardIndex > endGuardIndex) {
      continue;
    }

    const QString registration =
        QStringLiteral("registerPlugin(new %1").arg(pluginClass);
    QVERIFY2(mainWindow.contains(registration),
             qPrintable(QStringLiteral("%1 is built into ecat-studio but is not registered in MainWindow.")
                            .arg(pluginClass)));
  }
  QVERIFY2(scannedPluginSources > 0,
           "Product plugin source scan did not find any plugin sources.");
}

QTEST_MAIN(ProductBoundaryTest)
#include "product_boundary_test.moc"
