// MainWindowExport.cpp — unified export functions for all workspaces.
//
// Provides CSV export for: PDO Map, SDO Dictionary, SDO History,
// ESI Repository, Watch, Startup SDO, Topology, and Host Health.
// Each function follows the same pattern: check data, prompt for path,
// write CSV, log result.

#include "MainWindowIncludes.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

// ── Helper: export a QTableWidget to CSV ─────────────────────────────
// Writes all visible (non-hidden) rows. Returns the number of rows written.
static int exportTableToCsv(QTableWidget *table, QTextStream &out,
                            bool visibleOnly = true)
{
    if (!table || table->rowCount() == 0) return 0;

    /* Header row */
    QStringList headers;
    for (int col = 0; col < table->columnCount(); ++col) {
        const auto *h = table->horizontalHeaderItem(col);
        headers.append(csvCell(h ? h->text() : QString("Column %1").arg(col + 1)));
    }
    out << headers.join(',') << '\n';

    /* Data rows */
    int count = 0;
    for (int row = 0; row < table->rowCount(); ++row) {
        if (visibleOnly && table->isRowHidden(row)) continue;
        QStringList cells;
        for (int col = 0; col < table->columnCount(); ++col) {
            cells.append(csvCell(tableText(table, row, col)));
        }
        out << cells.join(',') << '\n';
        ++count;
    }
    return count;
}

// ── Helper: prompt for save path ─────────────────────────────────────
static QString promptSavePath(QWidget *parent, const QString &title,
                              const QString &defaultName,
                              const QString &filter)
{
    return QFileDialog::getSaveFileName(
        parent, title,
        QDir::home().absoluteFilePath(defaultName), filter);
}

// ── Export PDO Map ───────────────────────────────────────────────────
// Exports the current PDO mapping table (Tx/Rx) to CSV.
void MainWindow::exportPdoMapCsv()
{
    auto *table = sdo_ ? sdo_->pdoTable : nullptr;
    if (!table || table->rowCount() <= 0) {
        QMessageBox::information(this,
            uiText("Export PDO Map", "导出 PDO 映射"),
            uiText("No PDO entries are available to export.",
                   "当前没有可导出的 PDO 条目。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export PDO Map CSV", "导出 PDO 映射 CSV"),
        QString("ethercat-pdo-map-%1.csv")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "CSV (*.csv);;Text (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    int n = exportTableToCsv(table, out);
    updateDiagnostics("Info", "PDO Map",
        uiText("Exported %1 PDO row(s): %2", "已导出 %1 条 PDO 行：%2").arg(n).arg(path));
}

// ── Export SDO Dictionary ────────────────────────────────────────────
// Exports the Object Dictionary (SDO) table to CSV.
void MainWindow::exportSdoDictionaryCsv()
{
    auto *table = sdo_ ? sdo_->sdoTable : nullptr;
    if (!table || table->rowCount() <= 0) {
        QMessageBox::information(this,
            uiText("Export SDO Dictionary", "导出 SDO 字典"),
            uiText("No SDO entries are available to export.",
                   "当前没有可导出的 SDO 条目。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export SDO Dictionary CSV", "导出 SDO 字典 CSV"),
        QString("ethercat-sdo-dictionary-%1.csv")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "CSV (*.csv);;Text (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    int n = exportTableToCsv(table, out);
    updateDiagnostics("Info", "SDO Dictionary",
        uiText("Exported %1 SDO row(s): %2", "已导出 %1 条 SDO 行：%2").arg(n).arg(path));
}

// ── Export SDO History ───────────────────────────────────────────────
// Exports the SDO read/write history table to CSV.
void MainWindow::exportSdoHistoryCsv()
{
    if (!sdoHistoryTable_ || sdoHistoryTable_->rowCount() <= 0) {
        QMessageBox::information(this,
            uiText("Export SDO History", "导出 SDO 历史"),
            uiText("No SDO history entries are available to export.",
                   "当前没有可导出的 SDO 历史记录。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export SDO History CSV", "导出 SDO 历史 CSV"),
        QString("ethercat-sdo-history-%1.csv")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "CSV (*.csv);;Text (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    int n = exportTableToCsv(sdoHistoryTable_, out, false);
    updateDiagnostics("Info", "SDO History",
        uiText("Exported %1 SDO history row(s): %2", "已导出 %1 条 SDO 历史：%2").arg(n).arg(path));
}

// ── Export ESI Repository ────────────────────────────────────────────
// Exports the ESI repository table (slave list with ESI info) to CSV.
void MainWindow::exportEsiRepositoryCsv()
{
    if (!esiTable_ || esiTable_->rowCount() <= 0) {
        QMessageBox::information(this,
            uiText("Export ESI Repository", "导出 ESI 仓库"),
            uiText("No ESI entries are available to export.",
                   "当前没有可导出的 ESI 条目。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export ESI Repository CSV", "导出 ESI 仓库 CSV"),
        QString("ethercat-esi-repository-%1.csv")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "CSV (*.csv);;Text (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    int n = exportTableToCsv(esiTable_, out, false);
    updateDiagnostics("Info", "ESI Repository",
        uiText("Exported %1 ESI row(s): %2", "已导出 %1 条 ESI 条目：%2").arg(n).arg(path));
}

// ── Export ESI XML ───────────────────────────────────────────────────
// Exports the raw ESI XML text panel content to an XML file.
void MainWindow::exportEsiXml()
{
    if (!rawText_ || !rawText_->xmlText || rawText_->xmlText->toPlainText().isEmpty()) {
        QMessageBox::information(this,
            uiText("Export ESI XML", "导出 ESI XML"),
            uiText("No ESI XML data is available to export.",
                   "当前没有可导出的 ESI XML 数据。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export ESI XML", "导出 ESI XML"),
        QString("ethercat-esi-%1.xml")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "XML (*.xml);;Text (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    out << rawText_->xmlText->toPlainText();
    updateDiagnostics("Info", "ESI XML",
        uiText("Exported ESI XML: %1", "已导出 ESI XML：%1").arg(path));
}

// ── Export Watch ─────────────────────────────────────────────────────
// Exports the current Watch items table to CSV.
void MainWindow::exportWatchCsv()
{
    auto *table = watch_ ? watch_->watchTable : nullptr;
    if (!table || table->rowCount() <= 0) {
        QMessageBox::information(this,
            uiText("Export Watch", "导出监视"),
            uiText("No Watch items are available to export.",
                   "当前没有可导出的监视项。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export Watch CSV", "导出监视 CSV"),
        QString("ethercat-watch-%1.csv")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "CSV (*.csv);;Text (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    int n = exportTableToCsv(table, out);
    updateDiagnostics("Info", "Watch",
        uiText("Exported %1 Watch row(s): %2", "已导出 %1 条监视行：%2").arg(n).arg(path));
}

// ── Export Startup SDO ───────────────────────────────────────────────
// Exports the Startup SDO table to CSV.
void MainWindow::exportStartupSdoCsv()
{
    if (!startupSdoTable_ || startupSdoTable_->rowCount() <= 0) {
        QMessageBox::information(this,
            uiText("Export Startup SDO", "导出启动 SDO"),
            uiText("No Startup SDO entries are available to export.",
                   "当前没有可导出的启动 SDO 条目。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export Startup SDO CSV", "导出启动 SDO CSV"),
        QString("ethercat-startup-sdo-%1.csv")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "CSV (*.csv);;Text (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    int n = exportTableToCsv(startupSdoTable_, out, false);
    updateDiagnostics("Info", "Startup SDO",
        uiText("Exported %1 Startup SDO row(s): %2", "已导出 %1 条启动 SDO：%2").arg(n).arg(path));
}

// ── Export Topology ──────────────────────────────────────────────────
// Exports the topology tree to a structured CSV (depth, position, name, state, etc.)
void MainWindow::exportTopologyCsv()
{
    if (!topologyTree_ || topologyTree_->topLevelItemCount() == 0) {
        QMessageBox::information(this,
            uiText("Export Topology", "导出拓扑"),
            uiText("No topology data is available to export.",
                   "当前没有可导出的拓扑数据。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export Topology CSV", "导出拓扑 CSV"),
        QString("ethercat-topology-%1.csv")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "CSV (*.csv);;Text (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);

    /* Write header */
    const int cols = topologyTree_->columnCount();
    QStringList headers;
    for (int c = 0; c < cols; ++c) {
        auto *h = topologyTree_->headerItem();
        headers.append(csvCell(h ? h->text(c) : QString("Column %1").arg(c + 1)));
    }
    out << headers.join(',') << '\n';

    /* Recursive tree walker */
    int count = 0;
    std::function<void(QTreeWidgetItem *, int)> writeItem =
        [&](QTreeWidgetItem *item, int depth) {
        QStringList cells;
        for (int c = 0; c < cols; ++c) {
            cells.append(csvCell(item->text(c)));
        }
        out << cells.join(',') << '\n';
        ++count;
        for (int i = 0; i < item->childCount(); ++i) {
            writeItem(item->child(i), depth + 1);
        }
    };
    for (int i = 0; i < topologyTree_->topLevelItemCount(); ++i) {
        writeItem(topologyTree_->topLevelItem(i), 0);
    }

    updateDiagnostics("Info", "Topology",
        uiText("Exported %1 topology node(s): %2", "已导出 %1 个拓扑节点：%2").arg(count).arg(path));
}

// ── Export Host Health ───────────────────────────────────────────────
// Exports the host health/diagnostics checks table to CSV.
void MainWindow::exportHostHealthCsv()
{
    if (!hostHealthTable_ || hostHealthTable_->rowCount() <= 0) {
        QMessageBox::information(this,
            uiText("Export Host Health", "导出主机健康"),
            uiText("No host health data is available to export.",
                   "当前没有可导出的主机健康数据。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export Host Health CSV", "导出主机健康 CSV"),
        QString("ethercat-host-health-%1.csv")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "CSV (*.csv);;Text (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    int n = exportTableToCsv(hostHealthTable_, out, false);
    updateDiagnostics("Info", "Host Health",
        uiText("Exported %1 host health row(s): %2", "已导出 %1 条主机健康项：%2").arg(n).arg(path));
}

// ── Export PDO Raw ──────────────────────────────────────────────────
// Exports the PDO raw text panel content (ethercat pdo output) to a text file.
void MainWindow::exportPdoRawText()
{
    if (!rawText_ || !rawText_->pdoText || rawText_->pdoText->toPlainText().isEmpty()) {
        QMessageBox::information(this,
            uiText("Export PDO Raw", "导出 PDO 原始输出"),
            uiText("No PDO raw data is available to export.",
                   "当前没有可导出的 PDO 原始数据。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export PDO Raw Text", "导出 PDO 原始输出"),
        QString("ethercat-pdo-raw-%1.txt")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "Text (*.txt);;All (*)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    out << rawText_->pdoText->toPlainText();
    updateDiagnostics("Info", "PDO Raw",
        uiText("Exported PDO raw text: %1", "已导出 PDO 原始输出：%1").arg(path));
}

// ── Export SDO Raw ──────────────────────────────────────────────────
// Exports the SDO raw text panel content (ethercat sdo output) to a text file.
void MainWindow::exportSdoRawText()
{
    if (!rawText_ || !rawText_->sdoText || rawText_->sdoText->toPlainText().isEmpty()) {
        QMessageBox::information(this,
            uiText("Export SDO Raw", "导出 SDO 原始输出"),
            uiText("No SDO raw data is available to export.",
                   "当前没有可导出的 SDO 原始数据。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export SDO Raw Text", "导出 SDO 原始输出"),
        QString("ethercat-sdo-raw-%1.txt")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "Text (*.txt);;All (*)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    out << rawText_->sdoText->toPlainText();
    updateDiagnostics("Info", "SDO Raw",
        uiText("Exported SDO raw text: %1", "已导出 SDO 原始输出：%1").arg(path));
}

// ── Export Master Raw ───────────────────────────────────────────────
// Exports the master raw text panel content to a text file.
void MainWindow::exportMasterRawText()
{
    if (!rawText_ || !rawText_->masterText || rawText_->masterText->toPlainText().isEmpty()) {
        QMessageBox::information(this,
            uiText("Export Master Raw", "导出主站原始输出"),
            uiText("No master raw data is available to export.",
                   "当前没有可导出的主站原始数据。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export Master Raw Text", "导出主站原始输出"),
        QString("ethercat-master-raw-%1.txt")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "Text (*.txt);;All (*)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    out << rawText_->masterText->toPlainText();
    updateDiagnostics("Info", "Master Raw",
        uiText("Exported master raw text: %1", "已导出主站原始输出：%1").arg(path));
}

// ── Export Slave Raw ────────────────────────────────────────────────
// Exports the slave raw text panel content to a text file.
void MainWindow::exportSlaveRawText()
{
    if (!rawText_ || !rawText_->infoText || rawText_->infoText->toPlainText().isEmpty()) {
        QMessageBox::information(this,
            uiText("Export Slave Raw", "导出从站原始输出"),
            uiText("No slave raw data is available to export.",
                   "当前没有可导出的从站原始数据。"));
        return;
    }
    const QString path = promptSavePath(this,
        uiText("Export Slave Raw Text", "导出从站原始输出"),
        QString("ethercat-slave-raw-%1.txt")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "Text (*.txt);;All (*)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, uiText("Export failed", "导出失败"), file.errorString());
        return;
    }
    QTextStream out(&file);
    out << rawText_->infoText->toPlainText();
    updateDiagnostics("Info", "Slave Raw",
        uiText("Exported slave raw text: %1", "已导出从站原始输出：%1").arg(path));
}

