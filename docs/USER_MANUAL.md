# NekoEcat Studio User Manual

## Overview

NekoEcat Studio is a modern EtherCAT engineering workstation for Linux. It provides
a graphical environment for commissioning, diagnostics, monitoring, and maintenance
of EtherCAT networks using the IgH EtherCAT Master stack.

The application is split into two processes:

- **ecatd** -- the background daemon that communicates with the IgH EtherCAT Master
  via the native ecrt API (with CLI fallback for operations the API does not cover).
- **ecat-studio** -- the Qt6-based GUI frontend that connects to ecatd over TCP
  (localhost, port 5877) using a JSON-over-TCP protocol.

The GUI is built on a plugin-based architecture with ServiceContainer dependency
injection. Each workspace tab is a self-contained plugin that registers its widget,
icon, and display name with the PluginRegistry.

## Installation

See `INSTALLATION.md` in the project root for build prerequisites, dependencies,
and step-by-step compilation instructions.

## Quick Start

### First Launch

1. Ensure the IgH EtherCAT Master is loaded and your network adapter is bound to
   the kernel driver (run `sudo ethercat master` to verify).
2. Start the daemon: `ecatd` (run as root or with appropriate capabilities).
3. Start the GUI: `ecat-studio`.
4. The GUI auto-connects to ecatd on localhost:5877. When connected, the status
   bar shows the bus state.
5. Click the "Scan Bus" button or use `Ctrl+R` to scan the EtherCAT bus and
   populate the topology.

> **Tip:** The daemon supports three backend modes: `auto` (default), `native`
> (ecrt API only), and `cli` (ethercat CLI commands only). Set the mode via
> `ecatd --backend <mode>`.

### Project Files

Save your workspace layout, connection settings, and notes as a `.ecatproj`
project file (`Ctrl+S`). Reopen with `Ctrl+O` to resume where you left off.

## Workspace Reference

NekoEcat Studio has 28 workspace tabs organized into functional groups. Use the
tab bar at the top or the View -> Workspaces menu to navigate.

### Home

| Tab | Description |
|-----|-------------|
| **Overview** | Dashboard showing bus topology summary, slave count, master state, and quick-access links to other workspaces. |

### Topology / Slaves

| Tab | Description |
|-----|-------------|
| **Topology** | Graphical bus topology with slave status indicators, drag-and-drop rearrangement, and state machine controls for individual slaves. |
| **ESI Browser** | Browse the ESI (EtherCAT Slave Information) database. Search by vendor, device name, or revision. |
| **Bus Statistics** | Real-time bus health metrics: frame rate, error counters, lost frames, and jitter. |
| **I/O Variables** | Engineering table of process data variables. Add aliases, set default values, and export variable lists. |

### Dictionary / SDO

| Tab | Description |
|-----|-------------|
| **Object Dictionary** | Hierarchical OD browser with search, SDO read/write with evidence tracking, bookmarks, access trail, and history. |
| **Startup SDO** | Configure SDO sequences that are written automatically on bus startup. Manage lists per slave. |
| **PDO Mapping** | View and edit PDO assignment and mapping for each slave. Add or remove PDO entries and configure sync managers. |

### Monitoring

| Tab | Description |
|-----|-------------|
| **Watch** | Add multiple SDO/PDO targets for real-time monitoring. Batch read/write, value-change highlighting, CSV export. |
| **Signal Analyzer** | Live signal visualization with zoom, pan, and measurement cursors. |
| **Oscilloscope** | Multi-channel oscilloscope-style display for analog and digital signals. Trigger modes and persistence. |
| **Signal Trace** | High-speed trace recording for post-capture analysis. Configurable trigger conditions and buffer depth. |
| **Logic Analyzer** | Digital logic analysis with protocol decoding, pattern triggers, and export. |

### DC (Distributed Clocks)

| Tab | Description |
|-----|-------------|
| **DC Sync** | Overview of distributed clock synchronization status per slave. |
| **DC Sync Precision** | Detailed jitter and drift measurements with statistical distribution charts. |
| **DC Sync Optimizer** | Guided tool for optimizing DC sync parameters (shift times, cycle offsets) across the network. |

### Runtime

| Tab | Description |
|-----|-------------|
| **Free Run** | Process image I/O testing in a real-time loop (SCHED_FIFO, ~1 kHz). Toggle outputs, monitor inputs, log data. |
| **State Machine** | Master- and slave-level EtherCAT state machine control (Init, Pre-Op, Safe-Op, Op). |

### Diagnostics

| Tab | Description |
|-----|-------------|
| **Online Diagnostics** | Real-time health dashboard with live slave status, error rates, and link quality. |
| **Diagnostics** | Historical error analysis, correlated events, and trend charts. |
| **Consistency** | Cross-check OD, PDO, and DC configuration against the ESI definitions. Flag mismatches and unsupported entries. |
| **AL Events** | Real-time AL (Application Layer) event log from the bus. |
| **Alarms** | User-configurable alarm rules with severity levels, notifications, and logging. |

### Protocols

| Tab | Description |
|-----|-------------|
| **EoE** | Ethernet over EtherCAT management: configure virtual network interfaces, monitor tunnel traffic. |
| **SoE Drive** | Servo Drive over EtherCAT (SoE) access: drive parameters, homing, and motion commands. |
| **ENI Export** | Export the current bus configuration as an ENI (EtherCAT Network Information) XML file for compatible runtime tools. |

### System

| Tab | Description |
|-----|-------------|
| **Session** | Multi-session management: save and switch between bus configurations without relaunching. |
| **Notes** | Free-form notes attached to the project. Supports Markdown and inline images. |

## Connection to Daemon and Bus

### Architecture

```
ecat-studio (GUI)  --JSON/TCP-->  ecatd (daemon)  --ecrt/CLI-->  IgH EtherCAT Master
                                      |
                                  FreeRunController
                                   (real-time loop)
```

The GUI connects to the daemon automatically on startup. If ecatd is not
running, the status bar shows "Disconnected." Reconnection uses exponential
backoff (2 s -> 4 s -> 8 s -> 16 s -> 30 s capping).

### Daemon Backend Modes

| Mode  | Behavior |
|-------|----------|
| auto  | Try native ecrt API first; fall back to CLI for operations ecrt cannot do (SDO enumeration, ESI XML, AL state transitions, rescan). |
| native| Use ecrt API only. Falls back to CLI only for the five unsupported operations, marking each response with `"backend": "cli_fallback"`. |
| cli   | Always shell out to the `ethercat` command-line tool. Most compatible but slower. |

The GUI can interrogate the `lastOperationWasFallback()` flag on each response
to show which backend was used.

### Free Run Mode

The Free Run workspace starts a dedicated real-time thread in ecatd. The thread
opens the IgH master directly (`ecrt_request_master`), creates a PDO domain,
configures slaves, and runs a cyclic exchange at approximately 1 kHz. This path
is independent of the normal CLI or native backend.

## Language Switching

NekoEcat Studio supports 8 languages. Switch via `Tools -> Settings -> Display
-> Language` (restart required):

| Language              | Display Name |
|-----------------------|--------------|
| English               | English      |
| Chinese (Simplified)  | 简体中文     |
| Japanese              | 日本語       |
| German                | Deutsch      |
| Korean                | 한국어       |
| Chinese (Traditional) | 繁體中文     |
| French                | Français     |
| Spanish               | Español      |

After changing the language, the workspace tabs are rebuilt automatically via
the PluginRegistry, which reloads each plugin's localized display name and icon.

## Troubleshooting

### Common Issues

1. **"Disconnected" in the status bar** -- ecatd is not running. Start it with
   `sudo ecatd` and check the daemon logs.
2. **No slaves found after scan** -- Verify the network cable and that the IgH
   master is loaded (`sudo ethercat master`). Check that your NIC is bound to
   the ecat driver (`sudo ethercat slaves`).
3. **SDO read/write timeout** -- Increase the timeout value in
   `Tools -> Settings -> Connection`. Heavily loaded buses may need 5-10 s.
4. **Permission denied** -- The daemon requires root (or CAP_NET_ADMIN +
   CAP_SYS_NICE). The GUI does not need elevated privileges.
5. **Free Run won't start** -- Ensure ecatd has real-time scheduling
   capabilities (mlockall, SCHED_FIFO). Check `ulimit -l` and `chrt`
   permissions.
6. **CLI fallback warnings** -- These are expected for SDO enumeration, ESI
   XML, AL state transitions, and rescan. The native ecrt API does not expose
   these operations.

### Log Files

- **Daemon logs**: `~/.config/NekoEcatStudio/logs/ecatd.log`
- **GUI logs**: `~/.config/NekoEcatStudio/logs/ecat-studio.log`

Set the log level in `Tools -> Settings -> Logging`.

### Keyboard Shortcuts

| Shortcut   | Action              |
|------------|---------------------|
| Ctrl+N     | New project         |
| Ctrl+O     | Open project        |
| Ctrl+S     | Save project        |
| Ctrl+R     | Scan bus            |
| Ctrl+F     | Search OD           |
| F5         | Refresh topology    |
| Ctrl+D     | Disconnect / connect|

### Tests

The project includes 247 unit and integration tests covering core services,
handlers, and performance benchmarks. Run them from the build directory:

```bash
ctest --output-on-failure
```