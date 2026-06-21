#include "DiagnosticReportService.h"
#include "EventBus.h"
#include "TopologyService.h"
#include "DcSyncService.h"
#include "PerformanceMonitorService.h"
#include "WatchdogService.h"
#include "infra/EcatClient.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>

// DiagnosticReportService.cpp — Generates comprehensive diagnostic reports in text and CSV
//
// Implementation notes:
//   - Aggregates data from TopologyService, DcSyncService, PerformanceMonitor, Watchdog
//   - Produces Markdown-formatted reports with tables for topology and watchdog status
//   - CSV export emits section/metric/value rows for spreadsheet analysis

DiagnosticReportService::DiagnosticReportService(
    EventBus *bus, EcatClient *client, TopologyService *topology,
    DcSyncService *dcSync, PerformanceMonitorService *perfMonitor,
    WatchdogService *watchdog, QObject *parent)
    : QObject(parent), bus_(bus), client_(client), topology_(topology),
      dcSync_(dcSync), perfMonitor_(perfMonitor), watchdog_(watchdog) {}

QString DiagnosticReportService::generateReport() const {
  const qint64 now = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
  QString report;

  QTextStream ts(&report);
  ts << "# EtherCAT Diagnostic Report\n\n";
  ts << "**Generated:** "
     << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << " UTC\n";
  ts << "**Connected:** " << (client_ && client_->isConnected() ? "Yes" : "No")
     << "\n\n";

  ts << "---\n\n";
  ts << topologySection();
  ts << slaveStatusSection();
  ts << performanceSection();
  ts << dcSyncSection();
  ts << watchdogSection();

  emit const_cast<DiagnosticReportService *>(this)->reportGenerated(report);
  return report;
}

void DiagnosticReportService::exportReport(const QString &filePath) const {
  QString report = generateReport();
  QFile file(filePath);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream ts(&file);
    ts << report;
  }
}

void DiagnosticReportService::exportReportCsv(const QString &filePath) const {
  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;

  QTextStream ts(&file);
  ts << "Section,Metric,Value\n";

  auto slaves = topology_ ? topology_->currentSlaves() : QVector<SlaveInfo>();
  for (const auto &s : slaves) {
    ts << "Slave,Position " << s.position << "," << s.state << "\n";
    ts << "Slave,Name " << s.position << "," << s.name << "\n";
  }

  if (perfMonitor_ && perfMonitor_->isMonitoring()) {
    QJsonObject m = perfMonitor_->currentMetrics();
    ts << "Performance,CycleTimeUs," << m["cycleTimeUs"].toDouble() << "\n";
    ts << "Performance,JitterUs," << m["jitterUs"].toDouble() << "\n";
    ts << "Performance,FrameLoss," << m["frameLoss"].toInt() << "\n";
    ts << "Performance,PdoUpdateRate," << m["pdoUpdateRate"].toDouble()
       << "\n";
  }

  if (watchdog_) {
    QJsonObject w = watchdog_->currentStatus();
    ts << "Watchdog,TotalTimeouts," << w["totalTimeouts"].toInt() << "\n";
    ts << "Watchdog,TotalTriggers," << w["totalTriggers"].toInt() << "\n";
  }
}

QString DiagnosticReportService::topologySection() const {
  QString s;
  QTextStream ts(&s);
  ts << "## Bus Topology\n\n";

  auto slaves = topology_ ? topology_->currentSlaves() : QVector<SlaveInfo>();
  if (slaves.isEmpty()) {
    ts << "No slaves discovered.\n\n";
    return s;
  }

  ts << "| Position | Name | State | Flags |\n";
  ts << "|----------|------|-------|-------|\n";
  for (const auto &sl : slaves) {
    ts << "| " << sl.position << " | " << sl.name << " | " << sl.state
       << " | " << sl.flags << " |\n";
  }
  ts << "\n";
  return s;
}

QString DiagnosticReportService::slaveStatusSection() const {
  QString s;
  QTextStream ts(&s);
  ts << "## Slave Status Overview\n\n";

  auto slaves = topology_ ? topology_->currentSlaves() : QVector<SlaveInfo>();
  int opCount = 0;
  int preOpCount = 0;
  int otherCount = 0;

  for (const auto &sl : slaves) {
    if (sl.state.contains("OP"))
      ++opCount;
    else if (sl.state.contains("PREOP"))
      ++preOpCount;
    else
      ++otherCount;
  }

  ts << "- **Total slaves:** " << slaves.size() << "\n";
  ts << "- **OP:** " << opCount << "\n";
  ts << "- **PRE-OP:** " << preOpCount << "\n";
  ts << "- **Other:** " << otherCount << "\n\n";
  return s;
}

QString DiagnosticReportService::performanceSection() const {
  QString s;
  QTextStream ts(&s);
  ts << "## Performance Metrics\n\n";

  if (!perfMonitor_ || !perfMonitor_->isMonitoring()) {
    ts << "Performance monitoring not active.\n\n";
    return s;
  }

  QJsonObject m = perfMonitor_->currentMetrics();
  ts << "- **Cycle time:** " << m["cycleTimeUs"].toDouble() << " µs\n";
  ts << "- **Jitter:** " << m["jitterUs"].toDouble() << " µs\n";
  ts << "- **Frame loss:** " << m["frameLoss"].toInt() << "\n";
  ts << "- **PDO update rate:** " << m["pdoUpdateRate"].toDouble()
     << " Hz\n";
  ts << "- **Sample count:** " << m["sampleCount"].toInt() << "\n\n";
  return s;
}

QString DiagnosticReportService::dcSyncSection() const {
  QString s;
  QTextStream ts(&s);
  ts << "## DC Sync Status\n\n";

  if (!dcSync_) {
    ts << "DC Sync service not available.\n\n";
    return s;
  }

  ts << "DC Sync polling is handled by DcSyncService.\n\n";
  return s;
}

QString DiagnosticReportService::watchdogSection() const {
  QString s;
  QTextStream ts(&s);
  ts << "## Watchdog Status\n\n";

  if (!watchdog_) {
    ts << "Watchdog service not available.\n\n";
    return s;
  }

  QJsonObject w = watchdog_->currentStatus();
  ts << "- **Monitoring:** " << (w["monitoring"].toBool() ? "Active" : "Inactive") << "\n";
  ts << "- **Total timeouts:** " << w["totalTimeouts"].toInt() << "\n";
  ts << "- **Total triggers:** " << w["totalTriggers"].toInt() << "\n";

  QJsonArray slaves = w["slaves"].toArray();
  if (!slaves.isEmpty()) {
    ts << "\n| Position | Watchdog OK | Timeouts | Triggers |\n";
    ts << "|----------|-------------|----------|----------|\n";
    for (const auto &entry : slaves) {
      QJsonObject obj = entry.toObject();
      ts << "| " << obj["position"].toInt() << " | "
         << (obj["watchdogOk"].toBool() ? "Yes" : "No") << " | "
         << obj["timeoutCount"].toInt() << " | "
         << obj["triggerCount"].toInt() << " |\n";
    }
  }
  ts << "\n";
  return s;
}
