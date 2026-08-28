# Design: Real-time Platform - Unified NekoEcatClient with Shared Memory

**Date**: 2026-07-07  
**Status**: Draft for review  
**Owners**: Grok + user  

## 1. Goals & Scope

### Primary Goal
Transform NekoEcat Studio into a **solid real-time control platform** on Linux + IgH EtherCAT. External programs (C/C++, Python, ROS2, custom controllers, etc.) should be able to:
- Run their own real-time control logic.
- Get low-latency, deterministic access to process data.
- Use the tool reliably for commissioning + runtime control.

### v1 Scope (High-Performance Process Data Focus)
- Concentrate on **FreeRun / cyclic process data** (PDO exchange).
- Provide excellent SHM-based data plane + JSON-RPC control plane.
- Support both "full start" and "attach to running" modes.
- Deliver an official lightweight client library (`nekoecat_client.h` + Python examples).

**Out of scope for v1**:
- Full replacement of all existing RPC operations (SDO, state machine, diagnostics, etc.).
- Complex motion control, trajectory generation, or PLC-like runtime inside the client.
- Mandatory migration of the existing GUI.

### Non-Goals
- Become a full-featured soft PLC.
- Replace TwinCAT-style engineering for complex motion.

## 2. High-Level Architecture

```
External Real-time Program
          │
          ├── NekoEcatClient (unified)
          │     ├── JSON-RPC (control plane)
          │     │     - start/stop FreeRun
          │     │     - get layout
          │     │     - config / diagnostics
          │     └── Shared Memory (data plane)
          │           - Double-buffered process image
          │           - Direct read (inputs) + Direct write (outputs)
          │
          ▼
ecatd (daemon)
  - Owns the real-time cyclic loop (ecrt)
  - Mirrors domain data ↔ SHM (double buffer + version)
  - Applies validated writes from SHM into domain
  - Still exposes JSON-RPC for control
```

**Key Principles**
- Daemon owns the real-time domain and EtherCAT cycle.
- Client provides convenient, high-performance access.
- JSON-RPC remains the reliable control channel.
- Shared memory is the hot path for process data.

## 3. NekoEcatClient Responsibilities (Unified Client)

One object (`NekoEcatClient`) manages both planes:

- JSON-RPC connection lifecycle
- Shared memory mapping, layout caching, version tracking
- High-level API: `read_xxx_by_index()`, `write_xxx_by_index()`, `wait_next_cycle()`
- Mode support:
  - `start_freerun(mapping)` — full high-level start
  - `attach()` — connect to already-running FreeRun
- Layout handling: auto-fetch via RPC (default) + manual override
- Status / state machine exposure
- Configurable robustness (reconnection policy)

The client must be usable from C/C++ directly and from Python via ctypes / wrapper.

## 4. Shared Memory Design

### Layout (proposed)

```c
typedef struct {
    uint64_t version;           // monotonically increasing
    uint64_t cycle_count;
    uint64_t timestamp_ns;      // CLOCK_MONOTONIC
    uint32_t active_buffer;     // 0 or 1
    uint32_t data_size;
    uint32_t layout_version;
    uint32_t status_flags;      // WC, AL, running, etc.
    uint32_t reserved[4];
} NekoEcatShmHeader;

typedef struct {
    NekoEcatShmHeader header;
    uint8_t process_data[2][MAX_PD_SIZE];
} NekoEcatShm;
```

### Synchronization
- Double buffering + version number (chosen approach).
- Reader (client) does the classic double-read of `version` + `active_buffer`.
- Writer (client for outputs) writes to the appropriate buffer(s).
- Daemon updates the inactive buffer then flips `active_buffer` + bumps version.

### Writes
- Clients may directly write output data into SHM (performance priority).
- Daemon validates that the written entry is a configured output before applying it to the domain.
- Illegal writes are ignored (with optional counting/logging for diagnostics).

## 5. API Surface (High Level)

### C Helper (`nekoecat_client.h`)
- Creation / connection
- `start_freerun()` / `stop_freerun()`
- `attach_to_running()`
- `wait_next_cycle(timeout_ns)` (blocking)
- `try_read_latest(...)` (non-blocking)
- `read_u16_by_index(slave, index, sub, *out)`
- `write_u16_by_index(slave, index, sub, value)`
- Raw offset variants for maximum performance
- Status query + version access

Both blocking and non-blocking wait must be provided.

Python example using `mmap` + `ctypes` will be supplied.

## 6. Modes & Flexibility

- **startFreeRun mode**: Client sends the command and prepares SHM.
- **attach mode**: Client discovers (or is told) an existing running FreeRun and maps the SHM.
- Layout in attach: auto via JSON-RPC by default; user can also pass a layout manually.
- Both modes supported from day one.

## 7. Robustness & Error Handling

- Clear state machine exposed to the user (Connected, DataAvailable, DataStale, Error, etc.).
- Configurable recovery strategy:
  - Auto-reconnect RPC + re-attach SHM
  - Only report errors (user decides)
  - Hybrid with callbacks
- On SHM or RPC loss, the client must not silently continue with stale data.

## 8. Daemon-Side Changes (FreeRunController)

- Extend to support double-buffered SHM export.
- On every cycle:
  1. Process domain.
  2. Copy inputs to the inactive SHM buffer.
  3. Read latest outputs from SHM (validated).
  4. Apply validated outputs to domain.
  5. Flip active buffer + bump version.
- Basic output validation (only registered outputs).
- Expose current layout and SHM information over JSON-RPC.
- Keep existing telemetry for compatibility.

## 9. First Version Boundaries

**In scope**
- Unified `NekoEcatClient` (C + Python example)
- SHM double-buffer + version
- Direct read + direct write
- `start_freerun` + `attach`
- Auto + manual layout
- Configurable robustness + state machine
- Basic validation in daemon
- Documentation + examples

**Explicitly out of scope (v1)**
- Full wrapping of SDO / state machine / diagnostics inside the client
- Dynamic PDO mapping changes while running (stop + restart required)
- Complex motion features
- Mandatory replacement of internal GUI client

## 10. Evolution Path

- v1: External real-time programs (primary)
- Later: Internal GUI can gradually adopt the same client for process data access (design for reuse)
- Future: richer control-plane helpers, better diagnostics in SHM header, multi-domain support, etc.

## 11. Risks & Mitigations

| Risk                        | Mitigation                              |
|----------------------------|-----------------------------------------|
| Torn reads/writes          | Double buffer + version                 |
| Stale data after disconnect| Version + status flags + client checks  |
| Multiple writers           | Last-write-wins (documented) + validation |
| Layout version mismatch    | `layout_version` in header + client check |
| Performance regression     | Measure cycle jitter before/after       |

## 12. Implementation Outline (High Level)

1. Enhance `FreeRunController` (daemon) for SHM export + write application.
2. Add new JSON-RPC methods: layout query, SHM info, enhanced start.
3. Create `NekoEcatClient` (C++ or C wrapper) + pure C header for external use.
4. Implement Python bindings/examples.
5. Add tests (unit + integration with simulated or real slaves).
6. Update documentation and add examples under `examples/realtime/`.

## 13. Next Steps After Approval

- Write detailed interface definitions (C header + JSON-RPC additions).
- Prototype the SHM mirroring loop in the daemon.
- Measure baseline jitter on target hardware.
- Implement client library.

---

**Status**: Ready for review and approval.

Please reply with feedback or "approved" (or "start implementation") so we can move to detailed planning and coding.