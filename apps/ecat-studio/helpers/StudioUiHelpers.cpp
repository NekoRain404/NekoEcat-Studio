#include "StudioUiHelpers.h"

#include <QFrame>
#include <QLabel>
#include <QSizePolicy>
#include <QStyle>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

QLabel *makeSectionTitle(const QString &text) {
  auto *label = new QLabel(text);
  label->setObjectName("sectionTitle");
  return label;
}

QFrame *makeMetricCard(const QString &title, const QString &value,
                       QLabel **valueLabel) {
  auto *card = new QFrame;
  card->setObjectName("metricCard");
  card->setMinimumHeight(86);
  card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = new QVBoxLayout(card);
  layout->setContentsMargins(14, 12, 14, 12);
  layout->setSpacing(6);

  auto *titleLabel = new QLabel(title);
  titleLabel->setObjectName("metricTitle");

  auto *label = new QLabel(value);
  label->setObjectName("metricValue");
  label->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

  layout->addWidget(titleLabel);
  layout->addStretch(1);
  layout->addWidget(label);

  if (valueLabel) {
    *valueLabel = label;
  }
  label->setToolTip(value);
  return card;
}

QLabel *makeToolbarLabel(const QString &text) {
  auto *label = new QLabel(text);
  label->setObjectName("toolbarLabel");
  return label;
}

QLabel *makeStatusSummaryLabel(const QString &text, const QString &tooltip,
                               const QString &severity) {
  auto *label = new QLabel(text);
  label->setObjectName("statusSummary");
  label->setProperty("severity", severity);
  label->setWordWrap(true);
  label->setTextInteractionFlags(Qt::TextSelectableByMouse);
  if (!tooltip.isEmpty()) {
    label->setToolTip(tooltip);
  }
  return label;
}

void configureWorkspaceTabsForRelease(QTabWidget *tabs) {
  if (!tabs) {
    return;
  }

  tabs->setDocumentMode(true);
  tabs->setMovable(true);
  tabs->setUsesScrollButtons(true);
  tabs->setElideMode(Qt::ElideNone);
  if (auto *bar = tabs->tabBar()) {
    bar->setExpanding(false);
    bar->setUsesScrollButtons(true);
  }
}

void repolish(QWidget *widget) {
  if (!widget) {
    return;
  }
  widget->style()->unpolish(widget);
  widget->style()->polish(widget);
  widget->update();
}

bool activateTabContainingWidget(QTabWidget *tabs, QWidget *widget) {
  if (!tabs || !widget) {
    return false;
  }

  QWidget *page = widget;
  while (page && page->parentWidget() && tabs->indexOf(page) < 0) {
    page = page->parentWidget();
  }
  const int index = page ? tabs->indexOf(page) : -1;
  if (index < 0) {
    return false;
  }

  tabs->setCurrentIndex(index);
  return true;
}
