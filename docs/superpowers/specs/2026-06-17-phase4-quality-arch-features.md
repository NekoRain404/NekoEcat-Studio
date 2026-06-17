# Phase 4: Quality, Architecture, Features — Design Spec

> Date: 2026-06-17 | Goal: Production-quality codebase with comprehensive comments, clean architecture, and expanded features.

## Problem Statement

NekoEcat Studio has solid architecture (models/adapters/ui_state/helpers/infra/workspaces) and 52 passing tests, but suffers from:
- Low comment density (0.4-2.3% in largest files)
- Duplicate includes in 15 workspace files
- Dead member variables not extracted to workspace structs
- `docs/` gitignored — design specs not tracked
- No unit tests for LanguageManager
- MainWindow.h still 749 lines with 71 member variables
- Largest files (4800+ lines) need decomposition

## Design Decisions

### 1. Comment Strategy
**Target:** 8-12% comment density in all files >200 lines.
**Approach:** Add method-level docstrings to all public methods, inline comments for non-obvious logic.
**Standard:** Follow Qt/Javadoc style: `// Brief description.` for single-line, `/* ... */` for multi-line.

### 2. File Decomposition
**MainWindow.h target:** ≤500 lines (from 749).
**Approach:** Extract remaining direct members into workspace structs. Move method declarations to workspace-specific headers.
**MainWindowSdoWorkspace.cpp target:** Split into 3 files (SDO dictionary, SDO evidence, SDO target panel).

### 3. Dead Code Cleanup
**Approach:** Remove duplicate includes, merge `freeRunTable_` into `FreeRunWorkspaceWidgets`, remove unused forward declarations.

### 4. i18n Expansion
**Approach:** Add LanguageManager unit tests. Keep inline `uiText()` pattern. Add Japanese and German stub translations.

### 5. Test Coverage
**Target:** 60+ tests (from 52).
**New tests:** LanguageManager, workspace widget lifecycle, settings persistence.

## Verification Criteria
- [ ] Build clean, 0 warnings
- [ ] 60+ tests passing
- [ ] GUI smoke test: exit 124
- [ ] MainWindow.h ≤ 500 lines
- [ ] Comment density ≥ 8% in all files >200 lines
- [ ] No duplicate includes
- [ ] No dead member variables
