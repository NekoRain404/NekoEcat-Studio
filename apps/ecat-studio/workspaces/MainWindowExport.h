// MainWindowExport.h — Data export methods (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Provides CSV and raw-text export for every major data table in the
// application: PDO maps, SDO dictionaries, SDO history, ESI repositories,
// watchdog entries, startup SDOs, topologies, host health, and raw slave/
// master data. Each method serializes the relevant model data and writes
// it to a user-chosen file.

// ── Data Exports ──────────────────────────────────────────────
void exportPdoMapCsv();
void exportSdoDictionaryCsv();
void exportSdoHistoryCsv();
void exportEsiRepositoryCsv();
void exportEsiXml();
void exportWatchCsv();
void exportStartupSdoCsv();
void exportTopologyCsv();
void exportHostHealthCsv();
void exportPdoRawText();
void exportSdoRawText();
void exportMasterRawText();
void exportSlaveRawText();
