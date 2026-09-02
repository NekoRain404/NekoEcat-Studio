// MainWindowIoVariable.h — I/O variable workspace (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Manages the I/O variable table that maps SDO objects to PLC symbols.
// Handles display, filtering, metadata editing, bulk naming, PLC handoff
// quality checks (duplicate symbol detection, issue review), and export to
// CSV or structured text (ST) PLC declaration blocks. Also bridges
// I/O variable rows into the Watch and Startup SDO workspaces.

// ── I/O Variables Workspace ───────────────────────────────────
void updateIoVariableTable();
void filterIoVariableTable();
void updateIoVariableRowDetail();
QString ioVariableRowKey(int row) const;
QVector<int> selectedIoVariableRows(bool visibleOnly) const;
QVector<int> visibleIoVariableRows() const;
QString ioVariableHandoffIssueLabel(const QString& key) const;
QStringList ioVariableHandoffIssueLabels(const QStringList& keys) const;
QString ioVariablePlcQuality(int row, const QSet<QString>* duplicateSymbols, QString* symbol = nullptr) const;
QSet<QString> duplicateIoVariablePlcSymbols() const;
QVector<int> plcHandoffIssueRows(const QVector<int>& rows) const;
QStringList plcHandoffIssueDetails(const QVector<int>& rows, int previewLimit, int totalRows = -1) const;
QString plcDeclarationBlock(const QVector<int>& rows) const;
void editSelectedIoVariableMetadata();
void bulkNameIoVariables();
void reviewPlcHandoffIssues();
void focusPlcHandoffIssueRows(const QVector<int>& issueRows, bool showReadyMessage);
void copyIoVariablePlcDeclarations(bool selectedOnly);
void clearSelectedIoVariableMetadata();
void exportIoVariablesCsv();
void exportIoVariablesPlcCsv();
void exportIoVariablesPlcDeclarationsSt();
bool confirmPlcHandoffOperation(const QVector<int>& rows, const QString& operation, const QString& continueText);

// ── I/O Variable Actions ──────────────────────────────────────
void addSelectedIoVariablesToWatch();
void addVisibleIoVariablesToWatch();
void addIoVariableRowsToWatch(const QVector<int>& rows, const QString& sourceLabel);
void addSelectedIoVariablesToStartupSdo();
void addVisibleIoVariablesToStartupSdo();
void addIoVariableRowsToStartupSdo(const QVector<int>& rows, const QString& sourceLabel);
