# NekoEcat Studio Development Flow

NekoEcat Studio is split into a GUI and a local runtime daemon.

- `ecatd`: local runtime service. The first backend shells out to the installed IgH `ethercat` CLI.
- `ecat-studio`: Qt desktop GUI. It talks to `ecatd` on `127.0.0.1:5877` using newline-delimited JSON.
- `src/igh`: IgH integration layer. This can later grow a native `ecrt.h` backend without changing the GUI.

See `docs/PROJECT_MANAGEMENT.md` for the directory map, dependency direction,
risk register, commenting standard, and gradual `MainWindow` split plan.

## Source Boundaries

- Source files live in `apps/`, `src/`, and `docs/`.
- `build/` is generated output. Do not edit it by hand.
- GUI-only review and navigation code must stay local-only: no SDO reads, SDO
  writes, state changes, Free Run toggles, rescan/connect calls, or Host Health
  runs.
- Online behavior should flow through `EcatClient` -> `ecatd` -> backend.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

Start the daemon first:

```bash
./build/apps/ecatd/ecatd
```

Then launch the GUI:

```bash
./build/apps/ecat-studio/ecat-studio
```

## First Runtime Features

- Master status via `ethercat master`
- Slave scan via `ethercat slaves`
- Bus rescan via `ethercat rescan`
- Slave state requests via `ethercat states`
- PDO view via `ethercat pdos`
- SDO dictionary via `ethercat sdos`
- SDO upload via `ethercat upload`
- XML view via `ethercat xml`

## Verification

For release收口 and normal UI-only edits, run the fast gate first:

```bash
clang-format -i <touched .cpp/.h files>
cmake --build build
cmake --build build --target release-smoke
QT_QPA_PLATFORM=offscreen timeout 5s build/apps/ecat-studio/ecat-studio
```

Before a tagged release, shared model refactor, CMake change, or risky workflow
change, run the full gate:

```bash
cmake --build build
ctest --test-dir build --output-on-failure
QT_QPA_PLATFORM=offscreen timeout 5s build/apps/ecat-studio/ecat-studio
```

Do not run `clang-format` on `CMakeLists.txt` files. CMake formatting should be
edited manually unless a CMake-aware formatter is introduced.

The offscreen smoke test is expected to return `124` because `timeout` stops the
GUI after startup.

## Release Checklist

Use this checklist when preparing a local release candidate:

```bash
cmake --build build
cmake --build build --target release-smoke
ctest --test-dir build --output-on-failure
QT_QPA_PLATFORM=offscreen timeout 5s build/apps/ecat-studio/ecat-studio
```

For a visual UI check without EtherCAT hardware, start the GUI under `xvfb-run`
and capture the main window. Confirm that Overview is the default workspace,
high-frequency tabs are reachable through the scrollable tab bar, and Host
Health remains inside Diagnostics.
