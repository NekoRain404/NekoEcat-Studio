// Stateless confirmation dialog for dangerous online operations.
#include "utils/ConfirmDialogBuilder.h"

#include "LanguageManager.h"
#include "TranslationRegistry.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QStringList>
#include <QTextBrowser>
#include <QWidget>

namespace ConfirmDialogBuilder {

static QString uiText(const QString &english, const QString & /*zh*/) {
  const Language lang = LanguageManager::instance().currentLanguage();
  if (lang == Language::English) {
    return english;
  }
  return TranslationRegistry::instance().translate(english, lang);
}

static bool containsAny(const QString &text,
                         std::initializer_list<const char *> words) {
  const QString lower = text.toLower();
  for (const char *word : words) {
    if (lower.contains(QString::fromUtf8(word).toLower())) {
      return true;
    }
  }
  return false;
}

static bool startsWithAny(const QString &text,
                           std::initializer_list<const char *> words) {
  const QString lower = text.trimmed().toLower();
  for (const char *word : words) {
    if (lower.startsWith(QString::fromUtf8(word).toLower())) {
      return true;
    }
  }
  return false;
}

static QString sectionHtml(const QString &title, const QString &subtitle,
                           const QString &color, const QStringList &items,
                           const QString &theme) {
  if (items.isEmpty()) {
    return QString();
  }
  QString html =
      QString("<section style='border:1px solid %1; border-left:4px solid "
              "%1; border-radius:8px; padding:10px 12px; margin:10px "
              "0;'>")
          .arg(color);
  html += QString("<h3 style='margin:0 0 4px 0; color:%1; font-size:15px;'>"
                  "%2</h3>")
              .arg(color, title.toHtmlEscaped());
  if (!subtitle.trimmed().isEmpty()) {
    html += QString("<p style='margin:0 0 6px 0; color:%1;'>%2</p>")
                .arg(theme == "Light" ? "#475569" : "#b9c6d6",
                     subtitle.toHtmlEscaped());
  }
  html += "<ul style='margin:6px 0 0 18px; padding:0;'>";
  for (const QString &item : items) {
    html += QString("<li style='margin:4px 0;'>%1</li>")
                .arg(item.toHtmlEscaped());
  }
  html += "</ul></section>";
  return html;
}

bool confirm(QWidget *parent, const QString &title, const QString &summary,
             const QStringList &details, const QString &confirmText,
             const QString &theme) {
  QDialog dialog(parent);
  dialog.setObjectName("dangerConfirmDialog");
  dialog.setWindowTitle(title);
  dialog.setModal(true);
  dialog.resize(760, 560);

  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(18, 18, 18, 16);
  layout->setSpacing(12);

  auto *heading = new QLabel(title);
  heading->setObjectName("dialogTitle");
  heading->setWordWrap(true);

  auto *summaryLabel = new QLabel(summary);
  summaryLabel->setObjectName("diagnosticsSummary");
  summaryLabel->setWordWrap(true);
  summaryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

  QStringList criticalItems;
  QStringList reviewItems;
  QStringList evidenceItems;
  QStringList contextItems;
  QStringList otherItems;

  for (const QString &detail : details) {
    const QString trimmed = detail.trimmed();
    if (trimmed.isEmpty()) {
      continue;
    }
    if (startsWithAny(trimmed,
                      {"risk:", "风险：", "critical:", "critical："}) ||
        containsAny(trimmed,
                    {"drive command", "drive/mode", "actuator safety",
                     "persistent storage", "output behavior",
                     "outputs and drive behavior", "op can make outputs",
                     "can affect real hardware", "驱动命令", "驱动/模式",
                     "执行机构安全", "持久化", "输出行为", "op 可能"})) {
      criticalItems << trimmed;
    } else if (containsAny(trimmed, {"validation warning",
                                     "topology baseline",
                                     "consistency "
                                     "gate: not run",
                                     "consistency gate: stale",
                                     "consistency gate: ",
                                     "evidence set conflict",
                                     "write target: differs",
                                     "differs from",
                                     "mismatch",
                                     "missing",
                                     "no live",
                                     "no process",
                                     "no watch",
                                     "no local",
                                     "failed",
                                     "error",
                                     "warning",
                                     "stale",
                                     "校验警告",
                                     "拓扑基线",
                                     "一致性门禁",
                                     "证据集冲突",
                                     "写入目标：不同",
                                     "不同于",
                                     "不一致",
                                     "缺失",
                                     "没有",
                                     "无监视",
                                     "无可比较",
                                     "失败",
                                     "错误",
                                     "警告",
                                     "过期",
                                     "未运行"})) {
      reviewItems << trimmed;
    } else if (containsAny(trimmed, {"evidence",
                                     "watch",
                                     "startup",
                                     "pdo",
                                     "free run",
                                     "object class",
                                     "dictionary",
                                     "write target",
                                     "change preview",
                                     "current evidence",
                                     "current source",
                                     "drive evidence",
                                     "evidence score",
                                     "证据",
                                     "watch",
                                     "startup",
                                     "pdo",
                                     "free run",
                                     "对象类别",
                                     "对象字典",
                                     "写入目标",
                                     "变更预览",
                                     "当前证据",
                                     "当前值",
                                     "驱动证据"})) {
      evidenceItems << trimmed;
    } else if (startsWithAny(trimmed, {"master:",
                                       "slave:",
                                       "row:",
                                       "object:",
                                       "type:",
                                       "value:",
                                       "requested state:",
                                       "current state:",
                                       "detected slaves:",
                                       "current state mix:",
                                       "主站：",
                                       "从站：",
                                       "行：",
                                       "对象：",
                                       "类型：",
                                       "值：",
                                       "目标状态：",
                                       "当前状态：",
                                       "检测到从站：",
                                       "当前状态分布："}) ||
               containsAny(trimmed, {"this operation", "this sends",
                                     "confirm machine safety", "此操作",
                                     "继续前请确认"})) {
      contextItems << trimmed;
    } else {
      otherItems << trimmed;
    }
  }

  const QString severity =
      !criticalItems.isEmpty()
          ? uiText("Critical impact items require explicit review.",
                   "存在关键影响项，需要明确复核。")
          : (!reviewItems.isEmpty()
                 ? uiText("Review warnings before confirming.",
                          "确认前请复核警告项。")
                 : uiText("No high-risk evidence flags were detected, but this "
                          "is still an online operation.",
                          "未发现高风险证据标记，但这仍是在线操作。"));
  auto *severityLabel = new QLabel(severity);
  severityLabel->setObjectName("diagnosticsSummary");
  severityLabel->setWordWrap(true);
  severityLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  severityLabel->setStyleSheet(
      !criticalItems.isEmpty()
          ? QStringLiteral("QLabel { color: #ef4444; font-weight: 700; }")
          : (!reviewItems.isEmpty()
                 ? QStringLiteral("QLabel { color: #f59e0b; font-weight: "
                                  "700; }")
                 : QStringLiteral("QLabel { color: #16a34a; font-weight: "
                                  "700; }")));

  const QString foreground = theme == "Light" ? QStringLiteral("#172033")
                                              : QStringLiteral("#e6edf5");
  const QString muted = theme == "Light" ? QStringLiteral("#475569")
                                         : QStringLiteral("#b9c6d6");
  const QString background = theme == "Light" ? QStringLiteral("#ffffff")
                                              : QStringLiteral("#151b25");
  const QString border = theme == "Light" ? QStringLiteral("#d9e1ec")
                                          : QStringLiteral("#2a3546");

  QString html = QStringLiteral("<!doctype html><html><body>");
  html += QString("<div style='font-family:Inter, Segoe UI, sans-serif; "
                  "font-size:13px; line-height:1.5; color:%1;'>")
              .arg(foreground);
  html += sectionHtml(uiText("Critical Impact", "关键影响"),
                      uiText("Items that can move outputs, drives, persistent "
                             "parameters, or machine behavior.",
                             "可能影响输出、驱动、持久参数或设备行为的项目。"),
                      "#ef4444", criticalItems, theme);
  html += sectionHtml(uiText("Review Before Confirming", "确认前复核"),
                      uiText("Warnings, mismatches, stale gates, missing "
                             "evidence, or topology concerns.",
                             "警告、不一致、过期门禁、缺失证据或拓扑问题。"),
                      "#f59e0b", reviewItems, theme);
  html += sectionHtml(uiText("Evidence", "证据"),
                      uiText("Loaded local or live evidence used to explain "
                             "the requested operation.",
                             "用于解释本次操作的已加载本地或实时证据。"),
                      "#2563eb", evidenceItems, theme);
  html += sectionHtml(uiText("Target Context", "目标上下文"),
                      uiText("Master, slave, object, value, and operation "
                             "scope.",
                             "主站、从站、对象、数值和操作范围。"),
                      "#64748b", contextItems, theme);
  html += sectionHtml(uiText("Other Details", "其他细节"), QString(), "#64748b",
                      otherItems, theme);
  html +=
      QString("<p style='color:%1; margin:10px 0 0 0;'>%2</p>")
          .arg(muted,
               uiText("The confirm button only authorizes the operation; "
                      "the existing validation, runtime request, and "
                      "result logging paths remain unchanged.",
                      "确认按钮只授权本次操作；现有校验、运行时请求和结果记录"
                      "路径保持不变。")
                   .toHtmlEscaped());
  html += "</div></body></html>";

  auto *review = new QTextBrowser;
  review->setObjectName("dangerImpactReview");
  review->setReadOnly(true);
  review->setOpenExternalLinks(false);
  review->setFrameShape(QFrame::NoFrame);
  review->setStyleSheet(QString("QTextBrowser#dangerImpactReview { background: "
                                "%1; border: 1px solid %2; border-radius: 8px; "
                                "padding: 10px; }")
                            .arg(background, border));
  review->setHtml(html);

  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel);
  auto *confirmBtn =
      buttons->addButton(confirmText, QDialogButtonBox::AcceptRole);
  if (auto *cancel = buttons->button(QDialogButtonBox::Cancel)) {
    cancel->setText(uiText("Cancel", "取消"));
    cancel->setDefault(true);
    cancel->setAutoDefault(true);
  }
  confirmBtn->setAutoDefault(false);
  QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog,
                   &QDialog::accept);
  QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog,
                   &QDialog::reject);

  layout->addWidget(heading);
  layout->addWidget(summaryLabel);
  layout->addWidget(severityLabel);
  layout->addWidget(review, 1);
  layout->addWidget(buttons);

  return dialog.exec() == QDialog::Accepted;
}

}  // namespace ConfirmDialogBuilder
