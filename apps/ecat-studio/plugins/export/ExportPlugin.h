#pragma once

/// @brief Workspace plugin for CSV/text data export operations.
///
/// @details The Export workspace provides unified export functionality for
/// all data tables and text panels in NekoEcat Studio. It supports
/// exporting to CSV (for tables and trees) and plain text (for raw
/// text panels).
///
/// Supported export targets:
///   - PDO Map table → CSV
///   - SDO Dictionary table → CSV
///   - SDO History table → CSV
///   - ESI Repository table → CSV
///   - Watch table → CSV
///   - Startup SDO table → CSV
///   - Topology tree → CSV
///   - Host Health table → CSV
///   - Raw text panels (Master, Slave, PDO, SDO, XML) → TXT
///
/// @par Constructor
///   ExportPlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "export"
///   - defaultOrder: 85
///   - visible: always true
///
/// @see WorkspacePlugin, MainWindow

#include "plugins/WorkspacePlugin.h"

class QTableWidget;
class QTreeWidget;
class QPlainTextEdit;
class ServiceContainer;

class ExportPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit ExportPlugin(ServiceContainer* container, QObject* parent = nullptr);

    // WorkspacePlugin identity
    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    // Lifecycle
    void activate() override;
    void deactivate() override;
    void onSettingsChanged(const AppSettings& settings) override;
    void onConnectionChanged(bool connected) override;

    // Table-based exports — pass the source table and a descriptive label.
    // Returns true on success, false on cancel or error.
    bool exportTableCsv(QWidget* parent, QTableWidget* table, const QString& defaultName, const QString& logSource,
                        bool visibleOnly = true);

    // Tree-based export (topology).
    bool exportTreeCsv(QWidget* parent, QTreeWidget* tree, const QString& defaultName, const QString& logSource);

    // Plain-text export (raw text panels).
    bool exportPlainText(QWidget* parent, QPlainTextEdit* textEdit, const QString& defaultName,
                         const QString& logSource, const QString& filter = "Text (*.txt);;All (*)");

private:
    void buildUi();

    ServiceContainer* container_;
    QWidget* containerWidget_ = nullptr;
};
