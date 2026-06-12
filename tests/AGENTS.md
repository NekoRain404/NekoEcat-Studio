# tests — Unit Tests

Each test executable links the minimal set of source files it needs.
Tests that require a `QTableWidget` use `QT_QPA_PLATFORM=offscreen`.

## Test Categories

- **Model tests** — Pure logic, no QApplication needed
- **Adapter tests** — Table population, need `Qt6::Widgets`
- **UI State tests** — Detail panel text generation

## Release Smoke Target

`cmake --build build --target release-smoke` runs the 15 core tests
that must pass before packaging a release.
