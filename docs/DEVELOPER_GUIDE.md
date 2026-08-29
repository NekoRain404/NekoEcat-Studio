# NekoEcat Studio Developer Guide

## Architecture Overview

NekoEcat Studio follows a modular architecture:

```
NekoEcatStudio/
├── src/core/          # Shared libraries (EthercatTypes, JsonProtocol)
├── src/igh/           # IgH EtherCAT CLI adapter
├── apps/ecat-studio/  # Qt6 desktop GUI application
├── apps/ecatd/        # Runtime daemon
├── tests/             # Unit, integration, and performance tests
├── scripts/           # Build and packaging scripts
└── docs/              # Documentation
```

### Technology Stack

- **Language**: C++20
- **GUI Framework**: Qt6 (Core, Network, Widgets)
- **Build System**: CMake 3.20+
- **Testing**: Qt Test framework
- **EtherCAT**: IgH EtherCAT Master

### Plugin Architecture

The application uses a plugin-based architecture:

- **WorkspacePlugin**: Interface for workspace plugins
- **EventBus**: Publish-subscribe event system
- **ServiceContainer**: Dependency injection container
- **PluginRegistry**: Plugin lifecycle management

## Building

### Prerequisites

```bash
# Arch Linux
sudo pacman -S qt6-base qt6-tools cmake gcc

# Ubuntu/Debian
sudo apt-get install qt6-base-dev qt6-tools-dev cmake g++

# IgH EtherCAT Master (required for runtime)
sudo pacman -S ethercat  # Arch Linux
```

### Build Commands

```bash
# Configure (default build: 179 tests, 24 visible workspace tabs, 42 service TUs)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Run tests
ctest --test-dir build --output-on-failure -j$(nproc)

# Install (client library to lib/ + headers to include/nekoecat)
cmake --install build --prefix /usr/local

# With experimental services (adds AlarmService, LoggingService, and their tests)
cmake -B build -DECAT_EXPERIMENTAL_SERVICES=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure -j$(nproc)
```

### Build Types

- **Release**: Optimized build; validate target hardware and runtime before field use
- **Debug**: Debug symbols, no optimization
- **Coverage**: Instrumented for code coverage analysis

## Testing

### Test Structure

```
tests/
├── unit/              # Unit tests for individual components
├── integration/       # Integration tests for component interactions
├── performance/       # Performance benchmarks
├── fixtures/          # Test fixtures and utilities
└── mocks/             # Mock objects for testing
```

### Running Tests

```bash
# All tests
ctest --test-dir build --output-on-failure -j$(nproc)

# Specific test
./build/tests/unit/core/event_bus_test

# Run only the unit / integration / performance suites via labels
ctest --test-dir build -L unit
ctest --test-dir build -L integration
ctest --test-dir build -L performance

# With coverage
bash scripts/analyze_coverage.sh

# Memory leak check
bash scripts/check_memory.sh

# Performance benchmarks
bash scripts/benchmark.sh
```

The default `ctest` run only builds and executes tests whose system under test
is actually compiled into the application. The default suite is **179 tests
(143 unit + 11 integration + 25 performance)** — all pass in a fresh Release
build with `ECAT_EXPERIMENTAL_SERVICES=OFF`. The default build compiles 34
plugins (24 visible workspace tabs) and 42 service translation units in
`apps/ecat-studio/services/`; `ServiceContainer` exposes 41 accessors
(`EcatClient` + `EventBus` + 39 domain services).

Services/plugins that are not part of the default app build (workflow,
optimization, EtherCAT analytics, dashboard editors, etc.) are gated behind the
`ECAT_EXPERIMENTAL_SERVICES` option:

```bash
cmake -B build -DECAT_EXPERIMENTAL_SERVICES=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure -j$(nproc)
```

### IgH EtherCAT header stub

The daemon and core tests need IgH `ecrt.h`/`ethercat.h` headers. CI does not
install the IgH stack; it installs a tracked, complete stub via:

```bash
bash .github/workflows/ci/setup-ethercat-stub.sh
```

which copies `.github/workflows/ci/ecrt_stub.h` into place. Contributors without
IgH installed can use the same script locally to build and test the daemon/core
without a real IgH master. The stub provides the ecrt API surface the daemon
uses; runtime hardware paths are exercised on real targets.

### Building and using the real-time client library

The pure-C client is built independently of the GUI/daemon and consumed like any
static library:

```bash
cmake -S client -B build-client
cmake --build build-client
# produces build-client/libnekoecat_client.a
```

From the top-level build it is built as part of the default tree and installed by
`cmake --install` (library to `lib/`, headers `nekoecat_client.h`/`shm_layout.h`/
`nekoecat_shm.h` to `include/nekoecat`). The client speaks JSON-RPC to ecatd
(control plane) and reads the shared-memory Free Run process image (data plane).
A runnable Python example is provided:

```bash
python3 examples/realtime/nekoecat_client_example.py [--layout layout.json] [--cycles N]
```

Scaffold a C demo against the installed headers/library:

```bash
cc -I include/nekoecat demo.c -L lib -lnekoecat_client -lpthread -o demo
```

### Writing Tests

```cpp
#include <QtTest/QtTest>

class MyTest : public QObject {
    Q_OBJECT
private slots:
    void testFeature();
};

void MyTest::testFeature() {
    QCOMPARE(1 + 1, 2);
}

QTEST_MAIN(MyTest)
#include "my_test.moc"
```

## Code Style

### C++ Standards

- Use C++20 features (concepts, ranges, etc.)
- Prefer `auto` for type deduction
- Use RAII for resource management
- Follow Qt naming conventions for Qt-related code

### Naming Conventions

- **Classes**: PascalCase (`EventBus`, `PluginRegistry`)
- **Functions**: camelCase (`connectToBus`, `readSdo`)
- **Variables**: camelCase (`slaveCount`, `errorCode`)
- **Constants**: UPPER_SNAKE_CASE (`MAX_SLAVES`, `DEFAULT_TIMEOUT`)
- **Namespaces**: lowercase (`ecat`, `core`)

### File Organization

- Header files: `.h`
- Source files: `.cpp`
- One class per file (with exceptions for closely related classes)
- Include guards: `#pragma once`

## Contributing

### Workflow

1. Fork the repository
2. Create a feature branch
3. Write tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

### Code Review

- All changes require review
- Tests must pass before merge
- Documentation updates for user-facing changes

## Release Process

### Version Bumping

1. Update `CMakeLists.txt` version
2. Update `CHANGELOG.md`
3. Update `RELEASE_NOTES.md`
4. Update `README.md` examples

### Packaging

```bash
# Create all packages
bash scripts/package-linux.sh <version>
bash scripts/package-deb.sh <version>
bash scripts/package-source.sh <version>

# Create release
bash scripts/release.sh <version>
```

### Quality Checks

```bash
# Code quality
bash scripts/check_quality.sh

# Coverage analysis
bash scripts/analyze_coverage.sh

# Memory leak check
bash scripts/check_memory.sh

# Performance benchmarks
bash scripts/benchmark.sh
```

## API Reference

### Core Types (src/core/EthercatTypes.h)

- `EcSlaveInfo`: Slave information structure
- `EcPDOEntry`: PDO entry definition
- `EcSDOValue`: SDO value container

### EventBus (apps/ecat-studio/services/EventBus.h)

```cpp
// Subscribe to events
eventBus->subscribe("topology_changed", [](const QVariant& data) {
    // Handle event
});

// Publish events
eventBus->publish("topology_changed", topologyData);
```

### ServiceContainer

```cpp
// Register services
container->registerService<EventBus>(new EventBus());

// Resolve services
auto eventBus = container->resolve<EventBus>();
```

## Debugging

### Enable Debug Logging

```bash
# Set log level
export NEKOECAT_LOG_LEVEL=DEBUG

# Run with debug output
ecat-studio --debug
```

### Common Debug Scenarios

1. **Connection issues**: Check adapter selection and master service
2. **SDO errors**: Enable SDO logging in settings
3. **UI freezes**: Check for blocking operations in main thread
4. **Memory issues**: Run with Valgrind

## Performance Tuning

### Build Optimization

```bash
# Use ccache
export CCACHE_DIR=~/.cache/ccache
cmake -B build -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

# Parallel build
cmake --build build -j$(nproc)
```

### Runtime Optimization

- Use connection pooling for bus operations
- Cache frequently accessed data
- Minimize UI updates during batch operations
- Use background threads for I/O operations

## License

NekoEcat Studio is licensed under the GNU General Public License v3.0. See LICENSE for details.
