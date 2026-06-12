#include "StudioDocumentation.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStyle>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextDocument>

QString documentationBrowserStyle(const QString &objectName, bool lightTheme) {
  const QString selector = QString("QTextBrowser#%1").arg(objectName);
  return lightTheme
             ? QStringLiteral(
                   "%1 { background: #ffffff; border: 1px solid #d9e1ec; "
                   "border-radius: 8px; padding: 14px; }")
                   .arg(selector)
             : QStringLiteral(
                   "%1 { background: #151b25; border: 1px solid #2a3546; "
                   "border-radius: 8px; padding: 14px; }")
                   .arg(selector);
}

QTextBrowser *makeDocumentationBrowser(const QString &objectName,
                                       bool lightTheme,
                                       bool openExternalLinks) {
  auto *browser = new QTextBrowser;
  browser->setObjectName(objectName);
  browser->setOpenExternalLinks(openExternalLinks);
  browser->setReadOnly(true);
  browser->setFrameShape(QFrame::NoFrame);
  browser->setStyleSheet(documentationBrowserStyle(objectName, lightTheme));
  return browser;
}

ManualSearchControls
makeManualSearchControls(QObject *owner, QTextBrowser *browser, QStyle *style,
                         const QString &searchPlaceholder,
                         const QString &previousText, const QString &nextText,
                         const QString &contentsText) {
  ManualSearchControls controls;
  auto *layout = new QHBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);

  auto *search = new QLineEdit;
  search->setObjectName("manualSearch");
  search->setPlaceholderText(searchPlaceholder);

  auto *findPrevious = new QPushButton(previousText);
  findPrevious->setIcon(style->standardIcon(QStyle::SP_ArrowBack));
  auto *findNext = new QPushButton(nextText);
  findNext->setIcon(style->standardIcon(QStyle::SP_ArrowForward));
  auto *contents = new QPushButton(contentsText);
  contents->setIcon(style->standardIcon(QStyle::SP_FileDialogContentsView));

  layout->addWidget(search, 1);
  layout->addWidget(findPrevious);
  layout->addWidget(findNext);
  layout->addWidget(contents);

  auto findText = [browser, search](QTextDocument::FindFlags flags = {}) {
    const QString term = search->text().trimmed();
    if (term.isEmpty()) {
      return;
    }
    if (browser->find(term, flags)) {
      return;
    }
    QTextCursor cursor = browser->textCursor();
    cursor.movePosition(flags.testFlag(QTextDocument::FindBackward)
                            ? QTextCursor::End
                            : QTextCursor::Start);
    browser->setTextCursor(cursor);
    browser->find(term, flags);
  };
  QObject::connect(search, &QLineEdit::textChanged, owner,
                   [browser, findText](const QString &text) {
                     QTextCursor cursor = browser->textCursor();
                     cursor.movePosition(QTextCursor::Start);
                     browser->setTextCursor(cursor);
                     if (!text.trimmed().isEmpty()) {
                       findText();
                     }
                   });
  QObject::connect(findPrevious, &QPushButton::clicked, owner,
                   [findText] { findText(QTextDocument::FindBackward); });
  QObject::connect(findNext, &QPushButton::clicked, owner,
                   [findText] { findText(); });
  QObject::connect(contents, &QPushButton::clicked, owner,
                   [browser] { browser->scrollToAnchor("contents"); });

  controls.layout = layout;
  return controls;
}

QString finalizeDocumentationHtml(const QString &htmlTemplate,
                                  const QString &version,
                                  const QString &activeMaster,
                                  const QString &runtimePath, bool lightTheme) {
  QString html = QString(htmlTemplate).arg(version, activeMaster, runtimePath);
  if (lightTheme) {
    return html;
  }

  html.replace("#172033", "#e6edf5");
  html.replace("#0f172a", "#f8fafc");
  html.replace("#12376f", "#93c5fd");
  html.replace("#1f2937", "#dbeafe");
  html.replace("#f7f9fc", "#111722");
  html.replace("#d9e1ec", "#2a3546");
  html.replace("#eef4fb", "#172033");
  html.replace("#fff7ed", "#2b1d10");
  html.replace("#fef2f2", "#2a1214");
  html.replace("#eef2f7", "#1a2230");
  html.replace("#f0f4f9", "#1a2230");
  html.replace("#475569", "#b9c6d6");
  return html;
}
