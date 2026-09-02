// MainWindowProject.h — Project management and ESI repository (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Manages project lifecycle: create, open, save, and save-as operations for
// .ecat project files. Also handles importing EtherCAT Slave Information
// (ESI) XML files into the local ESI repository and maintaining the recent-
// projects menu for quick access.

// ── Project Management ────────────────────────────────────────
void newProject();
void openProject();
void saveProject();
void saveProjectAs();
bool writeProjectFile(const QString& path);
bool readProjectFile(const QString& path);

// ── ESI Repository ────────────────────────────────────────────
void importEsiFiles();
void refreshEsiRepository();

// ── Recent Projects ───────────────────────────────────────────
void updateRecentProjectsMenu();
void addToRecentProjects(const QString& path);
