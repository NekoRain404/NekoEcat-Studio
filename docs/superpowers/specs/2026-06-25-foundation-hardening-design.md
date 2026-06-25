# Foundation Hardening Design

## Purpose

This phase improves release trust and runtime safety without changing the product's user-facing workflow. It addresses issues found in the project scan: inconsistent versions, non-blocking CI quality gates, weak daemon request validation, synchronous external command risk, and experimental/stub features being presented as stable capabilities.

## Scope

In scope:

- Use one project version source for CMake, GUI, daemon responses, packaging scripts, and generated release metadata.
- Make CI quality checks fail when they find actionable issues, while keeping hardware-dependent IgH checks stubbed for CI.
- Add shared daemon request validation for bus-affecting commands.
- Remove unnecessary shell invocation from adapter probing and make external command execution behavior explicit.
- Default experimental services off, or clearly hide/label stub-only feature surfaces when they remain compiled.

Out of scope:

- Rewriting the entire plugin architecture.
- Replacing all synchronous daemon commands with a threaded job system.
- Changing the JSON protocol shape for existing successful responses.
- Reworking UI layout or visual design.
- Adding real AI, blockchain, quantum, FoE, CoE, or EoE implementations.

## Architecture

### Version Single Source

The CMake project version remains the source of truth. Build targets receive `NEKOECAT_VERSION` via compile definitions. GUI and daemon entry points use that value instead of hardcoded string literals. Packaging scripts derive their default version from CMake when no explicit version argument is provided.

This preserves existing command-line release flows while making manual packaging safer.

### CI Quality Gates

CI keeps the same job structure but changes soft checks into meaningful gates:

- GUI smoke test must fail on startup crash, while still treating a timeout as expected for a healthy Qt event loop.
- clang-tidy must either scan all production files or a deliberately named subset with a tracked allowlist.
- cppcheck uses non-zero exit behavior for warning/style/performance issues selected by the project.
- Valgrind reports should fail on definite leaks or invalid memory access for the selected test subset.

Hardware-dependent IgH headers may still be stubbed in CI, but that stub must be treated as a compatibility shim, not proof of hardware behavior.

### Daemon Request Validation

The daemon gets a small validation helper near `CommandDispatcher` or `EcatDaemon` that validates common fields:

- `position`: required integer, `0 <= position <= 65535`.
- `master`: optional numeric string, defaults to `"0"` only when omitted.
- `index`: required SDO index in hex or decimal, normalized to a canonical string.
- `subIndex`: required SDO subindex in hex or decimal, `0 <= subIndex <= 255`.
- `type`: optional but must be from the supported IgH/CLI type set when present.
- `state`: required AL state from a known allowlist.
- batch items: each row validated independently, with row-specific error output.

Validation runs before any backend call. Invalid requests return the existing failure envelope, so clients do not need protocol changes.

### External Command Boundaries

The first implementation step keeps the daemon single-threaded but tightens command execution:

- Replace `sh -c` adapter probing with `QProcess` program plus argument list.
- Preserve timeouts and capture stderr for diagnostics.
- Centralize helper behavior where practical so future async migration has one boundary to replace.

Commands that can block on real hardware remain synchronous in this phase, but the design makes the remaining risk explicit and test-covered.

### Experimental Feature Policy

Experimental services default to disabled for production builds. If compiled, feature surfaces that depend on stub/simulated services must be visibly marked as experimental in code metadata and docs, or hidden from default workspace registration.

This phase does not delete experimental code. It prevents placeholder capabilities from being advertised as stable EtherCAT engineering features.

## Data Flow

GUI requests keep the same newline-delimited JSON flow:

1. `EcatClient` sends a method and params.
2. `EcatDaemon` parses the frame.
3. `CommandDispatcher` selects a handler.
4. Handler validates params.
5. Handler calls backend only after validation passes.
6. Response uses the current `{id, ok, result}` or `{id, ok, error}` envelope.

Version data flows from CMake configuration into compile definitions and from packaging scripts into archive names and generated release notes.

## Error Handling

Validation errors should be specific and field-oriented, for example:

- `Missing required field: index`
- `Invalid position: expected integer in range 0..65535`
- `Invalid state: expected one of INIT, PREOP, SAFEOP, OP`

Batch operations return per-row failures without aborting the entire batch unless the request body itself is malformed.

CI scripts should fail loudly when a required tool is missing, except for the existing IgH package fallback path where CI deliberately creates a stub header.

## Testing

Add or update tests for:

- GUI and daemon report the CMake project version.
- Packaging scripts derive the default version correctly.
- Invalid daemon requests do not call backend operations.
- Batch Startup SDO validation reports row-level errors.
- Adapter probing uses argument-based `QProcess` and handles timeout/errors.
- CI script semantics are testable where possible through shellcheck-style static review or focused script tests.

Existing full verification remains:

- `cmake --build build -j$(nproc)`
- `ctest --test-dir build --output-on-failure -j$(nproc)`

## Rollout

Implement in small commits:

1. Version single-source.
2. Daemon validation helpers and tests.
3. External command boundary cleanup.
4. CI gate tightening.
5. Experimental feature default and documentation alignment.

Each commit should build and keep the full test suite passing.
