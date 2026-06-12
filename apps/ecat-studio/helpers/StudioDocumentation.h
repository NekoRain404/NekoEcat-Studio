#pragma once

// HTML documentation strings for the built-in user manual and about dialog.


#include <QString>

class QObject;
class QHBoxLayout;
class QStyle;
class QTextBrowser;

// Holds the search toolbar layout for the documentation browser.
struct ManualSearchControls {
  QHBoxLayout *layout = nullptr;
};

// Theme-appropriate CSS for the documentation browser.
QString documentationBrowserStyle(const QString &objectName, bool lightTheme);
// Creates a styled, read-only, frameless documentation browser.
QTextBrowser *makeDocumentationBrowser(const QString &objectName,
                                       bool lightTheme,
                                       bool openExternalLinks = false);
// Builds the search toolbar with wrap-around find and contents navigation.
ManualSearchControls
makeManualSearchControls(QObject *owner, QTextBrowser *browser, QStyle *style,
                         const QString &searchPlaceholder,
                         const QString &previousText, const QString &nextText,
                         const QString &contentsText);
// Substitutes version/path placeholders and adapts colors for dark theme.
QString finalizeDocumentationHtml(const QString &htmlTemplate,
                                  const QString &version,
                                  const QString &activeMaster,
                                  const QString &runtimePath, bool lightTheme);
