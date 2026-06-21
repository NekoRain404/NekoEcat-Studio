#include "ImpactAnalysisService.h"

#include "EthercatTypes.h"
#include "EvidenceModel.h"
#include "TableHelpers.h"
#include "TextHelpers.h"

#include <QTableWidget>

// ImpactAnalysisService.cpp — Pure data aggregation for impact analysis dialogs
//
// Implementation notes:
//   - Namespace-scoped free functions (not a class) for stateless aggregation
//   - Reads QTableWidget contents to build evidence summaries for Free Run and transitions
//   - Computes an evidence score (0–5) from identity/OD/PDO/Watch/FreeRun data availability
//   - All strings bilingual (EN/ZH) via uiText() lambda

namespace ImpactAnalysisService {

QStringList freeRunImpactDetails(int selectedPosition,
                                 const QVector<SlaveInfo> &slaves,
                                 const QTableWidget *pdoTable,
                                 const QTableWidget *freeRunEntryTable,
                                 const QTableWidget *watchTable,
                                 const QStringList &topologyIssues,
                                 const QStringList &consistencyDetails,
                                 const TextFn &uiText) {
  QStringList details;

  QString slaveName = uiText("unknown slave", "未知从站");
  QString slaveState = uiText("unknown", "未知");
  int opSlaves = 0;
  int safeOpSlaves = 0;
  int preOpSlaves = 0;
  int initSlaves = 0;
  for (const auto &slave : slaves) {
    const QString state = slave.state.trimmed().toUpper();
    if (state == "OP") {
      ++opSlaves;
    } else if (state == "SAFEOP") {
      ++safeOpSlaves;
    } else if (state == "PREOP") {
      ++preOpSlaves;
    } else if (state == "INIT") {
      ++initSlaves;
    }
    if (slave.position == selectedPosition) {
      slaveName =
          slave.name.trimmed().isEmpty() ? slaveName : slave.name.trimmed();
      slaveState =
          slave.state.trimmed().isEmpty() ? slaveState : slave.state.trimmed();
    }
  }

  details << uiText("Selected slave context: #%1 %2, state %3",
                     "选中从站上下文：#%1 %2，状态 %3")
                 .arg(selectedPosition)
                 .arg(slaveName, slaveState);
  details << uiText("Bus state mix: OP %1, SAFEOP %2, PREOP %3, INIT %4",
                     "总线状态分布：OP %1，SAFEOP %2，PREOP %3，INIT %4")
                 .arg(opSlaves)
                 .arg(safeOpSlaves)
                 .arg(preOpSlaves)
                 .arg(initSlaves);

  int pdoRows = 0;
  int rxPdoRows = 0;
  int txPdoRows = 0;
  int rxBits = 0;
  int txBits = 0;
  QStringList rxPreview;
  if (pdoTable) {
    pdoRows = pdoTable->rowCount();
    for (int row = 0; row < pdoTable->rowCount(); ++row) {
      const QString pdo = tableText(const_cast<QTableWidget *>(pdoTable), row, 1);
      const int bits = tableText(const_cast<QTableWidget *>(pdoTable), row, 4).toInt();
      const QString name = tableText(const_cast<QTableWidget *>(pdoTable), row, 5);
      const QString address = QString("%1:%2").arg(
          tableText(const_cast<QTableWidget *>(pdoTable), row, 2),
          tableText(const_cast<QTableWidget *>(pdoTable), row, 3));
      if (pdo.contains("RxPDO", Qt::CaseInsensitive)) {
        ++rxPdoRows;
        rxBits += bits;
        if (rxPreview.size() < 4) {
          rxPreview << QString("%1 %2").arg(address, name);
        }
      } else if (pdo.contains("TxPDO", Qt::CaseInsensitive)) {
        ++txPdoRows;
        txBits += bits;
      }
    }
  }
  details << uiText("PDO map evidence: %1 row(s), RxPDO/output %2 (%3 bit), "
                     "TxPDO/input %4 (%5 bit)",
                     "PDO 映射证据：%1 行，RxPDO/输出 %2（%3 bit），TxPDO/输入 "
                     "%4（%5 bit）")
                 .arg(pdoRows)
                 .arg(rxPdoRows)
                 .arg(rxBits)
                 .arg(txPdoRows)
                 .arg(txBits);
  if (pdoRows <= 0) {
    details << uiText("PDO map warning: no PDO rows are loaded for review",
                      "PDO 映射警告：当前没有可复核的 PDO 行");
  } else if (rxPdoRows > 0) {
    details << uiText("Output risk: Free Run may exchange %1 RxPDO/output "
                      "entry row(s)",
                      "输出风险：Free Run 可能交换 %1 条 RxPDO/输出条目")
                   .arg(rxPdoRows);
    if (!rxPreview.isEmpty()) {
      details << uiText("Output preview: %1", "输出预览：%1")
                     .arg(rxPreview.join(" | "));
    }
  }

  if (freeRunEntryTable && freeRunEntryTable->rowCount() > 0) {
    int outputEntries = 0;
    int inputEntries = 0;
    QStringList meaningPreview;
    for (int row = 0; row < freeRunEntryTable->rowCount(); ++row) {
      const QString direction =
          tableText(const_cast<QTableWidget *>(freeRunEntryTable), row, 2).toLower();
      if (direction.contains("rx") || direction.contains("out")) {
        ++outputEntries;
      } else if (direction.contains("tx") || direction.contains("in")) {
        ++inputEntries;
      }
      const QString meaning =
          tableText(const_cast<QTableWidget *>(freeRunEntryTable), row, 12);
      if (!meaning.isEmpty() && meaningPreview.size() < 3) {
        meaningPreview << meaning;
      }
    }
    details << uiText("Previous Free Run cache: %1 entries, output-like %2, "
                      "input-like %3",
                      "上次 Free Run 缓存：%1 项，输出类 %2，输入类 %3")
                   .arg(freeRunEntryTable->rowCount())
                   .arg(outputEntries)
                   .arg(inputEntries);
    if (!meaningPreview.isEmpty()) {
      details << uiText("Decoded evidence: %1", "解析证据：%1")
                     .arg(meaningPreview.join(" | "));
    }
  }

  QString statusword;
  QString errorCode;
  if (watchTable && selectedPosition >= 0) {
    for (int row = 0; row < watchTable->rowCount(); ++row) {
      const int rowPosition =
          tableText(const_cast<QTableWidget *>(watchTable), row, 1).toInt();
      if (rowPosition != selectedPosition) {
        continue;
      }
      const QString index =
          normalizeHexText(tableText(const_cast<QTableWidget *>(watchTable), row, 2), 4);
      const QString value =
          tableText(const_cast<QTableWidget *>(watchTable), row, 4);
      const QString decoded =
          tableText(const_cast<QTableWidget *>(watchTable), row, 5);
      if (index == "0x6041" && !decoded.isEmpty()) {
        statusword = decoded;
      } else if (index == "0x603f" && !decoded.isEmpty() && !value.isEmpty() &&
                 value != "0" && value.toLower() != "0x0000") {
        errorCode = decoded;
      }
    }
  }
  if (!statusword.isEmpty() || !errorCode.isEmpty()) {
    QStringList driveFacts;
    if (!statusword.isEmpty()) {
      driveFacts << uiText("statusword %1", "状态字 %1").arg(statusword);
    }
    if (!errorCode.isEmpty()) {
      driveFacts << uiText("error %1", "错误 %1").arg(errorCode);
    }
    details << uiText("Drive Watch evidence: %1", "驱动 Watch 证据：%1")
                   .arg(driveFacts.join(" | "));
  } else {
    details << uiText(
        "Drive Watch evidence: no CiA 402 status/error watch rows",
        "驱动 Watch 证据：没有 CiA 402 状态/错误监视行");
  }

  if (!topologyIssues.isEmpty()) {
    details << uiText("Topology baseline: %1 issue(s); review before Free Run",
                      "拓扑基线：%1 个问题；启动 Free Run 前请复核")
                   .arg(topologyIssues.size());
  }
  details << consistencyDetails;
  details << uiText(
      "GUI refresh will switch to 500 ms while Free Run is active.",
      "Free Run 运行时 GUI 刷新周期会切换到 500 ms。");
  return details;
}

QStringList stateTransitionImpactDetails(int position,
                                         const QString &requestedState,
                                         int loadedSlaveInfoPosition,
                                         int loadedSdoPosition,
                                         int loadedPdoPosition,
                                         const QTableWidget *identityTable,
                                         const QTableWidget *sdoTable,
                                         const QTableWidget *pdoTable,
                                         const QTableWidget *watchTable,
                                         const QTableWidget *startupSdoTable,
                                         const QTableWidget *freeRunEntryTable,
                                         const QStringList &topologyIssues,
                                         const QStringList &consistencyDetails,
                                         const TextFn &uiText) {
  QStringList details;

  const QString target = requestedState.trimmed().toUpper();
  const bool identityLoaded = loadedSlaveInfoPosition == position;
  const bool odLoaded = loadedSdoPosition == position;
  const bool pdoLoaded = loadedPdoPosition == position;
  const int identityRows =
      identityLoaded && identityTable ? identityTable->rowCount() : 0;
  const int odRows = odLoaded && sdoTable ? sdoTable->rowCount() : 0;
  const int pdoRows = pdoLoaded && pdoTable ? pdoTable->rowCount() : 0;

  int watchRows = 0;
  int watchValueRows = 0;
  QString statusword;
  QString modeDisplay;
  QString errorCode;
  if (watchTable) {
    for (int row = 0; row < watchTable->rowCount(); ++row) {
      if (tableText(const_cast<QTableWidget *>(watchTable), row, 1).toInt() !=
          position) {
        continue;
      }
      ++watchRows;
      const QString value =
          tableText(const_cast<QTableWidget *>(watchTable), row, 4);
      const QString decoded =
          tableText(const_cast<QTableWidget *>(watchTable), row, 5);
      if (!value.isEmpty()) {
        ++watchValueRows;
      }
      const QString index = normalizeHexText(
          tableText(const_cast<QTableWidget *>(watchTable), row, 2), 4);
      if (index == "0x6041" && !decoded.isEmpty()) {
        statusword = decoded;
      } else if (index == "0x6061" && !decoded.isEmpty()) {
        modeDisplay = decoded;
      } else if (index == "0x603f" && !decoded.isEmpty() && value != "0" &&
                 value.toLower() != "0x0000") {
        errorCode = decoded;
      }
    }
  }

  int startupRows = 0;
  int startupDiffs = 0;
  if (startupSdoTable) {
    for (int row = 0; row < startupSdoTable->rowCount(); ++row) {
      if (tableText(const_cast<QTableWidget *>(startupSdoTable), row, 0)
              .toInt() != position) {
        continue;
      }
      ++startupRows;
      if (hasStartupDiffEvidence(
              tableText(const_cast<QTableWidget *>(startupSdoTable), row, 8))) {
        ++startupDiffs;
      }
    }
  }

  int freeRunRows = 0;
  int mapIssues = 0;
  if (freeRunEntryTable) {
    for (int row = 0; row < freeRunEntryTable->rowCount(); ++row) {
      if (tableText(const_cast<QTableWidget *>(freeRunEntryTable), row, 0)
              .toInt() != position) {
        continue;
      }
      ++freeRunRows;
      if (hasPdoMapIssueEvidence(
              tableText(const_cast<QTableWidget *>(freeRunEntryTable), row, 13))) {
        ++mapIssues;
      }
    }
  }

  int evidenceScore = 0;
  evidenceScore += identityRows > 0 ? 1 : 0;
  evidenceScore += odRows > 0 ? 1 : 0;
  evidenceScore += pdoRows > 0 ? 1 : 0;
  evidenceScore += watchValueRows > 0 ? 1 : 0;
  evidenceScore += freeRunRows > 0 ? 1 : 0;

  details << uiText("Evidence score: %1/5 (ID %2, OD %3, PDO %4, Watch "
                     "%5/%6 values, Free Run %7)",
                     "证据完整度：%1/5（身份 %2，OD %3，PDO %4，Watch %5/%6 "
                     "有值，Free Run %7）")
                 .arg(evidenceScore)
                 .arg(identityRows > 0 ? uiText("ready", "就绪")
                                       : uiText("missing", "缺失"))
                 .arg(odRows)
                 .arg(pdoRows)
                 .arg(watchValueRows)
                 .arg(watchRows)
                 .arg(freeRunRows);

  if (!statusword.isEmpty() || !modeDisplay.isEmpty() || !errorCode.isEmpty()) {
    QStringList driveFacts;
    if (!statusword.isEmpty()) {
      driveFacts << uiText("statusword %1", "状态字 %1").arg(statusword);
    }
    if (!modeDisplay.isEmpty()) {
      driveFacts << uiText("mode %1", "模式 %1").arg(modeDisplay);
    }
    if (!errorCode.isEmpty()) {
      driveFacts << uiText("error %1", "错误 %1").arg(errorCode);
    }
    details << uiText("Drive evidence: %1", "驱动证据：%1")
                   .arg(driveFacts.join(" | "));
  } else {
    details << uiText("Drive evidence: no CiA 402 Watch values",
                      "驱动证据：没有 CiA 402 Watch 值");
  }

  if (startupRows > 0 || startupDiffs > 0) {
    details << uiText("Startup evidence: %1 row(s), %2 Watch mismatch(es)",
                      "Startup 证据：%1 行，%2 条 Watch 不一致")
                   .arg(startupRows)
                   .arg(startupDiffs);
  }
  if (mapIssues > 0) {
    details << uiText("PDO map evidence: %1 Free Run map issue(s)",
                      "PDO 映射证据：Free Run 有 %1 个映射问题")
                   .arg(mapIssues);
  }

  if (!topologyIssues.isEmpty()) {
    details << uiText("Topology baseline: %1 issue(s); review before state "
                      "transition",
                      "拓扑基线：%1 个问题；切换状态前请复核")
                   .arg(topologyIssues.size());
  }
  details << consistencyDetails;

  if (target == "OP" || target == "SAFEOP") {
    if (pdoRows <= 0) {
      details << uiText("Risk: PDO Map is not loaded for this slave",
                        "风险：当前从站尚未加载 PDO 映射");
    }
    if (watchValueRows <= 0) {
      details << uiText("Risk: no live Watch values for this slave",
                        "风险：当前从站没有实时 Watch 值");
    }
    if (target == "OP" && freeRunRows <= 0) {
      details << uiText("Risk: no process-image evidence before OP",
                        "风险：进入 OP 前没有过程映像证据");
    }
    if (startupDiffs > 0) {
      details << uiText("Risk: Startup SDO expectations differ from Watch",
                        "风险：Startup SDO 期望值和 Watch 不一致");
    }
  }

  return details;
}

} // namespace ImpactAnalysisService
