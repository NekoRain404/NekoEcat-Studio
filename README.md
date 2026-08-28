<div align="center">
  <img src="https://img.shields.io/badge/NekoEcat%20Studio-v1.0.0--beta-6C5CE7?style=for-the-badge&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSI0MCIgaGVpZ2h0PSI0MCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9IiM2QzVDRTciIHN0cm9rZS13aWR0aD0iMiI+PHBhdGggZD0iTTEyIDJMMTIgMjJNMiAxMkwyMiAxMiIvPjwvc3ZnPg==">
  <h1>NekoEcat Studio</h1>
  <p><strong>A modern EtherCAT engineering workstation for Linux + IgH</strong></p>
  <p>Commission, debug, and diagnose your EtherCAT bus with confidence.</p>

  <p>
    <img alt="C++20" src="https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=cplusplus&logoColor=white">
    <img alt="Qt6" src="https://img.shields.io/badge/Qt-6-41CD52?style=for-the-badge&logo=qt&logoColor=white">
    <img alt="CMake" src="https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white">
    <img alt="Linux" src="https://img.shields.io/badge/Linux-EtherCAT-FCC624?style=for-the-badge&logo=linux&logoColor=111111">
    <img alt="License" src="https://img.shields.io/badge/License-GPL--3.0-6C5CE7?style=for-the-badge">
    <img alt="Tests" src="https://img.shields.io/badge/Tests-175%20passed-00D68F?style=for-the-badge">
  </p>
</div>

---

NekoEcat Studio is an **EtherCAT commissioning and diagnostics workstation** purpose-built for Linux systems using the IgH EtherCAT Master stack. It replaces the workflow of juggling terminals, spreadsheets, and custom scripts with a **coherent, evidence-driven desktop application**.

Whether you are integrating a new drive, tuning distributed clocks, debugging a startup sequence, or handing off a validated configuration — NekoEcat Studio gives you a single tool for the job.

## ✨ Features

### 🔌 Full EtherCAT Protocol Support

| Protocol | Status | Details |
|----------|--------|---------|
| **CoE** (CANopen over EtherCAT) | ✅ | SDO upload/download, dictionary browsing, emergency object monitoring |
| **FoE** (File over EtherCAT) | ✅ | Firmware read/write via daemon |
| **EoE** (Ethernet over EtherCAT) | ✅ | IP configuration, frame statistics |
| **SoE** (Servo over EtherCAT) | ✅ | IDN read/write, parameter validation |

### 🛠️ 24 Purpose-Built Workspaces

Organized into logical groups — no clutter, just the right tool for each task.
(34 plugins are compiled and registered; the 24 visible ones each contribute a
workspace tab, the rest are hidden helper/preference plugins.)

| Group | Workspaces |
|-------|-----------|
| **🏠 Overview** | Dashboard with session brief, evidence matrix, next-best-action |
| **🔍 Topology & Slaves** | Topology graph, ESI browser, bus statistics, I/O variable table |
| **📖 Dictionary & SDO** | Object dictionary, startup SDO sequences, PDO mapping editor |
| **📊 Monitoring** | Watch window, signal analyzer, oscilloscope, trace, logic analyzer |
| **⚡ Distributed Clocks** | DC sync status, precision tuning, drift optimizer |
| **🔄 Runtime** | FreeRun process image, state machine controller |
| **🩺 Diagnostics** | Online diagnostics, consistency checks, AL events, alarm panel |
| **🌐 Protocols** | EoE tunnel, SoE drive interface, ENI export |

### 🧠 Evidence-Driven Workflow

Every SDO write, state transition, and configuration change is tracked. Before writing to the bus, you can review the evidence trail — what was read, what changed, and whether it matches expectations.

### 🌍 8 Languages

Switch between English, 简体中文, 日本語, Deutsch, 한국어, 繁體中文, Français, and Español — at runtime, no restart needed.

### 🔌 Dual Backend Architecture

```
NekoEcat Studio  ←JSON-RPC→  ecatd daemon  ←ecrt/CLI→  IgH EtherCAT Master  ←→  Bus
```

- **Native ecrt API** — high-performance SDO, PDO, and slave operations
- **CLI fallback** — automatic fallback where ecrt is unavailable (ESI XML, SDO dictionary enumeration, state transitions)
- **Backend selection** — auto, native-only, or CLI-only modes

## 🚀 Quick Start

### Prerequisites

- Linux x86_64 with IgH EtherCAT Master 1.6.x
- Qt6 (Core, Network, Widgets)
- CMake 3.20+

### Install

```bash
# Debian / Ubuntu
sudo dpkg -i NekoEcatStudio-1.0.0_amd64.deb
sudo apt-get install -f
ecat-studio

# Fedora / RHEL
sudo dnf install ./NekoEcatStudio-1.0.0.x86_64.rpm
ecat-studio

# Portable (any distro)
tar -xzf NekoEcatStudio-1.0.0-linux-x64.tar.gz
./NekoEcatStudio-1.0.0-linux-x64/bin/ecat-studio
```

### Build from source

```bash
git clone https://github.com/NekoRain/nekoecat-studio.git
cd nekoecat-studio
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Start the daemon (requires IgH master)
sudo ./build/apps/ecatd/ecatd

# Launch the GUI
./build/apps/ecat-studio/ecat-studio
```

### First Steps

1. **Start the daemon** — `ecatd` connects to the IgH EtherCAT Master
2. **Launch the GUI** — `ecat-studio` auto-connects via localhost:5877
3. **Scan the bus** — the Overview workspace will populate with detected slaves
4. **Browse the Object Dictionary** — read SDO entries with evidence tracking
5. **Configure startup SDOs** — build reusable configuration sequences
6. **Go live** — use FreeRun for process image monitoring and Watch for real-time SDO polling

## 🧪 Tested

- **175 automated tests** — the default build runs unit, integration, and performance tests covering the plugins, daemon handlers, and services that actually ship. Reaching additional (previously removed) services/plugins requires `cmake -B build -DECAT_EXPERIMENTAL_SERVICES=ON`
- **Verified on real hardware** — SDO read/write, slave scan, state transitions, and PDO mapping confirmed on Beckhoff-compatible digital I/O slave
- **Test environment**: IgH Master 1.6.9, Linux 7.0.12-zen, RTL8125 NIC

## 🏗️ Architecture Overview

```
┌──────────────────────────────────────────────────────────┐
│                    NekoEcat Studio                        │
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────────┐  │
│  │ 34 Plugins   │  │ EventBus     │  │ ServiceContainer │  │
│  │ (24 visible) │  │ Pub/Sub      │  │ Dependency Inj.  │  │
│  └──────┬──────┘  └──────┬───────┘  └────────┬────────┘  │
│         │                │                    │           │
│  ┌──────┴────────────────┴────────────────────┴────────┐  │
│  │              EcatClient (JSON-RPC)                   │  │
│  └─────────────────────┬───────────────────────────────┘  │
└────────────────────────┼──────────────────────────────────┘
                         │ TCP localhost:5877
┌────────────────────────┼──────────────────────────────────┐
│  ┌─────────────────────┴───────────────────────────────┐  │
│  │                   ecatd daemon                       │  │
│  │  ┌──────────────┐  ┌─────────────┐  ┌────────────┐  │  │
│  │  │ CLI Backend   │  │ Native ecrt │  │ FreeRun    │  │  │
│  │  │ (ethercat cmd)│  │ Backend     │  │ Controller │  │  │
│  │  └──────┬───────┘  └──────┬──────┘  └─────┬──────┘  │  │
│  └─────────┼─────────────────┼────────────────┼─────────┘  │
└────────────┼─────────────────┼────────────────┼────────────┘
             │                 │                │
             └────────┬────────┴────────────────┘
                      │
             ┌────────┴────────┐
             │ IgH EtherCAT    │
             │ Master (ecrt)   │
             └────────┬────────┘
                      │
             ┌────────┴────────┐
             │  EtherCAT Bus   │
             │  (real slaves)  │
             └─────────────────┘
```

## 📚 Documentation

| Document | Description |
|----------|------------|
| [User Manual](docs/USER_MANUAL.md) | Getting started, workspace reference, troubleshooting |
| [Installation Guide](docs/INSTALLATION.md) | System requirements, multiple install methods |
| [Architecture](docs/ARCHITECTURE.md) | System design, IPC protocol, plugin system |
| [Plugin Guide](docs/PLUGIN_GUIDE.md) | Creating and integrating custom workspace plugins |
| [Developer Guide](docs/DEVELOPER_GUIDE.md) | Build system, testing, contribution workflow |
| [Changelog](CHANGELOG.md) | Release history and version notes |

## 🤝 Contributing

Contributions are welcome! Whether it is a bug report, feature request, or pull request:

1. Check the [open issues](https://github.com/NekoRain/nekoecat-studio/issues) for existing discussions
2. Read the [Developer Guide](docs/DEVELOPER_GUIDE.md) for build and test setup
3. Submit a PR — the CI pipeline builds and runs the full default test suite automatically

## 📄 License

NekoEcat Studio is free software licensed under the **GNU General Public License v3.0**.

---

<div align="center">
  <sub>Built with Qt 6 · IgH EtherCAT Master · CMake · ❤️</sub>
  <br>
  <sub>Copyright © 2026 NekoRain</sub>
</div>
