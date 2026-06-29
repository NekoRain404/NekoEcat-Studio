// MainWindowSdo.h — Service Data Object (SDO) workspace (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Core of the object dictionary workspace. Covers SDO read/write operations,
// the SDO inspector panel (target row, evidence candidates, write-delta
// review, CIA 402 controlword helpers), SDO history tracking, target trail
// management, table filtering and evidence updates. Also handles SDO
// selection requests originating from every other workspace (Watch, PDO map,
// Free Run, I/O variables, bookmarks, startup SDOs, history) and dispatches
// SDO value responses back to the watch table, charts, and startup SDO
// verification.

// ── SDO Impact Preview ────────────────────────────────────────
QStringList sdoWriteImpactDetails(int position, const QString &index,
                                  const QString &subIndex,
                                  const QString &targetValue,
                                  const QString &type) const;
QString startupSdoImpactLine(int row) const;
QStringList startupSdoBatchImpactDetails(const QVector<int> &rows,
                                         int previewLimit) const;

// ── SDO Inspector & Target Panel ──────────────────────────────
void restoreManualSdoWriteMode();
bool isCurrentSdoTarget(int position, const QString &index,
                        const QString &subIndex) const;
QString sdoObjectCategory(const QString &index, const QString &name,
                          const QString &object, const QString &detail) const;
void updateSdoTargetPanel(const QString &source, const QString &detail,
                          const QString &status, const QStringList &problems);
void updateSdoTargetRowActionButton();
void updateSdoTargetRowCopyButton();
void updateSdoInspector(const QString &source = QString(),
                        const QString &detail = QString());
void ensureSdoTargetTrailTable();
void updateSdoTargetTrailRowDetail();
void rememberCurrentSdoTarget(const QString &source, const QString &detail);
bool prepareSdoTargetTrailRow(int row, bool reportRestoreSuccess);
QString sdoTargetTrailRowStartupValue(int row) const;
bool sdoTargetTrailRowCanCreateStartup(int row) const;
void restoreSdoTargetTrailRow(int row);
void addSdoTargetTrailRowToWatch();
void bookmarkSdoTargetTrailRow();
void addSdoTargetTrailRowToStartup();
void removeSelectedSdoTargetTrailRows();
void clearSdoTargetTrail();
int currentSdoDictionaryRow() const;
int currentSdoWatchRow() const;
int currentSdoStartupRow() const;
int currentSdoBookmarkRow() const;
int currentSdoTargetTrailRow() const;
QString currentSdoPreferredEvidenceValue(QString *source = nullptr) const;
SdoEvidenceCandidates currentSdoEvidenceCandidates() const;
bool currentSdoEvidenceHasConflict() const;
bool currentSdoWriteDeltaReviewAvailable() const;
void reviewCurrentSdoWriteDelta();
void copyCurrentSdoEvidenceDigest();
bool copySdoTargetPanelRowDigest(int row);
bool openSdoTargetPanelRow(int row);
void openCurrentSdoWatchLink();
void openCurrentSdoStartupLink();
void openCurrentSdoBookmarkLink();
void openCurrentSdoTargetTrailLink();

// ── SDO Evidence Updates ──────────────────────────────────────
void updateSdoTableEvidence(int position, const QString &index,
                            const QString &subIndex, const QString &value,
                            const QString &status, const QString &detail);
void useReadSdoValueForWrite();
void usePreferredSdoEvidenceForWrite();
void pickSdoEvidenceForWrite();
void writeCurrentSdo();
void prepareCia402Controlword(const QString &label, const QString &value);
bool recommendedCia402Controlword(QString *label, QString *value,
                                  QString *reason) const;
bool validateSdoAddressAndValue(const QString &index, const QString &subIndex,
                                const QString &value, const QString &type,
                                QStringList *errors,
                                QStringList *warnings) const;

// ── SDO History ───────────────────────────────────────────────
void ensureSdoHistoryTable();
void appendSdoHistory(const QString &action, int position,
                      const QString &index, const QString &subIndex,
                      const QString &type, const QString &value,
                      const QString &status, const QString &detail);
void updateSdoHistoryRowDetail();

// ── SDO Selection from Other Workspaces ───────────────────────
void requestSdoRead(int position, const QString &index,
                    const QString &subIndex, const QString &source,
                    const QString &type = QString());
void applySdoSelectionFromDictionary(int row, bool readAfterFill);
void applySdoSelectionFromPdoMap(int row, bool readAfterFill);
void applySdoSelectionFromFreeRunEntry(int row, bool readAfterFill);
void applySdoSelectionFromIoVariable(int row, bool readAfterFill);
void applySdoSelectionFromWatch(int row, bool readAfterFill);
void applySdoSelectionFromHistory(int row, bool readAfterFill);
void applySdoSelectionFromStartup(int row, bool readAfterFill);

// ── SDO Table Filtering ───────────────────────────────────────
void setSdoFilterPreset(const QString &query);
bool hasFailedSdoEvidence() const;
int firstFailedSdoEvidenceRow() const;
void focusFailedSdoEvidence();
void filterSdoTable(const QString &text);

// ── SDO Value Response Handler ───────────────────────────────
void handleSdoValueResponse(int position, const QString &index,
                            const QString &subIndex, const QString &value);
void updateWatchTableFromSdo(int position, const QString &index,
                             const QString &subIndex, const QString &value,
                             const QString &source, const QString &readType,
                             bool currentTarget, const QString &key);
void feedChartFromSdo(const QString &index, const QString &subIndex,
                      const QString &value);
void verifyStartupSdo(const QString &key, const QString &value,
                      const QVector<int> &startupCheckRows);

// ── Topology & Data Updates (SDO part) ────────────────────────
void updateSdoTable(const QString &text);
