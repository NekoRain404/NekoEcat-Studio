# MainWindow Slim-Down Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Reduce MainWindow.h from 809 lines to <300 lines and MainWindow.cpp from 3376 lines to <1000 lines by extracting workspace-specific state and methods into helper classes and partial files.

**Architecture:** MainWindow remains the central QMainWindow subclass but becomes a thin coordinator. Workspace-specific widget pointers, tab indices, and page widgets are replaced with a `WorkspaceRegistry` that maps plugin IDs to their state. Heavy method implementations (`wire()`, `updateActionAvailability()`, `confirmDangerousOperation()`, etc.) are moved to new partial-class `.cpp` files following the existing pattern in `workspaces/`.

**Tech Stack:** C++17, Qt6, CMake. Existing partial-class pattern in `workspaces/`.

---

## File Structure

### New files to create:
- `workspaces/MainWindowWorkspaceRegistry.h` — WorkspaceState struct + WorkspaceRegistry class
- `workspaces/MainWindowWireSignals.cpp` — `wire()` implementation (~400 lines)
- `workspaces/MainWindowActionAvailability.cpp` — `updateActionAvailability()` (~230 lines)
- `workspaces/MainWindowConfirmation.cpp` — `confirmDangerousOperation()`, `stateTransitionImpactDetails()`, `freeRunImpactDetails()` (~740 lines)
- `workspaces/MainWindowDaemon.cpp` — `startEmbeddedDaemon()`, `requestRefresh()`, daemon management (~50 lines)

### Files to modify:
- `MainWindow.h` — slim down from 809 to <300 lines
- `MainWindow.cpp` — slim down from 3376 to <1000 lines
- `workspaces/MainWindowUiBuild.cpp` — update to use WorkspaceRegistry
- `workspaces/WorkspaceWidgets.h` — no changes needed (structs stay as-is)
- `CMakeLists.txt` — add new .cpp files

---

## Analysis: What Moves Where

### MainWindow.h members to remove:

| Category | Current Lines | Action |
|----------|--------------|--------|
| 16 plugin pointers (`overviewPlugin_`, etc.) | ~20 lines | Remove; use `pluginRegistry_.findById()` on demand |
| 18 tab index members (`overviewTabIndex_`, etc.) | ~20 lines | Move to `WorkspaceRegistry` |
| 18 page widget members (`overviewPage_`, etc.) | ~20 lines | Move to `WorkspaceRegistry` |
| Forward declarations for plugins | ~16 lines | Remove (only needed in .cpp files) |
| Forward declarations for text structs | ~25 lines | Keep (needed by method signatures) |
| ~200 method declarations | ~530 lines | Keep ~25 core methods; move rest to partial files as free-standing `MainWindow::` methods (they still need header declaration) |

### MainWindow.cpp methods to extract:

| Method | Lines | Target File |
|--------|-------|-------------|
| `wire()` | ~400 | `workspaces/MainWindowWireSignals.cpp` |
| `updateActionAvailability()` | ~230 | `workspaces/MainWindowActionAvailability.cpp` |
| `confirmDangerousOperation()` | ~290 | `workspaces/MainWindowConfirmation.cpp` |
| `stateTransitionImpactDetails()` | ~170 | `workspaces/MainWindowConfirmation.cpp` |
| `freeRunImpactDetails()` | ~180 | `workspaces/MainWindowConfirmation.cpp` |
| `setFreeRun()` | ~50 | `workspaces/MainWindowConfirmation.cpp` |
| `updateFreeRunEntryDetail()` | ~80 | `workspaces/MainWindowConfirmation.cpp` |
| `startEmbeddedDaemon()` | ~30 | `workspaces/MainWindowDaemon.cpp` |
| `requestRefresh()` | ~15 | `workspaces/MainWindowDaemon.cpp` |
| `addToRecentProjects()` | ~40 | `workspaces/MainWindowProjectIo.cpp` (already exists) |
| `updateRecentProjectsMenu()` | ~30 | `workspaces/MainWindowProjectIo.cpp` (already exists) |
| `applyCustomShortcuts()` | ~45 | `workspaces/MainWindowCommandPalette.cpp` (already exists) |
| `loadSettings()` | ~80 | keep (small enough) |
| `saveSettings()` | ~65 | keep (small enough) |
| `openSettings()` | ~40 | keep (small enough) |
| `clearOnlineViews()` | ~75 | keep (small enough) |

---

## Task 1: Create WorkspaceRegistry

**Files:**
- Create: `apps/ecat-studio/workspaces/MainWindowWorkspaceRegistry.h`

- [ ] **Step 1: Create WorkspaceRegistry header**

```cpp
#pragma once

#include <QHash>
#include <QString>
#include <QVector>
#include <QWidget>

struct WorkspaceState {
    QWidget *page = nullptr;
    int tabIndex = -1;
};

class WorkspaceRegistry {
public:
    void registerWorkspace(const QString &pluginId, QWidget *page);
    void setTabIndex(const QString &pluginId, int index);
    int tabIndex(const QString &pluginId) const;
    QWidget *page(const QString &pluginId) const;
    QVector<QPair<int, QWidget *>> allPages() const;
    void clear();

private:
    QHash<QString, WorkspaceState> entries_;
};
```

- [ ] **Step 2: Create WorkspaceRegistry implementation**

Create `apps/ecat-studio/workspaces/MainWindowWorkspaceRegistry.cpp`:

```cpp
#include "MainWindowWorkspaceRegistry.h"

void WorkspaceRegistry::registerWorkspace(const QString &pluginId, QWidget *page) {
    entries_[pluginId].page = page;
}

void WorkspaceRegistry::setTabIndex(const QString &pluginId, int index) {
    entries_[pluginId].tabIndex = index;
}

int WorkspaceRegistry::tabIndex(const QString &pluginId) const {
    auto it = entries_.constFind(pluginId);
    return it != entries_.constEnd() ? it->tabIndex : -1;
}

QWidget *WorkspaceRegistry::page(const QString &pluginId) const {
    auto it = entries_.constFind(pluginId);
    return it != entries_.constEnd() ? it->page : nullptr;
}

QVector<QPair<int, QWidget *>> WorkspaceRegistry::allPages() const {
    QVector<QPair<int, QWidget *>> result;
    result.reserve(entries_.size());
    for (auto it = entries_.constBegin(); it != entries_.constEnd(); ++it) {
        if (it->page) {
            result.append({it->tabIndex, it->page});
        }
    }
    return result;
}

void WorkspaceRegistry::clear() {
    entries_.clear();
}
```

- [ ] **Step 3: Add to CMakeLists.txt**

Add the new files to the `ecat-studio` target in `apps/ecat-studio/CMakeLists.txt`.

- [ ] **Step 4: Build to verify compilation**

Run: `cmake --build build -j4`

---

## Task 2: Slim Down MainWindow.h

**Files:**
- Modify: `apps/ecat-studio/MainWindow.h`

- [ ] **Step 1: Replace individual page/tab members with WorkspaceRegistry**

Remove all 18 `xxxPage_` members and 18 `xxxTabIndex_` members. Add:
```cpp
WorkspaceRegistry workspaceRegistry_;
```

Replace all references to `overviewPage_`, `objectDictionaryPage_`, etc. with `workspaceRegistry_.page("overview")`, etc. Replace all references to `overviewTabIndex_`, etc. with `workspaceRegistry_.tabIndex("overview")`, etc.

- [ ] **Step 2: Remove plugin pointer members**

Remove all 16 individual plugin pointer members (`overviewPlugin_`, `odPlugin_`, etc.) and the corresponding forward declarations. Access plugins via `pluginRegistry_.findById()` with `qobject_cast` when needed.

- [ ] **Step 3: Keep only core method declarations**

The header should retain only these method categories:
1. Constructor/destructor/eventFilter (core lifecycle)
2. `buildUi()`, `rebuildUi()`, `applyTheme()`, `applySettings()` (UI lifecycle)
3. `loadSettings()`, `saveSettings()`, `openSettings()` (settings)
4. `wire()`, `startEmbeddedDaemon()`, `requestRefresh()` (wiring/daemon)
5. `updateActionAvailability()`, `updateStatusBar()` (global state)
6. `uiText()`, `log()`, `selectedPosition()` (utilities)
7. `clearOnlineViews()` (data lifecycle)
8. Workspace navigation: `activateWorkspaceTab()`, `activateWorkspacePage()`, `recordWorkspaceHistory()`, `goWorkspaceBack()`, `goWorkspaceForward()`

All other method declarations should remain in the header (they're needed for the partial-class pattern). The reduction comes from removing members, not methods.

- [ ] **Step 4: Verify the header is under 300 lines**

Run: `wc -l apps/ecat-studio/MainWindow.h`

- [ ] **Step 5: Build to verify compilation**

Run: `cmake --build build -j4`

---

## Task 3: Extract wire() to Partial File

**Files:**
- Create: `apps/ecat-studio/workspaces/MainWindowWireSignals.cpp`
- Modify: `apps/ecat-studio/MainWindow.cpp` (remove wire() body)

- [ ] **Step 1: Move wire() to new file**

Move the entire `MainWindow::wire()` method (lines 1636-3001 in MainWindow.cpp) to `workspaces/MainWindowWireSignals.cpp`. The file starts with:
```cpp
#include "MainWindowIncludes.h"

void MainWindow::wire() {
  // ... existing body ...
}
```

- [ ] **Step 2: Remove wire() from MainWindow.cpp**

Delete the wire() implementation from MainWindow.cpp.

- [ ] **Step 3: Add to CMakeLists.txt**

Add `workspaces/MainWindowWireSignals.cpp` to the ecat-studio target.

- [ ] **Step 4: Build to verify**

Run: `cmake --build build -j4`

---

## Task 4: Extract updateActionAvailability() to Partial File

**Files:**
- Create: `apps/ecat-studio/workspaces/MainWindowActionAvailability.cpp`
- Modify: `apps/ecat-studio/MainWindow.cpp` (remove updateActionAvailability() body)

- [ ] **Step 1: Move updateActionAvailability() to new file**

Move the entire `MainWindow::updateActionAvailability()` method (lines 492-724 in MainWindow.cpp) to `workspaces/MainWindowActionAvailability.cpp`. The file starts with:
```cpp
#include "MainWindowIncludes.h"

void MainWindow::updateActionAvailability() {
  // ... existing body ...
}
```

- [ ] **Step 2: Remove from MainWindow.cpp**

Delete the updateActionAvailability() implementation from MainWindow.cpp.

- [ ] **Step 3: Add to CMakeLists.txt**

Add `workspaces/MainWindowActionAvailability.cpp` to the ecat-studio target.

- [ ] **Step 4: Build to verify**

Run: `cmake --build build -j4`

---

## Task 5: Extract Confirmation/Impact Methods to Partial File

**Files:**
- Create: `apps/ecat-studio/workspaces/MainWindowConfirmation.cpp`
- Modify: `apps/ecat-studio/MainWindow.cpp` (remove method bodies)

- [ ] **Step 1: Move methods to new file**

Move these methods from MainWindow.cpp to `workspaces/MainWindowConfirmation.cpp`:
- `confirmDangerousOperation()` (~290 lines)
- `stateTransitionImpactDetails()` (~170 lines)
- `freeRunImpactDetails()` (~180 lines)
- `setFreeRun()` (~50 lines)
- `updateFreeRunEntryDetail()` (~80 lines)

The file starts with:
```cpp
#include "MainWindowIncludes.h"

// ... all moved method implementations ...
```

- [ ] **Step 2: Remove from MainWindow.cpp**

Delete all moved method implementations from MainWindow.cpp.

- [ ] **Step 3: Add to CMakeLists.txt**

Add `workspaces/MainWindowConfirmation.cpp` to the ecat-studio target.

- [ ] **Step 4: Build to verify**

Run: `cmake --build build -j4`

---

## Task 6: Extract Daemon/Refresh Methods to Partial File

**Files:**
- Create: `apps/ecat-studio/workspaces/MainWindowDaemon.cpp`
- Modify: `apps/ecat-studio/MainWindow.cpp` (remove method bodies)

- [ ] **Step 1: Move methods to new file**

Move these methods from MainWindow.cpp to `workspaces/MainWindowDaemon.cpp`:
- `startEmbeddedDaemon()` (~30 lines)
- `requestRefresh()` (~15 lines)

The file starts with:
```cpp
#include "MainWindowIncludes.h"

// ... moved method implementations ...
```

- [ ] **Step 2: Remove from MainWindow.cpp**

Delete moved implementations from MainWindow.cpp.

- [ ] **Step 3: Add to CMakeLists.txt**

Add `workspaces/MainWindowDaemon.cpp` to the ecat-studio target.

- [ ] **Step 4: Build to verify**

Run: `cmake --build build -j4`

---

## Task 7: Move Recent Projects and Shortcuts to Existing Partial Files

**Files:**
- Modify: `apps/ecat-studio/workspaces/MainWindowProjectIo.cpp` (add recent project methods)
- Modify: `apps/ecat-studio/workspaces/MainWindowCommandPalette.cpp` (add shortcut methods)
- Modify: `apps/ecat-studio/MainWindow.cpp` (remove method bodies)

- [ ] **Step 1: Move recent project methods to MainWindowProjectIo.cpp**

Move from MainWindow.cpp to `workspaces/MainWindowProjectIo.cpp`:
- `addToRecentProjects()` (~40 lines)
- `updateRecentProjectsMenu()` (~30 lines)

- [ ] **Step 2: Move shortcut method to MainWindowCommandPalette.cpp**

Move from MainWindow.cpp to `workspaces/MainWindowCommandPalette.cpp`:
- `applyCustomShortcuts()` (~45 lines)

- [ ] **Step 3: Remove from MainWindow.cpp**

Delete moved implementations from MainWindow.cpp.

- [ ] **Step 4: Build to verify**

Run: `cmake --build build -j4`

---

## Task 8: Update MainWindow.cpp References to Removed Members

**Files:**
- Modify: `apps/ecat-studio/MainWindow.cpp`
- Modify: `apps/ecat-studio/workspaces/*.cpp` (all partial files)

- [ ] **Step 1: Replace plugin pointer references**

In MainWindow.cpp constructor, replace the 16 `qobject_cast` lines with on-demand lookups. Add a helper method:
```cpp
template<typename T>
T *findPlugin(const char *id) const {
    return qobject_cast<T *>(pluginRegistry_.findById(id));
}
```

Update all workspace files that reference `overviewPlugin_`, `odPlugin_`, etc. to use `findPlugin<T>("id")` or `pluginRegistry_.findById("id")`.

- [ ] **Step 2: Replace page/tab member references**

Update all workspace files that reference `overviewPage_`, `overviewTabIndex_`, etc. to use `workspaceRegistry_.page("overview")`, `workspaceRegistry_.tabIndex("overview")`, etc.

- [ ] **Step 3: Build to verify**

Run: `cmake --build build -j4`

---

## Task 9: Verify Line Count Targets

- [ ] **Step 1: Check MainWindow.h line count**

Run: `wc -l apps/ecat-studio/MainWindow.h`
Expected: < 300 lines

- [ ] **Step 2: Check MainWindow.cpp line count**

Run: `wc -l apps/ecat-studio/MainWindow.cpp`
Expected: < 1000 lines

- [ ] **Step 3: Run full build and tests**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4`

---

## Task 10: Final Cleanup

- [ ] **Step 1: Remove unused forward declarations from MainWindow.h**

Check which forward declarations are no longer needed after removing plugin pointers.

- [ ] **Step 2: Verify no compilation warnings**

Run: `cmake --build build -j4 2>&1 | grep -i warning`

- [ ] **Step 3: Run full test suite**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4`

---

## Expected Final Line Counts

| File | Before | After (est.) | Target |
|------|--------|--------------|--------|
| MainWindow.h | 809 | ~270 | < 300 |
| MainWindow.cpp | 3376 | ~800 | < 1000 |
| MainWindowWireSignals.cpp | 0 | ~400 | - |
| MainWindowActionAvailability.cpp | 0 | ~230 | - |
| MainWindowConfirmation.cpp | 0 | ~770 | - |
| MainWindowDaemon.cpp | 0 | ~50 | - |
| MainWindowWorkspaceRegistry.h | 0 | ~30 | - |
| MainWindowWorkspaceRegistry.cpp | 0 | ~35 | - |

## Risk Mitigation

1. **Circular includes**: All new partial files use `MainWindowIncludes.h` which already includes everything. No new include cycles.
2. **Method visibility**: All moved methods remain `MainWindow::` private methods declared in the header. No API changes.
3. **Signal connections**: `wire()` moves as-is to the partial file. No behavioral changes.
4. **Plugin access**: Removing cached plugin pointers means one hash lookup per access, which is negligible for UI operations.
