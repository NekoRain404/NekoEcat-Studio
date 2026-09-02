#include "ExportPlugin.h"
#include "services/ServiceContainer.h"
#include "utils/TableHelpers.h"

#include <functional>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QIcon>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QTextStream>
#include <QTreeWidget>
#include <QVBoxLayout>

ExportPlugin::ExportPlugin(ServiceContainer* container, QObject* parent) : container_(container) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString ExportPlugin::id() const {
    return "export";
}
QString ExportPlugin::displayName() const {
    return "Export";
}
QString ExportPlugin::displayNameZh() const {
    return QStringLiteral("导出");
}
int ExportPlugin::defaultOrder() const {
    return 85;
}
bool ExportPlugin::visible() const {
    return false;
}

QIcon ExportPlugin::icon() const {
    return QIcon::fromTheme("document-save-as");
}

void ExportPlugin::activate() {}
void ExportPlugin::deactivate() {}
void ExportPlugin::onSettingsChanged(const AppSettings&) {}
void ExportPlugin::onConnectionChanged(bool) {}

QWidget* ExportPlugin::widget() {
    return containerWidget_;
}

void ExportPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(0, 0, 0, 0);
}

static QString promptSavePath(QWidget* parent, const QString& title, const QString& defaultName,
                              const QString& filter) {
    return QFileDialog::getSaveFileName(parent, title, QDir::home().absoluteFilePath(defaultName), filter);
}

static int exportTableToCsv(QTableWidget* table, QTextStream& out, bool visibleOnly) {
    if (!table || table->rowCount() == 0)
        return 0;

    QStringList headers;
    for (int col = 0; col < table->columnCount(); ++col) {
        const auto* h = table->horizontalHeaderItem(col);
        headers.append(csvCell(h ? h->text() : QString("Column %1").arg(col + 1)));
    }
    out << headers.join(',') << '\n';

    int count = 0;
    for (int row = 0; row < table->rowCount(); ++row) {
        if (visibleOnly && table->isRowHidden(row))
            continue;
        QStringList cells;
        for (int col = 0; col < table->columnCount(); ++col) {
            cells.append(csvCell(tableText(table, row, col)));
        }
        out << cells.join(',') << '\n';
        ++count;
    }
    return count;
}

bool ExportPlugin::exportTableCsv(QWidget* parent, QTableWidget* table, const QString& defaultName,
                                  const QString& logSource, bool visibleOnly) {
    if (!table || table->rowCount() <= 0) {
        QMessageBox::information(parent, tr("Export %1").arg(logSource), tr("No entries are available to export."));
        return false;
    }
    const QString path = promptSavePath(
        parent, tr("Export %1 CSV").arg(logSource),
        QString("%1-%2.csv").arg(defaultName).arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "CSV (*.csv);;Text (*.txt)");
    if (path.isEmpty())
        return false;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(parent, tr("Export failed"), file.errorString());
        return false;
    }
    QTextStream out(&file);
    int n = exportTableToCsv(table, out, visibleOnly);
    emit updateDiagnostics("Info", logSource, tr("Exported %1 row(s): %2").arg(n).arg(path));
    return true;
}

bool ExportPlugin::exportTreeCsv(QWidget* parent, QTreeWidget* tree, const QString& defaultName,
                                 const QString& logSource) {
    if (!tree || tree->topLevelItemCount() == 0) {
        QMessageBox::information(parent, tr("Export %1").arg(logSource), tr("No data is available to export."));
        return false;
    }
    const QString path = promptSavePath(
        parent, tr("Export %1 CSV").arg(logSource),
        QString("%1-%2.csv").arg(defaultName).arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")),
        "CSV (*.csv);;Text (*.txt)");
    if (path.isEmpty())
        return false;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(parent, tr("Export failed"), file.errorString());
        return false;
    }
    QTextStream out(&file);

    const int cols = tree->columnCount();
    QStringList headers;
    for (int c = 0; c < cols; ++c) {
        auto* h = tree->headerItem();
        headers.append(csvCell(h ? h->text(c) : QString("Column %1").arg(c + 1)));
    }
    out << headers.join(',') << '\n';

    int count = 0;
    std::function<void(QTreeWidgetItem*, int)> writeItem = [&](QTreeWidgetItem* item, int depth) {
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
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        writeItem(tree->topLevelItem(i), 0);
    }

    emit updateDiagnostics("Info", logSource, tr("Exported %1 node(s): %2").arg(count).arg(path));
    return true;
}

bool ExportPlugin::exportPlainText(QWidget* parent, QPlainTextEdit* textEdit, const QString& defaultName,
                                   const QString& logSource, const QString& filter) {
    if (!textEdit || textEdit->toPlainText().isEmpty()) {
        QMessageBox::information(parent, tr("Export %1").arg(logSource), tr("No data is available to export."));
        return false;
    }
    const QString path = promptSavePath(
        parent, tr("Export %1").arg(logSource),
        QString("%1-%2.txt").arg(defaultName).arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss")), filter);
    if (path.isEmpty())
        return false;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(parent, tr("Export failed"), file.errorString());
        return false;
    }
    QTextStream out(&file);
    out << textEdit->toPlainText();
    emit updateDiagnostics("Info", logSource, tr("Exported: %1").arg(path));
    return true;
}
