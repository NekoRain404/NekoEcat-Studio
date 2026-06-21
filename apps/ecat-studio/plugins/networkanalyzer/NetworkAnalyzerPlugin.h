#pragma once

// NetworkAnalyzerPlugin — packet capture and protocol analysis workspace.

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QLineEdit;
class QPushButton;
class QSplitter;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class NetworkAnalyzerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit NetworkAnalyzerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct PacketEntry {
    QDateTime timestamp;
    QString source;
    QString destination;
    QString protocol;
    int size;
    QString summary;
  };

  struct FilterCondition {
    QString field;
    QString op;
    QString value;
  };

  struct ProtocolStats {
    QString protocol;
    int packetCount;
    int byteCount;
  };

  void startCapture();
  void stopCapture();
  bool isCapturing() const;

  void addPacket(const PacketEntry &packet);
  int packetCount() const;
  void clearPackets();

  void addFilter(const FilterCondition &filter);
  void removeFilter(int index);
  int filterCount() const;

  void applyFilters();
  int filteredCount() const;

  void exportCapture(const QString &path);

  QTableWidget *packetTable() const;
  QTableWidget *statisticsTable() const;
  QTableWidget *filterTable() const;
  QTextEdit *decodeView() const;
  QLabel *statusLabel() const;

signals:
  void captureStarted();
  void captureStopped();
  void packetAdded(int index);
  void packetSelected(int index);

public slots:
  void selectPacket(int index);

private:
  void buildUi();
  void rebuildPacketTable();
  void rebuildStatistics();
  void rebuildFilterTable();
  void updateDecodeView(int index);

  QWidget *containerWidget_ = nullptr;
  QTableWidget *packetTable_ = nullptr;
  QTableWidget *statisticsTable_ = nullptr;
  QTableWidget *filterTable_ = nullptr;
  QTextEdit *decodeView_ = nullptr;
  QLineEdit *filterFieldEdit_ = nullptr;
  QLineEdit *filterValueEdit_ = nullptr;
  QPushButton *startBtn_ = nullptr;
  QPushButton *stopBtn_ = nullptr;
  QPushButton *clearBtn_ = nullptr;
  QPushButton *addFilterBtn_ = nullptr;
  QPushButton *removeFilterBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QTabWidget *tabs_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QVector<PacketEntry> packets_;
  QVector<FilterCondition> filters_;
  QVector<int> filteredIndices_;
  QVector<ProtocolStats> stats_;
  bool capturing_ = false;
  int selectedPacket_ = -1;
};
