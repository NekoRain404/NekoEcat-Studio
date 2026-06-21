#pragma once

#include <QVector>
#include <QWidget>

class QPaintEvent;
class QMouseEvent;
class QDragEnterEvent;
class QDropEvent;
class QDragMoveEvent;

enum class PdoEntryDirection { Input, Output };

struct PdoCanvasEntry {
  QString index;
  QString subIndex;
  QString name;
  QString dataType;
  int bitSize = 0;
  PdoEntryDirection direction = PdoEntryDirection::Input;
  int smIndex = 0;
  bool enabled = true;
  bool selected = false;
  bool hasError = false;
};

struct SyncManagerBlock {
  int index = 0;
  QString name;
  PdoEntryDirection direction = PdoEntryDirection::Input;
  QVector<PdoCanvasEntry> entries;
  bool enabled = true;
};

class PdoMappingCanvas : public QWidget {
  Q_OBJECT
public:
  explicit PdoMappingCanvas(QWidget *parent = nullptr);

  void setSyncManagers(const QVector<SyncManagerBlock> &sms);
  QVector<SyncManagerBlock> syncManagers() const;

  void setSelectedEntry(int smIndex, int entryIndex);
  void clearSelection();
  int selectedSmIndex() const;
  int selectedEntryIndex() const;

  void setErrorHighlight(int smIndex, int entryIndex, bool hasError);
  void clearAllErrors();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

signals:
  void entrySelected(int smIndex, int entryIndex);
  void entryMoved(int fromSm, int fromEntry, int toSm, int toEntry);
  void entryDoubleClicked(int smIndex, int entryIndex);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

private:
  struct LayoutMetrics {
    int smHeaderHeight = 32;
    int entryHeight = 26;
    int smWidth = 280;
    int smSpacing = 20;
    int padding = 12;
    int bitBarWidth = 6;
  };

  void paintSyncManager(QPainter &p, const SyncManagerBlock &sm, int x, int y);
  void paintEntry(QPainter &p, const PdoCanvasEntry &entry, int x, int y, int width, bool selected, bool hasError);
  void paintBitIndicator(QPainter &p, int bitSize, int x, int y, int height, PdoEntryDirection dir);
  QRect entryRect(int smIndex, int entryIndex) const;
  QPair<int, int> entryAtPos(const QPoint &pos) const;

  QVector<SyncManagerBlock> sms_;
  int selectedSm_ = -1;
  int selectedEntry_ = -1;
  LayoutMetrics metrics_;
};
