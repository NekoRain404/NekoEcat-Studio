#pragma once

// Reusable UI widget factories: section titles, metric cards, status labels.


#include <QString>

class QLabel;
class QFrame;
class QTabWidget;
class QWidget;

QLabel *makeSectionTitle(const QString &text);
QFrame *makeMetricCard(const QString &title, const QString &value,
                       QLabel **valueLabel);
QLabel *makeToolbarLabel(const QString &text);
QLabel *makeStatusSummaryLabel(const QString &text,
                               const QString &tooltip = QString(),
                               const QString &severity = "neutral");
void configureWorkspaceTabsForRelease(QTabWidget *tabs);
void repolish(QWidget *widget);
bool activateTabContainingWidget(QTabWidget *tabs, QWidget *widget);
