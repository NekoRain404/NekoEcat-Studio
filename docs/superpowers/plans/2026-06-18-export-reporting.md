# Export & Reporting Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add professional data export (CSV/JSON/XML/HTML/PDF/Excel) and report generation (system overview, topology, performance, diagnostics, safety, custom) to NekoEcat Studio.

**Architecture:** Two new services (`ExportService` for file I/O, `ReportGeneratorService` for structured report creation) plus an enhanced `ExportPlugin` UI. The `ExportService` handles all file serialization; `ReportGeneratorService` queries domain services to build `Report` objects; the `ExportPlugin` presents format/source selectors and a preview pane. No external PDF/Excel libraries — we generate self-contained formats (HTML for PDF via QTextDocument, CSV/HTML for Excel compatibility).

**Tech Stack:** Qt6 Core/Widgets/Network, QJsonDocument, QDomDocument, QTextDocument, QTextCursor (HTML tables → PDF), QFile/QTextStream.

---

## File Structure

| File | Responsibility |
|------|----------------|
| `services/ExportService.h/.cpp` | Multi-format file writer (CSV, JSON, XML, HTML, PDF, Excel CSV) |
| `services/ReportGeneratorService.h/.cpp` | Builds `Report` structs from domain services |
| `plugins/export/ExportPlugin.h/.cpp` | Enhanced UI: format selector, data source, preview, report templates |
| `tests/export_plugin_test.cpp` | Tests for ExportService, ReportGeneratorService, ExportPlugin |
| `services/ServiceContainer.h/.cpp` | Add ExportService + ReportGeneratorService registration |
| `apps/ecat-studio/CMakeLists.txt` | Add new source files |
| `tests/CMakeLists.txt` | Add test executable |
| `MainWindow.cpp` | Register ExportPlugin with new service dependencies |

---

### Task 1: Create ExportService

**Files:**
- Create: `apps/ecat-studio/services/ExportService.h`
- Create: `apps/ecat-studio/services/ExportService.cpp`

- [ ] **Step 1: Create ExportService.h**

```cpp
#pragma once

// ExportService — multi-format data export (CSV, JSON, XML, HTML, PDF, Excel).
// All methods write to a file path and emit signals on success or failure.
// No widget dependencies — pure data serialization.

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantList>
#include <QString>

class QDomDocument;

enum class ExportFormat { Csv, Json, Xml, Html, Pdf, Excel };

struct ExportTemplate {
  QString name;
  ExportFormat defaultFormat;
  QStringList columns;
};

class ExportService : public QObject {
  Q_OBJECT
public:
  explicit ExportService(QObject *parent = nullptr);

  bool exportToCsv(const QString &filePath, const QVariantList &data);
  bool exportToJson(const QString &filePath, const QJsonObject &data);
  bool exportToXml(const QString &filePath, const QDomDocument &data);
  bool exportToHtml(const QString &filePath, const QString &html);
  bool exportToPdf(const QString &filePath, const QString &content);
  bool exportToExcel(const QString &filePath, const QVariantList &data);

  static QString formatFilter(ExportFormat format);
  static ExportTemplate topologyReportTemplate();
  static ExportTemplate sdoDictionaryTemplate();
  static ExportTemplate watchListTemplate();
  static ExportTemplate diagnosticsReportTemplate();
  static ExportTemplate performanceReportTemplate();

signals:
  void exportCompleted(const QString &filePath);
  void exportFailed(const QString &filePath, const QString &error);

private:
  bool writeFile(const QString &filePath, const QByteArray &data);
  static QString escapeCsvField(const QString &field);
  static QByteArray generateExcelXml(const QVariantList &data);
};
```

- [ ] **Step 2: Create ExportService.cpp**

Implement all export methods. Each follows the same pattern: validate data → serialize → write file → emit signal. The PDF method uses QTextDocument::print() with QPdfWriter. Excel uses a simple XML spreadsheet format (SpreadsheetML) that Excel/LibreOffice can open — no external library needed.

Key implementation details:
- `exportToCsv`: Iterate QVariantList rows, join with commas, handle quoting
- `exportToJson`: QJsonDocument(data).toJson() → write
- `exportToXml`: data.toString() → write (QDomDocument already serializes)
- `exportToHtml`: Write raw HTML string
- `exportToPdf`: Create QTextDocument, setHtml() with content wrapped in basic CSS, print to QPdfWriter
- `exportToExcel`: Generate SpreadsheetML XML (simple XML that Excel opens natively)
- `escapeCsvField`: Quote fields containing commas, quotes, or newlines
- Templates: Return static ExportTemplate structs with predefined column lists

- [ ] **Step 3: Verify it compiles**

Run: `cmake --build build -j4 2>&1 | tail -20` (after adding to CMakeLists in Task 5)

---

### Task 2: Create ReportGeneratorService

**Files:**
- Create: `apps/ecat-studio/services/ReportGeneratorService.h`
- Create: `apps/ecat-studio/services/ReportGeneratorService.cpp`

- [ ] **Step 1: Create ReportGeneratorService.h**

```cpp
#pragma once

// ReportGeneratorService — builds structured Report objects from domain
// services. Reports contain title, summary, sections, tables, and
// recommendations. Can render to HTML or Markdown for export.

#include <QObject>
#include <QString>
#include <QVector>
#include <QJsonObject>

class ExportService;
class TopologyService;
class PerformanceMonitorService;
class DiagnosticReportService;
class SafetyController;
class AlarmService;
class EventBus;
class EcatClient;

struct ReportSection {
  QString title;
  QString content;
  QStringList headers;
  QVector<QStringList> rows;
};

struct Report {
  QString title;
  QString summary;
  QString generatedAt;
  QVector<ReportSection> sections;
  QStringList recommendations;
  bool isValid() const { return !title.isEmpty(); }
};

enum class ReportType { SystemOverview, Topology, Performance, Diagnostics, Safety, Custom };

struct ReportConfig {
  ReportType type = ReportType::Custom;
  QString title;
  bool includeCharts = false;
  bool includeTables = true;
  bool includeRecommendations = true;
  QStringList sections;
};

class ReportGeneratorService : public QObject {
  Q_OBJECT
public:
  explicit ReportGeneratorService(ExportService *exportService,
                                  TopologyService *topology,
                                  PerformanceMonitorService *perfMonitor,
                                  DiagnosticReportService *diagReport,
                                  SafetyController *safety,
                                  AlarmService *alarm,
                                  QObject *parent = nullptr);

  Report generateSystemOverview();
  Report generateTopologyReport();
  Report generatePerformanceReport();
  Report generateDiagnosticsReport();
  Report generateSafetyReport();
  Report generateCustomReport(const ReportConfig &config);

  static QString renderToHtml(const Report &report);
  static QString renderToMarkdown(const Report &report);

signals:
  void reportGenerated(const Report &report);

private:
  ReportSection buildSlaveTable();
  ReportSection buildPerformanceMetrics();
  ReportSection buildAlarmSummary();
  ReportSection buildSafetyStatus();

  ExportService *exportService_;
  TopologyService *topology_;
  PerformanceMonitorService *perfMonitor_;
  DiagnosticReportService *diagReport_;
  SafetyController *safety_;
  AlarmService *alarm_;
};
```

- [ ] **Step 2: Create ReportGeneratorService.cpp**

Implement report generation methods. Each builds a `Report` with relevant sections:
- `generateSystemOverview`: Slave count, master state, performance summary, active alarms
- `generateTopologyReport`: Slave table, port states, chain topology
- `generatePerformanceReport`: Cycle time, jitter, frame loss, PDO/SDO rates
- `generateDiagnosticsReport`: Error history, AL events, watchdog status
- `generateSafetyReport`: Safety state, active safety alarms, compliance status
- `generateCustomReport`: Build from ReportConfig sections
- `renderToHtml`: Generate a styled HTML document with tables and sections
- `renderToMarkdown`: Generate Markdown with headers, tables, and bullet lists

- [ ] **Step 3: Verify compilation**

---

### Task 3: Enhance ExportPlugin

**Files:**
- Modify: `apps/ecat-studio/plugins/export/ExportPlugin.h`
- Modify: `apps/ecat-studio/plugins/export/ExportPlugin.cpp`

- [ ] **Step 1: Update ExportPlugin.h**

Replace the existing header with the enhanced version that includes:
- ExportService and ReportGeneratorService dependencies (via ServiceContainer)
- UI elements: format combo, data source combo, export button, report template combo, preview text
- Methods for building the new UI and handling export/report actions
- Keep existing `exportTableCsv`, `exportTreeCsv`, `exportPlainText` methods (backward compat)

```cpp
#pragma once

// ExportPlugin — workspace for multi-format data export and report generation.
// Supports CSV, JSON, XML, HTML, PDF, Excel exports and structured reports.

#include "plugins/WorkspacePlugin.h"

class QComboBox;
class QLabel;
class QPlainTextEdit;
class QProgressBar;
class QPushButton;
class QTableWidget;
class QTreeWidget;
class ServiceContainer;
class ExportService;
class ReportGeneratorService;

class ExportPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit ExportPlugin(ServiceContainer *container,
                        QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;
  void onSettingsChanged(const AppSettings &settings) override;
  void onConnectionChanged(bool connected) override;

  bool exportTableCsv(QWidget *parent, QTableWidget *table,
                      const QString &defaultName,
                      const QString &logSource,
                      bool visibleOnly = true);
  bool exportTreeCsv(QWidget *parent, QTreeWidget *tree,
                     const QString &defaultName,
                     const QString &logSource);
  bool exportPlainText(QWidget *parent, QPlainTextEdit *textEdit,
                       const QString &defaultName,
                       const QString &logSource,
                       const QString &filter = "Text (*.txt);;All (*)");

private:
  void buildUi();
  void onExportClicked();
  void onGenerateReportClicked();
  void updatePreview();

  ServiceContainer *container_;
  ExportService *exportService_;
  ReportGeneratorService *reportService_;
  QWidget *containerWidget_ = nullptr;
  QComboBox *formatCombo_ = nullptr;
  QComboBox *sourceCombo_ = nullptr;
  QComboBox *reportCombo_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QPushButton *reportBtn_ = nullptr;
  QPlainTextEdit *previewPane_ = nullptr;
  QProgressBar *progressBar_ = nullptr;
  QLabel *statusLabel_ = nullptr;
};
```

- [ ] **Step 2: Update ExportPlugin.cpp**

Rewrite to build the enhanced UI with:
- **Format selector**: QComboBox with CSV/JSON/XML/HTML/PDF/Excel options
- **Data source selector**: QComboBox with Topology/SDO Dictionary/Watch List/Diagnostics/Performance
- **Export button**: Triggers export using ExportService
- **Report template selector**: QComboBox with System Overview/Topology/Performance/Diagnostics/Safety/Custom
- **Generate report button**: Creates report via ReportGeneratorService, shows in preview
- **Preview pane**: QPlainTextEdit showing the generated report
- **Status bar**: QLabel showing export status

Keep the existing `exportTableCsv`, `exportTreeCsv`, `exportPlainText` methods unchanged for backward compatibility.

- [ ] **Step 3: Verify compilation**

---

### Task 4: Register Services in ServiceContainer and MainWindow

**Files:**
- Modify: `apps/ecat-studio/services/ServiceContainer.h`
- Modify: `apps/ecat-studio/services/ServiceContainer.cpp`
- Modify: `apps/ecat-studio/MainWindow.cpp`

- [ ] **Step 1: Update ServiceContainer.h**

Add forward declarations and accessors for ExportService and ReportGeneratorService:

```cpp
class ExportService;
class ReportGeneratorService;
```

Add to private section:
```cpp
ExportService *export_ = nullptr;
ReportGeneratorService *reportGenerator_ = nullptr;
```

Add public accessors:
```cpp
ExportService *exportService() const { return export_; }
ReportGeneratorService *reportGenerator() const { return reportGenerator_; }
```

- [ ] **Step 2: Update ServiceContainer.cpp**

Add includes and create instances in constructor:

```cpp
#include "ExportService.h"
#include "ReportGeneratorService.h"
```

After existing service creation, add:
```cpp
export_ = new ExportService(this);
reportGenerator_ = new ReportGeneratorService(
    export_, topology_, perfMonitor_, diagnosticReport_, safety_, alarm_, this);
```

- [ ] **Step 3: Update MainWindow.cpp**

Add include and register ExportPlugin with ServiceContainer:

```cpp
#include "plugins/export/ExportPlugin.h"
```

In the constructor, after existing plugin registrations:
```cpp
pluginRegistry_->registerPlugin(new ExportPlugin(container, this));
```

Note: The existing MainWindow creates services individually, not via ServiceContainer. The ExportPlugin needs a ServiceContainer, so we need to either:
- Option A: Create a ServiceContainer in MainWindow and pass it to ExportPlugin
- Option B: Pass individual services to ExportPlugin

Given the existing pattern where MainWindow creates services individually, Option B is cleaner — but the task says to use ServiceContainer. Looking at the existing code, some plugins (ConsistencyPlugin, StateMachinePlugin) already take ServiceContainer. We'll create a minimal ServiceContainer in MainWindow for the ExportPlugin.

Actually, looking more carefully: the existing ExportPlugin already takes `ServiceContainer *container`. So we need to create a ServiceContainer in MainWindow. Let me check if that's feasible...

The ServiceContainer creates its own EcatClient internally, which conflicts with MainWindow's `client_`. The cleanest approach: create the new services individually in MainWindow and pass them to ExportPlugin via a modified constructor.

**Revised approach for ExportPlugin constructor:**

```cpp
explicit ExportPlugin(ExportService *exportService,
                      ReportGeneratorService *reportService,
                      QObject *parent = nullptr);
```

This avoids the ServiceContainer dependency entirely and follows the pattern used by AlarmPlugin, ProjectPlugin, etc.

- [ ] **Step 3b: Update MainWindow.cpp with individual services**

```cpp
auto *exportService = new ExportService(this);
auto *reportService = new ReportGeneratorService(
    exportService, topologyService_, nullptr, nullptr, nullptr, nullptr, this);
pluginRegistry_->registerPlugin(new ExportPlugin(exportService, reportService, this));
```

Note: Some ReportGeneratorService dependencies (perfMonitor, diagReport, safety, alarm) aren't created in MainWindow's constructor. Pass nullptr for now — the service handles null gracefully.

- [ ] **Step 4: Verify compilation**

---

### Task 5: Update CMakeLists.txt Files

**Files:**
- Modify: `apps/ecat-studio/CMakeLists.txt`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Update app CMakeLists.txt**

Add to the Services section:
```cmake
    services/ExportService.h                   services/ExportService.cpp
    services/ReportGeneratorService.h          services/ReportGeneratorService.cpp
```

Add to `target_include_directories`:
```cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/export
```

Note: The ExportPlugin files (`plugins/export/ExportPlugin.h/.cpp`) are NOT in the current CMakeLists — they exist on disk but aren't compiled. Add them:
```cmake
    plugins/export/ExportPlugin.h              plugins/export/ExportPlugin.cpp
```

- [ ] **Step 2: Update test CMakeLists.txt**

Add test executable at the end:
```cmake
add_executable(export_plugin_test
    export_plugin_test.cpp
    ../apps/ecat-studio/plugins/export/ExportPlugin.cpp
    ../apps/ecat-studio/plugins/export/ExportPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/ExportService.cpp
    ../apps/ecat-studio/services/ExportService.h
    ../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../apps/ecat-studio/services/ReportGeneratorService.h
)
target_link_libraries(export_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(export_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)
set_target_properties(export_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME export_plugin_test COMMAND export_plugin_test)
set_tests_properties(export_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
```

---

### Task 6: Create Tests

**Files:**
- Create: `tests/export_plugin_test.cpp`

- [ ] **Step 1: Write test file**

Tests to include:
- `testExportServiceCsv`: Export QVariantList to CSV, verify file content
- `testExportServiceJson`: Export QJsonObject to JSON, verify file content
- `testExportServiceHtml`: Export HTML string, verify file exists
- `testExportServiceSignals`: Verify exportCompleted/exportFailed signals
- `testReportGeneratorBasic`: Generate a system overview report, verify structure
- `testReportRenderHtml`: Render a Report to HTML, verify contains title/sections
- `testReportRenderMarkdown`: Render a Report to Markdown, verify headers
- `testPluginIdentity`: Verify ExportPlugin id, displayName, defaultOrder
- `testTemplates`: Verify template static methods return valid templates

- [ ] **Step 2: Build and run tests**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4`

---

### Task 7: Final Integration Verification

- [ ] **Step 1: Full build**

```bash
cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat
cmake --build build -j4
```

- [ ] **Step 2: Run all tests**

```bash
ctest --test-dir build --output-on-failure -j4
```

- [ ] **Step 3: Verify ExportPlugin compiles and links**

The ExportPlugin should be visible in the plugin registry with id="export", defaultOrder=85.
