#!/usr/bin/env bash
# CI helper: provide an IgH EtherCAT header + linkable libethercat.so stub.
#
# Some distros ship native IgH (apt ethercat-master-dev / dnf ethercat-devel /
# arch ethercat). When real headers are absent this installs the tracked
# ecrt_stub.h and a valid ELF shared object, so the daemon + core tests
# compile/link/run on every CI image.
#
# Runtime loader lib dir differs by distro: /usr/lib (Debian, Arch) vs
# /usr/lib64 (Fedora/RHEL). Install the .so everywhere so tests run in both.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
STUB_HEADER="${SCRIPT_DIR}/ecrt_stub.h"

# Try to install real IgH headers, ignore failures (stub covers it).
run_as() { "$@" 2>/dev/null || sudo -n "$@" 2>/dev/null || return 0; }
if command -v apt-get &>/dev/null; then
    run_as apt-get install -y ethercat-master-dev
elif command -v dnf &>/dev/null; then
    run_as dnf install -y ethercat-devel
elif command -v pacman &>/dev/null; then
    run_as pacman -S --noconfirm ethercat
fi

# Header: real or stub into /usr/include/ecrt.h
if [ ! -f /usr/include/ecrt.h ] && [ -f "${STUB_HEADER}" ]; then
    if cp "${STUB_HEADER}" /usr/include/ecrt.h 2>/dev/null; then
        :
    elif sudo -n true 2>/dev/null; then
        sudo cp "${STUB_HEADER}" /usr/include/ecrt.h
    else
        mkdir -p /usr/include
        cp "${STUB_HEADER}" /usr/include/ecrt.h
    fi
fi

# Shared object: empty libethercat.so for linking + runtime.
if [ ! -f /usr/lib/libethercat.so ] && [ ! -f /usr/lib64/libethercat.so ]; then
    echo 'int ecrt_stub_dummy;' | gcc -x c -shared -fPIC -o /tmp/libethercat.so - 2>/dev/null || true
    if [ -s /tmp/libethercat.so ]; then
        for libdir in /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64; do
            if [ -d "$libdir" ] && [ ! -f "$libdir/libethercat.so" ]; then
                cp /tmp/libethercat.so "$libdir/libethercat.so" 2>/dev/null \
                    || sudo cp /tmp/libethercat.so "$libdir/libethercat.so" 2>/dev/null \
                    || true
            fi
        done
        rm -f /tmp/libethercat.so
    fi
fi

# Refresh the runtime linker cache so the stub is found by the loader.
if command -v ldconfig &>/dev/null; then
    ldconfig 2>/dev/null || true
fi