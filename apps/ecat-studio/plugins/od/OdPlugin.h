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

class OdPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit OdPlugin(ServiceContainer *container,
                    QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  // Lifecycle
  void activate() override;
  void deactivate() override;
  void onSettingsChanged(const AppSettings &settings) override;
  void onConnectionChanged(bool connected) override;

  // ── OD Table ──────────────────────────────────────────────────────
  QTableWidget *sdoTable() const;
  QLineEdit *sdoFilter() const;
  QComboBox *sdoObjectFilter() const;
  QComboBox *sdoAccessFilter() const;
  QLabel *sdoSummaryLabel() const;

  // ── SDO Inspector ─────────────────────────────────────────────────
  QTableWidget *sdoTargetTable() const;
  QLineEdit *sdoIndex() const;
  QLineEdit *sdoSubIndex() const;
  QComboBox *sdoType() const;
  QLineEdit *sdoValue() const;
  QLineEdit *sdoWriteValue() const;
  QLabel *sdoInspectorLabel() const;
  QPushButton *useSdoValueButton() const;

  // ── SDO Target Trail ──────────────────────────────────────────────
  QTableWidget *sdoTargetTrailTable() const;
  QLabel *sdoTargetTrailDetailLabel() const;

  // ── Object Bookmarks ──────────────────────────────────────────────
  QTableWidget *objectBookmarkTable() const;
  QLabel *objectBookmarkDetailLabel() const;

  // ── SDO History ───────────────────────────────────────────────────
  QTableWidget *sdoHistoryTable() const;

  // ── OD Table Update ───────────────────────────────────────────────
  void updateSdoTableSummary(int total, int visible, int withEvidence,
                             int failed, int writable);

  // ── SDO Target Trail ──────────────────────────────────────────────
  void ensureSdoTargetTrailTable();
  void updateSdoTargetTrailRowDetail(const QString &text,
                                     const QString &severity,
                                     const QString &tooltip);

  // ── Object Bookmarks ──────────────────────────────────────────────
  void ensureObjectBookmarkTable();
  void updateObjectBookmarkRowDetail(const QString &text,
                                     const QString &severity,
                                     const QString &tooltip);

  // ── SDO History ───────────────────────────────────────────────────
  void ensureSdoHistoryTable();

  // ── OD Table Evidence Update ──────────────────────────────────────
  void updateSdoTableEvidenceRow(int row, const QString &value,
                                 const QString &status, const QString &time,
                                 const QColor &statusColor,
                                 const QColor &valueBackground);

  // ── SDO Target Panel ──────────────────────────────────────────────
  void updateSdoTargetPanelRows(
      const QList<QPair<QString, QString>> &rows,
      const QMap<QString, QString> &rowColors,
      const QMap<QString, QString> &rowActions);

  // ── SDO Inspector Label ───────────────────────────────────────────
  void updateSdoInspectorLabel(const QString &text, const QString &state);

signals:
  void sdoFilterChanged(const QString &text);
  void sdoTableSelectionChanged();
  void sdoTargetTrailSelectionChanged();
  void objectBookmarkSelectionChanged();
  void sdoHistorySelectionChanged();
  void sdoTargetPanelRowDoubleClicked(int row);
  void sdoTargetPanelRowActionRequested();
  void sdoTargetPanelCopyRequested();

private:
  void buildUi();
  void buildOdTab(QWidget *parent);
  void buildInspectorPanel(QWidget *parent);
  void buildTargetTrailTab(QWidget *parent);
  void buildBookmarkTab(QWidget *parent);
  void buildHistoryTab(QWidget *parent);

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;

  // OD tab
  QTableWidget *sdoTable_ = nullptr;
  QLineEdit *sdoFilter_ = nullptr;
  QComboBox *sdoObjectFilter_ = nullptr;
  QComboBox *sdoAccessFilter_ = nullptr;
  QLabel *sdoSummaryLabel_ = nullptr;

  // SDO inspector
  QLineEdit *sdoIndex_ = nullptr;
  QLineEdit *sdoSubIndex_ = nullptr;
  QComboBox *sdoType_ = nullptr;
  QLineEdit *sdoValue_ = nullptr;
  QLineEdit *sdoWriteValue_ = nullptr;
  QPushButton *useSdoValueButton_ = nullptr;
  QLabel *sdoInspectorLabel_ = nullptr;
  QTableWidget *sdoTargetTable_ = nullptr;

  // Target trail
  QTableWidget *sdoTargetTrailTable_ = nullptr;
  QLabel *sdoTargetTrailDetailLabel_ = nullptr;

  // Bookmarks
  QTableWidget *objectBookmarkTable_ = nullptr;
  QLabel *objectBookmarkDetailLabel_ = nullptr;

  // History
  QTableWidget *sdoHistoryTable_ = nullptr;

  // State
  QSet<QString> rememberedSdoTargetTrailKeys_;
};
