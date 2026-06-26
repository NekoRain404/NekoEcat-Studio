# NekoEcat Studio Installation Guide

## System Requirements

- **OS**: Linux x86_64 (Arch, Ubuntu, Fedora, Debian, etc.)
- **RAM**: 4GB minimum, 8GB recommended
- **Disk**: 500MB for installation
- **Dependencies**: Qt6, IgH EtherCAT Master

## Installation Methods

### Method 1: Binary Package (tar.gz)

```bash
# Download and extract
tar -xzf NekoEcat-Studio-v<version>-linux-x86_64.tar.gz
cd NekoEcat-Studio-v<version>-linux-x86_64

# Run
./NekoEcat-Studio
```

### Method 2: DEB Package (Debian/Ubuntu)

```bash
# Install
sudo dpkg -i nekoecat-studio_<version>_amd64.deb

# Fix dependencies if needed
sudo apt-get install -f

# Run
ecat-studio
```

### Method 3: Build from Source

```bash
# Extract source
tar -xzf NekoEcat-Studio-<version>-src.tar.gz
cd NekoEcat-Studio-<version>-src

# Install dependencies (Arch Linux)
sudo pacman -S qt6-base qt6-tools cmake gcc

# Install dependencies (Ubuntu/Debian)
sudo apt-get install qt6-base-dev qt6-tools-dev cmake g++

# Build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Run
./build/apps/ecat-studio/ecat-studio
```

## Runtime Dependencies

- Qt6 Core, Network, Widgets, GUI runtime libraries
- IgH EtherCAT Master runtime (`libethercat.so.1`)
- Configured EtherCAT NIC and master service

### Installing IgH EtherCAT Master

```bash
# Arch Linux
sudo pacman -S ethercat

# Ubuntu/Debian (build from source)
# See: https://etherlab.org/en/ethercat/
```

## Post-Installation Setup

1. Configure EtherCAT network interface
2. Start IgH EtherCAT Master service
3. Grant user permissions for EtherCAT access
4. Launch NekoEcat Studio

## Verification

```bash
# Check installation
ecat-studio --version

# Run smoke tests
cmake --build build --target release-smoke
```

## Troubleshooting

- **GUI doesn't start**: Check Qt6 runtime libraries
- **Cannot connect to bus**: Verify IgH master service and NIC configuration
- **Permission denied**: Add user to `ethercat` group
