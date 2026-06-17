#pragma once

// Reusable UI widget factories: section titles, metric cards, status labels.


#include <QString>

class QLabel;
class QFrame;
class QTabWidget;
class QWidget;

// Styled label for section headings.
QLabel *makeSectionTitle(const QString &text);
// Compact metric card with selectable value label.
QFrame *makeMetricCard(const QString &title, const QString &value,
                       QLabel **valueLabel);
// Compact label for inline toolbar placement.
QLabel *makeToolbarLabel(const QString &text);
// Word-wrapping, selectable status label with severity styling.
QLabel *makeStatusSummaryLabel(const QString &text,
                               const QString &tooltip = QString(),
                               const QString &severity = "neutral");
// Applies production tab widget settings.
void configureWorkspaceTabsForRelease(QTabWidget *tabs);
// Forces style recalculation after dynamic property changes.
void repolish(QWidget *widget);
// Finds and activates the tab page containing the given widget.
bool activateTabContainingWidget(QTabWidget *tabs, QWidget *widget);
