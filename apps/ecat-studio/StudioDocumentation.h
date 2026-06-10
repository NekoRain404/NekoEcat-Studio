#pragma once

#include <QString>

class QObject;
class QHBoxLayout;
class QStyle;
class QTextBrowser;

struct ManualSearchControls {
  QHBoxLayout *layout = nullptr;
};

QString documentationBrowserStyle(const QString &objectName, bool lightTheme);
QTextBrowser *makeDocumentationBrowser(const QString &objectName,
                                       bool lightTheme,
                                       bool openExternalLinks = false);
ManualSearchControls
makeManualSearchControls(QObject *owner, QTextBrowser *browser, QStyle *style,
                         const QString &searchPlaceholder,
                         const QString &previousText, const QString &nextText,
                         const QString &contentsText);
QString finalizeDocumentationHtml(const QString &htmlTemplate,
                                  const QString &version,
                                  const QString &activeMaster,
                                  const QString &runtimePath, bool lightTheme);
