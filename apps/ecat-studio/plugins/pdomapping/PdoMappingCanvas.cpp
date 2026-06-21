#include "PdoMappingCanvas.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLinearGradient>
#include <QMimeData>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

static constexpr int kMaxBitSize = 1500 * 8;

PdoMappingCanvas::PdoMappingCanvas(QWidget *parent) : QWidget(parent) {
  setAcceptDrops(true);
  setMinimumSize(600, 400);
}

void PdoMappingCanvas::setSyncManagers(const QVector<SyncManagerBlock> &sms) {
  sms_ = sms;
  update();
}

QVector<SyncManagerBlock> PdoMappingCanvas::syncManagers() const { return sms_; }

void PdoMappingCanvas::setSelectedEntry(int smIndex, int entryIndex) {
  selectedSm_ = smIndex;
  selectedEntry_ = entryIndex;
  update();
}

void PdoMappingCanvas::clearSelection() {
  selectedSm_ = -1;
  selectedEntry_ = -1;
  update();
}

int PdoMappingCanvas::selectedSmIndex() const { return selectedSm_; }
int PdoMappingCanvas::selectedEntryIndex() const { return selectedEntry_; }

void PdoMappingCanvas::setErrorHighlight(int smIndex, int entryIndex, bool hasError) {
  if (smIndex >= 0 && smIndex < sms_.size()) {
    if (entryIndex >= 0 && entryIndex < sms_[smIndex].entries.size()) {
      sms_[smIndex].entries[entryIndex].hasError = hasError;
      update();
    }
  }
}

void PdoMappingCanvas::clearAllErrors() {
  for (auto &sm : sms_) {
    for (auto &e : sm.entries) {
      e.hasError = false;
    }
  }
  update();
}

QSize PdoMappingCanvas::sizeHint() const {
  int w = metrics_.padding * 2 + sms_.size() * (metrics_.smWidth + metrics_.smSpacing);
  int maxEntries = 0;
  for (const auto &sm : sms_) {
    maxEntries = qMax(maxEntries, sm.entries.size());
  }
  int h = metrics_.padding * 2 + metrics_.smHeaderHeight + maxEntries * metrics_.entryHeight + 40;
  return QSize(qMax(w, 600), qMax(h, 400));
}

QSize PdoMappingCanvas::minimumSizeHint() const { return QSize(600, 400); }

void PdoMappingCanvas::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.fillRect(rect(), QColor("#1e1e2e"));

  int x = metrics_.padding;
  for (int i = 0; i < sms_.size(); ++i) {
    paintSyncManager(p, sms_[i], x, metrics_.padding);
    x += metrics_.smWidth + metrics_.smSpacing;
  }
}

void PdoMappingCanvas::paintSyncManager(QPainter &p, const SyncManagerBlock &sm, int x, int y) {
  int totalHeight = metrics_.smHeaderHeight + sm.entries.size() * metrics_.entryHeight + 8;

  QPainterPath path;
  path.addRoundedRect(x, y, metrics_.smWidth, totalHeight, 6, 6);
  QColor bgColor = sm.direction == PdoEntryDirection::Input ? QColor("#1a2332") : QColor("#231a2e");
  p.fillPath(path, bgColor);
  p.setPen(QPen(QColor("#3a3a5c"), 1));
  p.drawPath(path);

  QLinearGradient headerGrad(x, y, x + metrics_.smWidth, y);
  if (sm.direction == PdoEntryDirection::Input) {
    headerGrad.setColorAt(0, QColor("#1e88e5"));
    headerGrad.setColorAt(1, QColor("#1565c0"));
  } else {
    headerGrad.setColorAt(0, QColor("#7b1fa2"));
    headerGrad.setColorAt(1, QColor("#6a1b9a"));
  }
  QPainterPath headerPath;
  headerPath.addRoundedRect(x, y, metrics_.smWidth, metrics_.smHeaderHeight, 6, 6);
  QRectF flatBottom(x, y + metrics_.smHeaderHeight - 6, metrics_.smWidth, 6);
  headerPath.addRect(flatBottom);
  p.fillPath(headerPath, headerGrad);

  p.setPen(Qt::white);
  QFont font = p.font();
  font.setBold(true);
  font.setPointSize(10);
  p.setFont(font);
  QString dirLabel = sm.direction == PdoEntryDirection::Input ? "IN" : "OUT";
  p.drawText(x + 8, y, metrics_.smWidth - 16, metrics_.smHeaderHeight,
             Qt::AlignVCenter | Qt::AlignLeft,
             QString("SM%1 [%2] %3").arg(sm.index).arg(dirLabel, sm.name));

  int ey = y + metrics_.smHeaderHeight + 4;
  for (int i = 0; i < sm.entries.size(); ++i) {
    bool sel = (selectedSm_ == sm.index && selectedEntry_ == i);
    paintEntry(p, sm.entries[i], x + 4, ey, metrics_.smWidth - 8, sel, sm.entries[i].hasError);
    ey += metrics_.entryHeight;
  }
}

void PdoMappingCanvas::paintEntry(QPainter &p, const PdoCanvasEntry &entry, int x, int y, int width,
                                   bool selected, bool hasError) {
  int h = metrics_.entryHeight - 2;

  QColor bg;
  if (hasError) {
    bg = QColor("#5c1a1a");
  } else if (selected) {
    bg = QColor("#2a3a5c");
  } else {
    bg = QColor("#252540");
  }

  QPainterPath path;
  path.addRoundedRect(x, y, width, h, 4, 4);
  p.fillPath(path, bg);

  if (selected) {
    p.setPen(QPen(QColor("#60a5fa"), 2));
    p.drawPath(path);
  } else if (hasError) {
    p.setPen(QPen(QColor("#ef4444"), 1.5));
    p.drawPath(path);
  } else {
    p.setPen(QPen(QColor("#3a3a5c"), 1));
    p.drawPath(path);
  }

  paintBitIndicator(p, entry.bitSize, x + 4, y + 3, h - 6, entry.direction);

  p.setPen(hasError ? QColor("#fca5a5") : QColor("#e0e0e0"));
  QFont font = p.font();
  font.setPointSize(8);
  font.setBold(false);
  p.setFont(font);

  int textX = x + metrics_.bitBarWidth + 10;
  int textW = width - metrics_.bitBarWidth - 16;
  p.drawText(textX, y, textW, h, Qt::AlignVCenter | Qt::AlignLeft,
             QString("%1.%2 %3").arg(entry.index, entry.subIndex, entry.name));

  p.setPen(hasError ? QColor("#f87171") : QColor("#888"));
  QString meta = QString("%1 [%2b]").arg(entry.dataType).arg(entry.bitSize);
  p.drawText(textX, y, textW, h, Qt::AlignVCenter | Qt::AlignRight, meta);
}

void PdoMappingCanvas::paintBitIndicator(QPainter &p, int bitSize, int x, int y, int height,
                                          PdoEntryDirection dir) {
  double ratio = qMin(1.0, static_cast<double>(bitSize) / 128.0);
  int barHeight = static_cast<int>(height * ratio);
  QColor color = dir == PdoEntryDirection::Input ? QColor("#42a5f5") : QColor("#ab47bc");
  p.setPen(Qt::NoPen);
  p.setBrush(color);
  p.drawRoundedRect(x, y + height - barHeight, metrics_.bitBarWidth - 2, barHeight, 2, 2);
}

QRect PdoMappingCanvas::entryRect(int smIndex, int entryIndex) const {
  int smX = metrics_.padding + smIndex * (metrics_.smWidth + metrics_.smSpacing);
  int smY = metrics_.padding;
  int ey = smY + metrics_.smHeaderHeight + 4 + entryIndex * metrics_.entryHeight;
  return QRect(smX + 4, ey, metrics_.smWidth - 8, metrics_.entryHeight - 2);
}

QPair<int, int> PdoMappingCanvas::entryAtPos(const QPoint &pos) const {
  for (int i = 0; i < sms_.size(); ++i) {
    for (int j = 0; j < sms_[i].entries.size(); ++j) {
      if (entryRect(i, j).contains(pos)) {
        return {sms_[i].index, j};
      }
    }
  }
  return {-1, -1};
}

void PdoMappingCanvas::mousePressEvent(QMouseEvent *event) {
  auto [smIdx, entryIdx] = entryAtPos(event->pos());
  if (smIdx >= 0) {
    setSelectedEntry(smIdx, entryIdx);
    emit entrySelected(smIdx, entryIdx);
  } else {
    clearSelection();
  }
}

void PdoMappingCanvas::mouseDoubleClickEvent(QMouseEvent *event) {
  auto [smIdx, entryIdx] = entryAtPos(event->pos());
  if (smIdx >= 0) {
    emit entryDoubleClicked(smIdx, entryIdx);
  }
}

void PdoMappingCanvas::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasFormat("application/x-pdo-entry")) {
    event->acceptProposedAction();
  }
}

void PdoMappingCanvas::dragMoveEvent(QDragMoveEvent *event) {
  if (event->mimeData()->hasFormat("application/x-pdo-entry")) {
    event->acceptProposedAction();
  }
}

void PdoMappingCanvas::dropEvent(QDropEvent *event) {
  auto [smIdx, entryIdx] = entryAtPos(event->pos());
  if (smIdx >= 0) {
    QByteArray data = event->mimeData()->data("application/x-pdo-entry");
    QStringList parts = QString::fromUtf8(data).split(":");
    if (parts.size() == 2) {
      int fromSm = parts[0].toInt();
      int fromEntry = parts[1].toInt();
      emit entryMoved(fromSm, fromEntry, smIdx, entryIdx);
    }
  }
  event->acceptProposedAction();
}
