# Phase 4: Quality, Architecture, Features — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development to implement this plan task-by-task.

**Goal:** Bring NekoEcat Studio to production quality with comprehensive comments, clean architecture, and expanded test coverage.

**Architecture:** Four-week plan. Week 1 (quality), Week 2 (architecture), Week 3 (features/tests), Week 4 (polish). Each task is self-contained and produces working, testable software.

**Tech Stack:** C++20, Qt 6 (Core, Network, Widgets), IgH ecrt, CMake

---

## Week 1: Code Quality

### Task 1: Remove duplicate includes

**Files:** 15 workspace .cpp files in `apps/ecat-studio/workspaces/`

- [ ] Remove duplicate `#include "MainWindow.h"` from all 15 workspace files (keep first occurrence)
- [ ] Remove duplicate `#include <QFont>` from MainWindowRtTestWorkspace.cpp
- [ ] Build and verify no regressions
- [ ] Commit: "chore: remove duplicate includes from 15 workspace files"

### Task 2: Extract freeRunTable_ to FreeRunWorkspaceWidgets

**Files:** Modify `apps/ecat-studio/workspaces/WorkspaceWidgets.h`, `apps/ecat-studio/MainWindow.h`, `apps/ecat-studio/workspaces/MainWindowUiBuild.cpp`, `apps/ecat-studio/MainWindow.cpp`, `apps/ecat-studio/workspaces/MainWindowDiagnosticsTopology.cpp`

- [ ] Add `QTableWidget *freeRunTable = nullptr;` to `FreeRunWorkspaceWidgets` struct
- [ ] Replace all `freeRunTable_` references with `freeRunWidgets_->freeRunTable`
- [ ] Remove `QTableWidget *freeRunTable_ = nullptr;` from MainWindow.h
- [ ] Build and run tests — all must pass
- [ ] Commit: "refactor: extract freeRunTable_ to FreeRunWorkspaceWidgets"

### Task 3: Add comments to MainWindowUiBuild.cpp

**Files:** `apps/ecat-studio/workspaces/MainWindowUiBuild.cpp`

- [ ] Add method-level docstrings to buildUi() and rebuildUi()
- [ ] Add section comments for each workspace page construction block
- [ ] Target: 8% comment density (209 comments / 2612 lines)
- [ ] Build and verify
- [ ] Commit: "docs: add comprehensive comments to MainWindowUiBuild.cpp"

### Task 4: Add comments to MainWindowCommandPalette.cpp

**Files:** `apps/ecat-studio/workspaces/MainWindowCommandPalette.cpp`

- [ ] Add method-level docstrings to showCommandPalette()
- [ ] Add inline comments for the command registry, filter logic, and keyboard shortcuts
- [ ] Target: 8% comment density (157 comments / 1961 lines)
- [ ] Build and verify
- [ ] Commit: "docs: add comprehensive comments to MainWindowCommandPalette.cpp"

### Task 5: Add comments to MainWindowConsistencyCommissioning.cpp

**Files:** `apps/ecat-studio/workspaces/MainWindowConsistencyCommissioning.cpp`

- [ ] Add method-level docstrings to all public methods
- [ ] Add inline comments for gate logic and evidence routing
- [ ] Target: 8% comment density (141 comments / 1766 lines)
- [ ] Build and verify
- [ ] Commit: "docs: add comprehensive comments to MainWindowConsistencyCommissioning.cpp"

## Week 2: Architecture

### Task 6: Extract SdoTargetPanel from MainWindowSdoWorkspace.cpp

**Files:** Create `apps/ecat-studio/workspaces/MainWindowSdoTargetPanel.cpp`, modify `apps/ecat-studio/CMakeLists.txt`

- [ ] Move SDO target panel methods (updateSdoTargetPanel, updateSdoInspector, etc.) to new file
- [ ] Update CMakeLists.txt to include new source
- [ ] Build and run tests — all must pass
- [ ] Commit: "refactor: extract SDO target panel to separate workspace file"

### Task 7: Extract SdoEvidence from MainWindowSdoWorkspace.cpp

**Files:** Create `apps/ecat-studio/workspaces/MainWindowSdoEvidence.cpp`, modify `apps/ecat-studio/CMakeLists.txt`

- [ ] Move SDO evidence methods (currentSdoEvidenceCandidates, copyCurrentSdoEvidenceDigest, etc.) to new file
- [ ] Update CMakeLists.txt
- [ ] Build and run tests
- [ ] Commit: "refactor: extract SDO evidence to separate workspace file"

### Task 8: Reduce MainWindow.h member variables

**Files:** `apps/ecat-studio/MainWindow.h`, various workspace files

- [ ] Group remaining direct members (sdoInspectorLabel_, sdoTargetTable_, sdoIndex_, sdoSubIndex_, sdoValue_, sdoWriteValue_, useSdoValueButton_, sdoType_) into `SdoInspectorWidgets` struct
- [ ] Group raw text panels (masterText_, infoText_, pdoText_, sdoText_, xmlText_, logText_, projectNotes_) into `RawTextWidgets` struct
- [ ] Build and run tests
- [ ] Commit: "refactor: extract SdoInspectorWidgets and RawTextWidgets from MainWindow.h"

## Week 3: Features & Tests

### Task 9: Add LanguageManager unit tests

**Files:** Create `tests/language_manager_test.cpp`, modify `tests/CMakeLists.txt`

- [ ] Write tests: default language is English, fromDisplayName works, setCurrentLanguage works, languages() returns correct count
- [ ] Add CMake target (links Qt6::Core only)
- [ ] Build and run — all tests pass
- [ ] Commit: "test: add LanguageManager unit tests"

### Task 10: Add workspace widget lifecycle tests

**Files:** Create `tests/workspace_widgets_test.cpp`, modify `tests/CMakeLists.txt`

- [ ] Write tests: all 12 widget structs can be allocated, members are nullptr by default, struct can be deleted safely
- [ ] Add CMake target
- [ ] Build and run
- [ ] Commit: "test: add workspace widget lifecycle tests"

### Task 11: Add settings persistence tests

**Files:** Create `tests/settings_persistence_test.cpp`, modify `tests/CMakeLists.txt`

- [ ] Write tests: AppSettings defaults, MasterProfile defaults, settings round-trip through QSettings
- [ ] Add CMake target
- [ ] Build and run
- [ ] Commit: "test: add settings persistence tests"

## Week 4: Polish & Verification

### Task 12: Add comments to remaining large files

**Files:** All .cpp files >500 lines with <5% comment density

- [ ] Add method-level docstrings and inline comments
- [ ] Target: 8% minimum comment density
- [ ] Build and verify
- [ ] Commit: "docs: add comprehensive comments to remaining large files"

### Task 13: Enable docs/ tracking

**Files:** `.gitignore`, `docs/`

- [ ] Remove `docs/` from .gitignore
- [ ] Add `docs/build/` to .gitignore instead
- [ ] Commit: "chore: enable docs/ tracking in git"

### Task 14: Final verification

- [ ] Build all targets clean, 0 warnings
- [ ] Run full test suite — all must pass (target: 60+ tests)
- [ ] Run GUI smoke test — exit 124
- [ ] Verify MainWindow.h ≤ 500 lines
- [ ] Verify comment density ≥ 8% in all files >200 lines
- [ ] Commit: "chore: final verification checkpoint"

---

## Verification Checklist

After all tasks:
- [ ] `cmake --build build` — clean, 0 warnings
- [ ] `ctest --test-dir build --output-on-failure` — 60+ tests pass
- [ ] `QT_QPA_PLATFORM=offscreen timeout 3 build/apps/ecat-studio/ecat-studio` — exit 124
- [ ] MainWindow.h ≤ 500 lines
- [ ] No duplicate includes in any workspace file
- [ ] Comment density ≥ 8% in all files >200 lines
- [ ] `docs/` tracked in git
