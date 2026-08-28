#!/usr/bin/env python3
#
# nekoecat_client_example.py
#
# Python mmap + ctypes example for the NekoEcat real-time platform shared
# memory data plane (see docs/superpowers/specs/2026-07-07-realtime-platform-shm-client.md
# section 5).
#
# The FreeRun controller in `ecatd` publishes a double-buffered process-data
# region in POSIX shared memory named "/nekoecat_proc_0".  The region layout is:
#
#     [ ShmHeader ]
#     [ data buffer 0  (kMaxProcessDataSize bytes) ]
#     [ data buffer 1  (kMaxProcessDataSize bytes) ]
#
# This example:
#   - attaches to the SHM with mmap()
#   - decodes the header with ctypes
#   - optionally decodes process-data entries from a layout JSON (the JSON the
#     daemon returns for the `shmInfo` RPC)
#   - prints a few live cycles and decodes entries by offset
#
# Usage:
#   python3 examples/realtime/nekoecat_client_example.py [--layout layout.json] [--cycles N]
#
# For raw-SHM work a layout JSON is optional; without it the example prints the
# header + raw hex of the first bytes of the active buffer.  The C client
# library (client/nekoecat_client.c) offers the same functionality through the
# ctypes extensions shown at the end of this file.

import argparse
import ctypes
import json
import mmap
import os
import struct
import sys
import time


# ---------------------------------------------------------------------------
# Shared memory layout (must match FreeRunController::ShmHeader in
# apps/ecatd/FreeRunController.h and the kShmName constant).
# ---------------------------------------------------------------------------

SHM_NAME = "/nekoecat_proc_0"
K_MAX_PROCESS_DATA_SIZE = 4096

# POSIX shared objects live under /dev/shm.
SHM_PATH = "/dev/shm/nekoecat_proc_0"


class ShmHeader(ctypes.Structure):
    _fields_ = [
        ("version", ctypes.c_uint64),        # 0x00
        ("cycle_count", ctypes.c_uint64),    # 0x08
        ("timestamp_ns", ctypes.c_uint64),   # 0x10
        ("active_buffer", ctypes.c_uint32),  # 0x18  (0 or 1)
        ("data_size", ctypes.c_uint32),      # 0x1c
        ("layout_version", ctypes.c_uint32), # 0x20
        ("status_flags", ctypes.c_uint32),   # 0x24
        ("reserved", ctypes.c_uint32 * 4),   # 0x28 (16 bytes)
    ]


HEADER_SIZE = ctypes.sizeof(ShmHeader)  # 56 bytes


def open_shm(shm_name: str = SHM_NAME):
    """Attach to the POSIX shared memory object and return (mmap, fd).

    Accepts either a POSIX object name ("/nekoecat_proc_0") or a literal
    filesystem path ("/dev/shm/nekoecat_proc_0").
    """
    path = shm_name
    if path.startswith("/") and not path.startswith("/dev/shm"):
        path = "/dev/shm" + path
    if not os.path.exists(path):
        raise FileNotFoundError(
            f"Shared memory '{shm_name}' not found at '{path}'. Is ecatd "
            "running with an active FreeRun session?"
        )
    fd = os.open(path, os.O_RDWR)  # read-write so the example can write outputs too
    size = os.fstat(fd).st_size
    mm = mmap.mmap(fd, size, mmap.MAP_SHARED, mmap.PROT_READ | mmap.PROT_WRITE)
    return mm, fd


def read_header(mm):
    """Read the shared memory header into a ShmHeader ctypes struct."""
    header = ShmHeader()
    ctypes.memmove(ctypes.byref(header), bytes(mm[:HEADER_SIZE]), HEADER_SIZE)
    return header


def active_buffer_bytes(mm, header):
    """Return the bytes of the currently-active process-data buffer."""
    base = HEADER_SIZE + header.active_buffer * K_MAX_PROCESS_DATA_SIZE
    size = min(header.data_size, K_MAX_PROCESS_DATA_SIZE)
    return bytes(mm[base : base + size])


# ---------------------------------------------------------------------------
# Layout decoding
# ---------------------------------------------------------------------------

# Mapping of behaviour for a given bitLength + direction (same convention as
# FreeRunController::readEntryDecodedValue).
_UNPACKERS = {
    8: ("u", lambda b: int.from_bytes(b, "little")),
    16: ("u", lambda b: int.from_bytes(b, "little")),
    32: ("f", lambda b: struct.unpack("<f", b)[0]),
    64: ("f", lambda b: struct.unpack("<d", b)[0]),
}


def decode_entry(data: bytes, entry: dict):
    """Decode one layout entry from the active buffer.

    `entry` matches the shape of the daemon `shmInfo` RPC layout items:
    {slave, index, subindex, bitLength, direction, name, offset}.
    """
    offset = entry.get("offset")
    if offset is None or offset < 0:
        return None
    bl = entry.get("bitLength", 0)
    if bl in (8, 16):
        if offset + 2 > len(data):
            return None
        raw = data[offset : offset + 2]
        val = int.from_bytes(raw, "little")
        if bl == 8:
            val &= 0xFF
        return val
    if bl == 32:
        if offset + 4 > len(data):
            return None
        return struct.unpack("<f", data[offset : offset + 4])[0]
    if bl == 64:
        if offset + 8 > len(data):
            return None
        return struct.unpack("<d", data[offset : offset + 8])[0]
    return None


def load_layout(path_or_json):
    """Accept a layout JSON file path or an inline JSON string."""
    if path_or_json and os.path.exists(path_or_json):
        with open(path_or_json, "r", encoding="utf-8") as fh:
            info = json.load(fh)
    elif path_or_json:
        info = json.loads(path_or_json)
    else:
        info = {}
    return info


# ---------------------------------------------------------------------------
# Main demonstration
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description="NekoEcat real-time SHM reader")
    parser.add_argument("--layout", default=None,
                        help="path to a layout JSON (shmInfo output) for entry decoding")
    parser.add_argument("--cycles", type=int, default=3,
                        help="number of cycles to sample")
    parser.add_argument("--shm", default=SHM_PATH,
                        help="shared memory path (default %(default)s)")
    args = parser.parse_args()

    info = load_layout(args.layout)
    layout = info.get("layout", [])
    if layout:
        print(f"Decoding {len(layout)} process-data entries from layout.")

    mm, fd = open_shm(args.shm)
    try:
        prev_version = None
        for cycle in range(args.cycles):
            header = read_header(mm)
            data = active_buffer_bytes(mm, header)

            print(f"--- cycle {cycle + 1} ---")
            print(f"  version       : {header.version}")
            print(f"  cycle_count   : {header.cycle_count}")
            print(f"  timestamp_ns  : {header.timestamp_ns}")
            print(f"  active_buffer : {header.active_buffer}")
            print(f"  data_size     : {header.data_size}")
            print(f"  layout_version: {header.layout_version}")
            print(f"  status_flags  : {header.status_flags:#010x}")

            if layout and header.version != prev_version:
                for entry in layout:
                    val = decode_entry(data, entry)
                    name = entry.get("name", "")
                    slave = entry.get("slave")
                    idx = entry.get("index")
                    sub = entry.get("subindex")
                    direction = entry.get("direction", "")
                    print(f"    slave={slave} 0x{idx:04X}:{sub} "
                          f"[{direction}] {name}: {val}")
                prev_version = header.version
            else:
                print(f"  data[0:32]: {data[:32].hex()}")

            if cycle + 1 < args.cycles:
                # Wait for the next published cycle (version bump).
                deadline = time.monotonic() + 1.0
                while time.monotonic() < deadline:
                    h = read_header(mm)
                    if h.version != header.version and h.version != 0:
                        time.sleep(0.001)
                        break
                    time.sleep(0.001)
    finally:
        mm.close()
        os.close(fd)

    print("\nDone. For the higher-level API (start/attach/wait/read-by-index),")
    print("see client/nekoecat_client.h and examples in tests/unit/core.")


if __name__ == "__main__":
    main()


# ---------------------------------------------------------------------------
# Bonus: ctypes binding to the C client (client/nekoecat_client.c).
#
# Build the C client as a shared library, e.g.:
#   cc -shared -fPIC -O2 -o /tmp/libnekoecat_client.so \
#      client/nekoecat_client.c -lrt
#
# Then the following wrapper gives a Pythonic handle to the full client API.
# ---------------------------------------------------------------------------

def _inline_ctypes_client_binding():
    """Illustrative (not exercised by main()) ctypes wrapper for the C client."""
    import ctypes as _c

    class NekoEcatState:
        DISCONNECTED = 0
        CONNECTED = 1
        RUNNING = 2
        DATA_STALE = 3
        ERROR = 4

    def load_client(so_path="/tmp/libnekoecat_client.so"):
        lib = _c.CDLL(so_path)

        lib.nekoecat_client_create.restype = _c.c_void_p
        lib.nekoecat_client_create.argtypes = []
        lib.nekoecat_client_destroy.argtypes = [_c.c_void_p]

        lib.nekoecat_client_connect.argtypes = [
            _c.c_void_p, _c.c_char_p, _c.c_uint16, _c.c_char_p, _c.c_size_t]
        lib.nekoecat_client_connect.restype = _c.c_bool

        lib.nekoecat_client_start_freerun.argtypes = [
            _c.c_void_p, _c.c_char_p, _c.c_char_p, _c.c_size_t]
        lib.nekoecat_client_start_freerun.restype = _c.c_bool

        lib.nekoecat_client_wait_next_cycle.argtypes = [_c.c_void_p, _c.c_int64]
        lib.nekoecat_client_wait_next_cycle.restype = _c.c_bool

        lib.nekoecat_client_read_u16_by_index.argtypes = [
            _c.c_void_p, _c.c_uint16, _c.c_uint16, _c.c_uint8,
            _c.POINTER(_c.c_uint16)]
        lib.nekoecat_client_read_u16_by_index.restype = _c.c_bool

        # Python wrapper object -------------------------------------------------
        class Client:
            def __init__(self):
                self.lib = lib
                self.handle = lib.nekoecat_client_create()

            def __del__(self):
                if getattr(self, "handle", None):
                    self.lib.nekoecat_client_destroy(self.handle)

            def connect(self, host="127.0.0.1", port=5877):
                err = _c.create_string_buffer(256)
                ok = self.lib.nekoecat_client_connect(
                    self.handle, host.encode(), port, err, len(err))
                if not ok:
                    raise RuntimeError(err.value.decode())
                return ok

            def start_freerun(self):
                err = _c.create_string_buffer(256)
                if not self.lib.nekoecat_client_start_freerun(
                        self.handle, None, err, len(err)):
                    raise RuntimeError(err.value.decode())

            def wait_next_cycle(self, timeout_ns=1_000_000_000):
                return self.lib.nekoecat_client_wait_next_cycle(
                    self.handle, timeout_ns)

            def read_u16(self, slave, index, sub=0):
                out = _c.c_uint16()
                if not self.lib.nekoecat_client_read_u16_by_index(
                        self.handle, slave, index, sub, _c.byref(out)):
                    raise RuntimeError("read failed")
                return out.value

        return Client