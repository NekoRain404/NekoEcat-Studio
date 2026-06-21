#pragma once

// ImpactAnalysisService — pure data aggregation for impact analysis dialogs.
// Extracted from MainWindow to reduce its size. No UI mutation.
//
// This service provides impact analysis capabilities for the NekoEcat
// Studio application. It handles:
//   - Free Run impact analysis
//   - State transition impact analysis
//   - Data aggregation from multiple sources
//   - Impact detail generation
//
// Usage:
//   QStringList details = ImpactAnalysisService::freeRunImpactDetails(
//     selectedPosition, slaves, pdoTable, freeRunEntryTable,
//     watchTable, topologyIssues, consistencyDetails, uiText);
//   QStringList stateDetails = ImpactAnalysisService::stateTransitionImpactDetails(
//     position, requestedState, loadedSlaveInfoPosition, loadedSdoPosition,
//     loadedPdoPosition, identityTable, sdoTable, pdoTable, watchTable,
//     startupSdoTable, freeRunEntryTable, topologyIssues, consistencyDetails, uiText);
//
// Thread safety:
//   All functions must be called from the main (GUI) thread. Functions
//   are pure data aggregators with no side effects.
//
// Performance:
//   - Impact analysis is O(n) where n is number of table rows
//   - Data aggregation is O(1) for each source
//   - String generation is O(n) where n is number of details

#include <QStringList>
#include <functional>

class QTableWidget;
struct SlaveInfo;

// Text function type for bilingual support.
using TextFn = std::function<QString(const char *, const char *)>;

namespace ImpactAnalysisService {

// Analyze the impact of Free Run operations.
// @param selectedPosition     Selected slave position
// @param slaves               Current slave list
// @param pdoTable             PDO mapping table
// @param freeRunEntryTable    Free Run entry table
// @param watchTable           Watch table
// @param topologyIssues       Topology issues list
// @param consistencyDetails   Consistency details list
// @param uiText               Text function for bilingual support
// @return QStringList of impact details
QStringList freeRunImpactDetails(int selectedPosition,
                                 const QVector<SlaveInfo> &slaves,
                                 const QTableWidget *pdoTable,
                                 const QTableWidget *freeRunEntryTable,
                                 const QTableWidget *watchTable,
                                 const QStringList &topologyIssues,
                                 const QStringList &consistencyDetails,
                                 const TextFn &uiText);

// Analyze the impact of state transitions.
// @param position                    Slave position
// @param requestedState              Requested state
// @param loadedSlaveInfoPosition     Loaded slave info position
// @param loadedSdoPosition           Loaded SDO position
// @param loadedPdoPosition           Loaded PDO position
// @param identityTable               Identity table
// @param sdoTable                    SDO table
// @param pdoTable                    PDO table
// @param watchTable                  Watch table
// @param startupSdoTable             Startup SDO table
// @param freeRunEntryTable           Free Run entry table
// @param topologyIssues              Topology issues list
// @param consistencyDetails          Consistency details list
// @param uiText                      Text function for bilingual support
// @return QStringList of impact details
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
                                         const TextFn &uiText);

} // namespace ImpactAnalysisService
