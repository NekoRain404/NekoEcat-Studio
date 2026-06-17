# Phase 3: i18n Infrastructure + Polish — Design Spec

> Date: 2026-06-17 | Branch: dev/phase3-i18n-polish

## Audit Summary

### Current State
- 52/52 tests passing, clean build
- `uiText("English", "中文")` pattern used 2878 times across 15 files
- MainWindow.h: 798 lines, ~100 member variables
- MainWindow.cpp: 4676 lines across 16 partial-class files
- Architecture: models/ adapters/ ui_state/ helpers/ infra/ workspaces/ — well-layered

### Issues Found

1. **No Qt i18n infrastructure**: `uiText()` is hardcoded to `settings_.language == "简体中文"`. No QTranslator, no .ts files, no language switching at runtime without `rebuildUi()`.

2. **MainWindow header bloat**: 798 lines, ~100 widget pointers that could be localized to workspace structs (following RtTestWidgets pattern).

3. **Incomplete file comments**: Only 15 files have file-level comments; ~80+ source files lack them.

4. **No service locator**: Workspace files access MainWindow members directly via `this->` — tight coupling.

5. **Charts module dependency**: `find_package(Qt6 COMPONENTS Charts)` but no Charts usage visible after OpenGL removal — possible dead dependency.

## Design Decisions

### i18n Approach
**Decision:** Keep inline `uiText()` pattern but add `LanguageManager` utility for:
- Language enumeration (English, 简体中文, 日本語, Deutsch)
- `rebuildUi()` trigger on language change
- Future .ts file generation via `lupdate` extraction

**Rationale:** The 2878 inline `uiText()` calls are deeply embedded. Migrating to QTranslator would require touching every call site. The current approach works and is actually more maintainable for a small team — translators can grep for `uiText(` and see both strings in context.

### MainWindow Header Reduction
**Decision:** Extract workspace widget pointers into per-workspace structs (like RtTestWidgets).

**Target structs:**
- `SdoWorkspaceWidgets` — sdoTable_, pdoTable_, sdoFilter_, etc.
- `WatchWorkspaceWidgets` — watchTable_, watchFilter_, etc.
- `ConsistencyWorkspaceWidgets` — consistencyTable_, consistencyFilter_, etc.
- `DiagnosticsWorkspaceWidgets` — diagnosticsTable_, hostHealthSummaryLabel_, etc.
- `IoVariableWorkspaceWidgets` — ioVariableTable_, ioVariableFilter_, etc.
- `SessionWorkspaceWidgets` — sessionBriefTable_, etc.

**Target:** MainWindow.h from 798 → ~400 lines.

### File Comments
**Decision:** Add file-level purpose comments to ALL .h and .cpp files that lack them.

## Verification Criteria
- [ ] Build clean with no new warnings
- [ ] 52/52 tests still pass
- [ ] GUI smoke test: exit 124
- [ ] MainWindow.h ≤ 450 lines
- [ ] All source files have file-level comments
