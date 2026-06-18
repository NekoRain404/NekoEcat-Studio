// MainWindowFreeRunChart.cpp — Real-time chart for Free Run entries.
// Opens a dialog that plots the selected entry's value over time.

#include "MainWindowIncludes.h"
#include "detail/RealtimeChartDialog.h"

#include <QDateTime>
#include <QMessageBox>

// Opens a real-time chart dialog for the currently selected Free Run entry.
// Reads the entry's name, key, and current value from the Free Run table,
// then creates a non-modal chart dialog that can be fed new values
// during the Free Run polling cycle.
void MainWindow::openFreeRunChart()
{
    if (!freeRunWidgets_ || !freeRunWidgets_->freeRunEntryTable) return;

    auto *table = freeRunWidgets_->freeRunEntryTable;
    const int row = table->currentRow();
    if (row < 0) return;

    /* Read entry name and value from the table. */
    const QString name = tableText(table, row, 0);
    if (name.isEmpty()) return;

    /* Try to read a numeric value from the current value column. */
    double currentValue = 0.0;
    bool hasValue = false;
    for (int col = table->columnCount() - 1; col >= 0; --col) {
        auto *h = table->horizontalHeaderItem(col);
        if (!h) continue;
        const QString hdr = h->text().toLower();
        if (hdr.contains("value") || hdr.contains("decoded") || hdr.contains("raw")) {
            const QString txt = tableText(table, row, col);
            double v = 0;
            bool ok = false;
            v = txt.toDouble(&ok);
            if (ok) { currentValue = v; hasValue = true; break; }
            /* Try hex. */
            v = txt.toULongLong(&ok, 16);
            if (ok) { currentValue = static_cast<double>(v); hasValue = true; break; }
        }
    }

    /* Build a key from the row for identification. */
    const QString key = QString("freerun_%1_%2").arg(row).arg(
        QDateTime::currentMSecsSinceEpoch());

    /* Create the chart dialog. */
    auto *dialog = new RealtimeChartDialog(name, key, currentValue, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    /* Clean up when closed. */
    connect(dialog, &QObject::destroyed, this, [this, dialog]() {
        openCharts_.removeOne(dialog);
    });
    openCharts_.append(dialog);

    dialog->show();
}
