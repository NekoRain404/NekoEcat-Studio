# apps/ecat-studio — NekoEcat Studio GUI

Qt6 desktop GUI for EtherCAT commissioning. Talks to `ecatd` over
newline-delimited JSON on `127.0.0.1:5877`.

## Directory Layout

| Directory     | Purpose |
|---------------|---------|
| `models/`     | Pure data/logic types — no QWidget dependencies |
| `adapters/`   | Bridge between models and `QTableWidget` |
| `ui_state/`   | Detail-panel text builders for each workspace |
| `helpers/`    | Reusable utilities (table, text, UI widgets, docs) |
| `infra/`      | TCP client, shared POD types, settings dialog |
| `workspaces/` | `MainWindow` partial implementations per workspace |
| (root)        | `MainWindow` core + `main.cpp` entry point |

## Architectural Layers

```
MainWindow  ──uses──►  workspaces/*.cpp   (UI construction, event wiring)
     │
     ├──reads──►  adapters/*              (populate QTableWidgets)
     ├──reads──►  ui_state/*              (detail panel text)
     ├──reads──►  models/*                (pure logic, filtering, scoring)
     │
     └──calls──►  infra/EcatClient        (JSON-over-TCP to ecatd)
```

- **Models** never include Qt widget headers.
- **Adapters** depend on models and `QTableWidget` only.
- **UI State** structs are plain data containers for detail panels.
- **MainWindow partials** in `workspaces/` implement different methods
  of the same `MainWindow` class (Qt partial-class pattern).

## Include Convention

All headers are included by bare name (e.g. `#include "SdoEvidenceModel.h"`).
CMake adds each subdirectory to the include path, so cross-directory
includes resolve without path prefixes.
