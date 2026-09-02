#pragma once

// OdPlugin — Object Dictionary workspace plugin.
// Extracted from MainWindow SDO workspace files. Owns the OD table, filter,
// SDO inspector, target panel, target trail, bookmarks, and history widgets.
// MainWindow delegates UI updates through setter methods while retaining
// data orchestration and business logic.

#include "plugins/WorkspacePlugin.h"

#include <QMap>
#include <QSet>
#include <QStringList>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;
class ServiceContainer;
struct SdoInspectorWidgets;

/// @brief Workspace plugin for Object Dictionary browsing and SDO read/write.
///
/// @details The Object Dictionary workspace provides full access to the
/// EtherCAT Object Dictionary including an OD table with filtering, an SDO
/// inspector for read/write operations, target trail tracking, object
/// bookmarks, and SDO history. MainWindow delegates UI updates through
/// setter methods while retaining data orchestration and business logic.
///
/// Features:
///   - **OD table**: Filterable table of SDO objects with evidence status.
///   - **SDO Inspector**: Read/write SDO values with type-aware controls.
///   - **Target Trail**: Tracks recently accessed SDO targets.
///   - **Object Bookmarks**: Bookmarked SDO entries for quick access.
///   - **SDO History**: Log of past SDO read/write operations.
///
/// @par Plugin Identity
///   - id: "od"
///   - defaultOrder: 20
///   - visible: always true
///
/// @see WorkspacePlugin, MainWindow, SdoService

class OdPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    /// Constructs the Object Dictionary plugin, building the OD table and inspector UI.
    /// @param container  Service container for accessing domain services
    /// @param parent     Qt parent object (typically MainWindow)
    explicit OdPlugin(ServiceContainer* container, QObject* parent = nullptr);

    // WorkspacePlugin identity
    QString id() const override;            ///< Returns "od"
    QString displayName() const override;   ///< Returns "Object Dictionary"
    QString displayNameZh() const override; ///< Returns "对象字典"
    QIcon icon() const override;            ///< Returns the OD theme icon
    QWidget* widget() override;             ///< Returns the root container widget
    int defaultOrder() const override;      ///< Returns 20
    bool visible() const override;          ///< Returns true (always visible)

    // Lifecycle
    void activate() override;                                     ///< Called when user switches to this tab
    void deactivate() override;                                   ///< Called when user switches away
    void onSettingsChanged(const AppSettings& settings) override; ///< Reacts to settings changes
    void onConnectionChanged(bool connected) override;            ///< Reacts to daemon connection state

    // ── OD Table ──────────────────────────────────────────────────────
    QTableWidget* sdoTable() const;     ///< Returns the main OD table widget
    QLineEdit* sdoFilter() const;       ///< Returns the text filter input for OD rows
    QComboBox* sdoObjectFilter() const; ///< Returns the object category filter combo
    QComboBox* sdoAccessFilter() const; ///< Returns the access type filter combo (RO/RW/WO)
    QLabel* sdoSummaryLabel() const;    ///< Returns the summary label with row counts

    // ── SDO Inspector ─────────────────────────────────────────────────
    QTableWidget* sdoTargetTable() const;   ///< Returns the SDO target table in the inspector
    QLineEdit* sdoIndex() const;            ///< Returns the index input field
    QLineEdit* sdoSubIndex() const;         ///< Returns the subindex input field
    QComboBox* sdoType() const;             ///< Returns the data type selector combo
    QLineEdit* sdoValue() const;            ///< Returns the read-only current value display
    QLineEdit* sdoWriteValue() const;       ///< Returns the writable value input field
    QLabel* sdoInspectorLabel() const;      ///< Returns the inspector status label
    QPushButton* useSdoValueButton() const; ///< Returns the "Use SDO Value" button

    // ── SDO Target Trail ──────────────────────────────────────────────
    QTableWidget* sdoTargetTrailTable() const; ///< Returns the target trail history table
    QLabel* sdoTargetTrailDetailLabel() const; ///< Returns the target trail detail label

    // ── Object Bookmarks ──────────────────────────────────────────────
    QTableWidget* objectBookmarkTable() const; ///< Returns the bookmarks table widget
    QLabel* objectBookmarkDetailLabel() const; ///< Returns the bookmark detail label

    // ── SDO History ───────────────────────────────────────────────────
    QTableWidget* sdoHistoryTable() const; ///< Returns the SDO read/write history table

    // ── OD Table Update ───────────────────────────────────────────────
    /// Updates the OD table summary with row count statistics.
    /// @param total        Total number of OD entries
    /// @param visible      Number of entries visible after filtering
    /// @param withEvidence Number of entries with evidence data
    /// @param failed       Number of entries with failed reads
    /// @param writable     Number of entries with write access
    void updateSdoTableSummary(int total, int visible, int withEvidence, int failed, int writable);

    // ── SDO Target Trail ──────────────────────────────────────────────
    void ensureSdoTargetTrailTable(); ///< Creates the target trail table if not yet built
    /// Updates a target trail row with detail information.
    /// @param text      Detail text to display
    /// @param severity  Severity level for coloring (e.g. "ok", "error")
    /// @param tooltip   Tooltip text for the row
    void updateSdoTargetTrailRowDetail(const QString& text, const QString& severity, const QString& tooltip);

    // ── Object Bookmarks ──────────────────────────────────────────────
    void ensureObjectBookmarkTable(); ///< Creates the bookmarks table if not yet built
    /// Updates a bookmark row with detail information.
    /// @param text      Detail text to display
    /// @param severity  Severity level for coloring
    /// @param tooltip   Tooltip text for the row
    void updateObjectBookmarkRowDetail(const QString& text, const QString& severity, const QString& tooltip);

    // ── SDO History ───────────────────────────────────────────────────
    void ensureSdoHistoryTable(); ///< Creates the history table if not yet built

    // ── OD Table Evidence Update ──────────────────────────────────────
    /// Updates a single OD table row with evidence data and coloring.
    /// @param row             Row index in the OD table
    /// @param value           Evidence value string
    /// @param status          Status text (e.g. "ok", "failed")
    /// @param time            Timestamp of the evidence
    /// @param statusColor     Background color for the status cell
    /// @param valueBackground Background color for the value cell
    void updateSdoTableEvidenceRow(int row, const QString& value, const QString& status, const QString& time,
                                   const QColor& statusColor, const QColor& valueBackground);

    // ── SDO Target Panel ──────────────────────────────────────────────
    /// Populates the SDO target panel with row data, colors, and action labels.
    /// @param rows       List of label-value pairs for each row
    /// @param rowColors  Map of row label to background color
    /// @param rowActions Map of row label to action button text
    void updateSdoTargetPanelRows(const QList<QPair<QString, QString>>& rows, const QMap<QString, QString>& rowColors,
                                  const QMap<QString, QString>& rowActions);

    // ── SDO Inspector Label ───────────────────────────────────────────
    /// Updates the inspector status label text and visual state.
    /// @param text   Status message to display
    /// @param state  Visual state qualifier (e.g. "ok", "error", "pending")
    void updateSdoInspectorLabel(const QString& text, const QString& state);

signals:
    void sdoFilterChanged(const QString& text);   ///< Emitted when the OD filter text changes
    void sdoTableSelectionChanged();              ///< Emitted when the OD table selection changes
    void sdoTargetTrailSelectionChanged();        ///< Emitted when target trail selection changes
    void objectBookmarkSelectionChanged();        ///< Emitted when bookmark selection changes
    void sdoHistorySelectionChanged();            ///< Emitted when history row selection changes
    void sdoTargetPanelRowDoubleClicked(int row); ///< Emitted on target panel row double-click
    void sdoTargetPanelRowActionRequested();      ///< Emitted when a target panel action is triggered
    void sdoTargetPanelCopyRequested();           ///< Emitted when copy is requested from target panel

private:
    void buildUi();                            ///< Builds the full OD workspace layout
    void buildOdTab(QWidget* parent);          ///< Builds the OD table and filter controls
    void buildInspectorPanel(QWidget* parent); ///< Builds the SDO inspector panel
    void buildTargetTrailTab(QWidget* parent); ///< Builds the target trail tab
    void buildBookmarkTab(QWidget* parent);    ///< Builds the bookmarks tab
    void buildHistoryTab(QWidget* parent);     ///< Builds the SDO history tab

    ServiceContainer* container_;
    QWidget* containerWidget_ = nullptr;

    // OD tab
    QTableWidget* sdoTable_ = nullptr;
    QLineEdit* sdoFilter_ = nullptr;
    QComboBox* sdoObjectFilter_ = nullptr;
    QComboBox* sdoAccessFilter_ = nullptr;
    QLabel* sdoSummaryLabel_ = nullptr;

    // SDO inspector
    QLineEdit* sdoIndex_ = nullptr;
    QLineEdit* sdoSubIndex_ = nullptr;
    QComboBox* sdoType_ = nullptr;
    QLineEdit* sdoValue_ = nullptr;
    QLineEdit* sdoWriteValue_ = nullptr;
    QPushButton* useSdoValueButton_ = nullptr;
    QLabel* sdoInspectorLabel_ = nullptr;
    QTableWidget* sdoTargetTable_ = nullptr;

    // Target trail
    QTableWidget* sdoTargetTrailTable_ = nullptr;
    QLabel* sdoTargetTrailDetailLabel_ = nullptr;

    // Bookmarks
    QTableWidget* objectBookmarkTable_ = nullptr;
    QLabel* objectBookmarkDetailLabel_ = nullptr;

    // History
    QTableWidget* sdoHistoryTable_ = nullptr;

    // State
    QSet<QString> rememberedSdoTargetTrailKeys_;
};
