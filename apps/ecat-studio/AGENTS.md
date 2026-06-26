# apps/ecat-studio — NekoEcat Studio GUI

Qt6 desktop GUI for EtherCAT commissioning. Talks to `ecatd` over
newline-delimited JSON on `127.0.0.1:5877`.

## Directory Layout

| Directory     | Purpose |
|---------------|---------|
| `models/`     | Pure data/logic types — no QWidget dependencies |
| `adapters/`   | Bridge between models and `QTableWidget` |
| `detail/`     | Detail-panel text builders for each workspace |
| `utils/`      | Reusable utilities (table, text, UI widgets, docs) |
| `infra/`      | TCP client, shared POD types, settings, i18n |
| `workspaces/` | `MainWindow` partial implementations per workspace |
| (root)        | `MainWindow` core + `main.cpp` entry point |

## Architectural Layers

```
MainWindow  ──uses──►  workspaces/*.cpp   (UI construction, event wiring)
     │
     ├──reads──►  adapters/*              (populate QTableWidgets)
     ├──reads──►  detail/*                (detail panel text)
     ├──reads──►  models/*                (pure logic, filtering, scoring)
     │
     └──calls──►  infra/EcatClient        (JSON-over-TCP to ecatd)
```

- **Models** never include Qt widget headers.
- **Adapters** depend on models and `QTableWidget` only.
- **Detail** structs are plain data containers for detail panels.
- **MainWindow partials** in `workspaces/` implement different methods
  of the same `MainWindow` class (Qt partial-class pattern).

## Include Convention

All workspace `.cpp` files include `MainWindowIncludes.h` (the shared
precompiled header in `workspaces/`) which provides all model, adapter,
detail, utility, infra, and Qt headers. This centralizes common include
blocks across the current workspace partial files.

Other headers are included by bare name (e.g. `#include "SdoEvidenceModel.h"`).
CMake adds each subdirectory to the include path, so cross-directory
includes resolve without path prefixes.

## Settings System

`AppSettings` (defined in `infra/SettingsDialog.h`) contains all
persisted preferences. `MainWindow::loadSettings()` and
`MainWindow::saveSettings()` serialize via QSettings. The settings
dialog (`infra/SettingsDialog.cpp`) presents 7 tabbed sections:
Appearance, EtherCAT, Timing, Free Run, Display, Notifications, Export.

## Internationalization

Runtime language coverage is defined by `TranslationRegistry`. The
`uiText(english, zh)` method selects the active language.
User manual is bilingual (EN/ZH) with sidebar TOC and 22 sections.
