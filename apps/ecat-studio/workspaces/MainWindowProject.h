// MainWindowProject.h — Project management & ESI repository (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.

// ── Project Management ────────────────────────────────────────
void newProject();
void openProject();
void saveProject();
void saveProjectAs();
bool writeProjectFile(const QString &path);
bool readProjectFile(const QString &path);

// ── ESI Repository ────────────────────────────────────────────
void importEsiFiles();
void refreshEsiRepository();

// ── Recent Projects ───────────────────────────────────────────
void updateRecentProjectsMenu();
void addToRecentProjects(const QString &path);
