#pragma once

// ConfirmDialogBuilder — builds a styled confirmation dialog for dangerous operations.
// Extracted from MainWindow::confirmDangerousOperation() to reduce file size.

#include <QString>
#include <QStringList>

class QWidget;

namespace ConfirmDialogBuilder {

bool confirm(QWidget* parent, const QString& title, const QString& summary, const QStringList& details,
             const QString& confirmText, const QString& theme);

} // namespace ConfirmDialogBuilder
