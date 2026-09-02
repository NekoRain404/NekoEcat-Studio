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

### Real-Time Client Library

The pure-C real-time client is installed with the main build. After
`cmake --install`, the static library lands at `<prefix>/lib/libnekoecat_client.a`
and the shared headers (`nekoecat_client.h`, `shm_layout.h`, `nekoecat_shm.h`)
at `<prefix>/include/nekoecat`. A standalone build is also possible:

```bash
cmake -S client -B build-client
cmake --build build-client      # -> build-client/libnekoecat_client.a
```

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

## Run as a Service

The runtime daemon (`ecatd`) can run as a supervised boot service. The tar
package ships a systemd unit under `packaging/systemd/`.

```bash
# Install the unit and start ecatd on boot
sudo cp packaging/systemd/ecatd.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now ecatd
```

The unit waits for the IgH master (`After=network.target ethercat.service`,
`Wants=ethercat.service`) and supervises ecatd (`Restart=on-failure`). It runs
as root with an RT priority limit and an unlimited memlock so the Free Run
control loop can mmap its process image; restrict firmware transfers with
`NEKOECAT_FIRMWARE_DIR` (default `/var/lib/nekoecat/firmware` — create it first):

```bash
sudo install -d /var/lib/nekoecat/firmware
```

Verify and follow logs:

```bash
systemctl status ecatd
journalctl -u ecatd -f
```

To run ecatd straight from a build tree (no install), override the binary with
a drop-in instead of editing the unit:

```bash
sudo systemctl edit ecatd
```

```ini
[Service]
ExecStart=/home/yourname/nekoecat/build/apps/ecatd/ecatd --foreground
```

```bash
sudo systemctl daemon-reload && sudo systemctl restart ecatd
```

For a hardened non-root deployment (dedicated user + capabilities), see the
comment block at the top of `packaging/systemd/ecatd.service` and
`packaging/systemd/README.md`. To stop the service:

```bash
sudo systemctl disable --now ecatd
```

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
