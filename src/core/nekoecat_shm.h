#pragma once
/*
 * nekoecat_shm.h
 *
 * Canonical, Qt-free shared-memory data-plane ABI for NekoEcat.
 *
 * This header is the SINGLE source of truth for the on-wire / on-shm layout
 * shared between the ecatd daemon (C++20/Qt6) and the nekoecat_client pure-C
 * library:
 *
 *   - ShmHeader        : mmap'd header laid out identically on both sides
 *   - ShmMirrorEntry   : daemon-side PDO entry description
 *   - ShmMirrorContext : everything mirrorToShm() needs per cycle
 *   - ShmLayoutEntry / ShmLayout : client-side parsed layout
 *   - atomic helpers   : cross-process field access (C11/GCC __atomic builtins)
 *   - compile-time layout assertions so ABI drift fails the build
 *
 * Do NOT add Qt includes here; this header must compile as plain C.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Layout constants                                                    */
/* ------------------------------------------------------------------ */

#define NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE 4096u
#define NEKOECAT_SHM_LAYOUT_MAX_ENTRIES 64u
#define NEKOECAT_SHM_LAYOUT_VERSION 1u

/* status_flags bits published by the daemon as part of each atomic publish */
#define NEKOECAT_FLAG_RUNNING 0x00000001u

/* ------------------------------------------------------------------ */
/* Shared memory header                                                */
/*                                                                     */
/* All fields accessed across processes MUST go through the atomic      */
/* helpers below; keep this struct POD with identical layout on both    */
/* sides.  version is bumped (release) as the final step of every       */
/* publish; readers acquire it first and re-check it after reading the  */
/* data buffer (classic double-read) to guarantee no torn views.        */
/* ------------------------------------------------------------------ */

typedef struct ShmHeader {
    uint64_t version;        /* bumped at the end of every publish       */
    uint64_t cycle_count;    /* completed cycles                         */
    uint64_t timestamp_ns;   /* monotonic timestamp of current publish   */
    uint32_t active_buffer;  /* 0 or 1                                   */
    uint32_t data_size;      /* actual process-data size in bytes (stride) */
    uint32_t layout_version; /* layout ABI version                       */
    uint32_t status_flags;   /* NEKOECAT_FLAG_*                          */
    uint32_t ignored_writes; /* daemon count of rejected out-of-bounds writes */
    uint32_t reserved[3];
} ShmHeader;

/* ------------------------------------------------------------------ */
/* Daemon-side mirror structures                                       */
/* ------------------------------------------------------------------ */

/* One PDO entry mirrored into shared memory (daemon side). */
typedef struct ShmMirrorEntry {
    uint16_t slave;
    uint16_t index;
    uint8_t sub;
    uint8_t bitLength;
    char direction[8];       /* "RxPDO" or "TxPDO" */
    uint32_t offset;
} ShmMirrorEntry;

/* Everything mirrorToShm() needs; the daemon fills this once per cycle. */
typedef struct ShmMirrorContext {
    uint8_t* domainData;         /* ecrt process-data image              */
    uint8_t* shmData[2];         /* shm buffers; stride == dataSize      */
    ShmMirrorEntry* entries;     /* precomputed layout                   */
    size_t entryCount;
    size_t dataSize;             /* clamped process-data size            */
    uint64_t timestampNs;        /* value to publish into timestamp_ns   */
    uint32_t* activeBuffer;      /* -> ShmHeader.active_buffer           */
    uint64_t* version;           /* -> ShmHeader.version                 */
    uint64_t* cycleCount;        /* -> ShmHeader.cycle_count             */
    uint64_t* timestampNsField;  /* -> ShmHeader.timestamp_ns            */
    uint32_t* statusFlags;       /* -> ShmHeader.status_flags            */
    uint32_t* ignoredWrites;     /* -> ShmHeader.ignored_writes          */
} ShmMirrorContext;

/* ------------------------------------------------------------------ */
/* Client-side parsed layout                                           */
/* ------------------------------------------------------------------ */

typedef struct ShmLayoutEntry {
    uint16_t slave;
    uint16_t index;
    uint8_t sub;
    uint8_t bitLength;
    char direction[8];
    uint32_t offset;
} ShmLayoutEntry;

typedef struct ShmLayout {
    ShmLayoutEntry entries[NEKOECAT_SHM_LAYOUT_MAX_ENTRIES];
    size_t count;
    uint32_t data_size;
    int truncated;  /* set when the source had more entries than the cap */
} ShmLayout;

/* ------------------------------------------------------------------ */
/* Cross-process atomic helpers                                        */
/*                                                                     */
/* Lock-free C11/GCC __atomic builtins; guaranteed lock-free for these  */
/* integer widths on every supported platform (x86-64 / arm64).        */
/* ------------------------------------------------------------------ */

#define NEKOECAT_MO_RELAXED __ATOMIC_RELAXED
#define NEKOECAT_MO_ACQUIRE __ATOMIC_ACQUIRE
#define NEKOECAT_MO_RELEASE __ATOMIC_RELEASE

#define nekoecat_shm_load(ptr, mo)          __atomic_load_n((ptr), (mo))
#define nekoecat_shm_store(ptr, val, mo)    __atomic_store_n((ptr), (val), (mo))
#define nekoecat_shm_fetch_add(ptr, val, mo) __atomic_fetch_add((ptr), (val), (mo))
#define nekoecat_shm_fetch_or(ptr, val, mo)  __atomic_fetch_or((ptr), (val), (mo))
#define nekoecat_shm_fetch_and(ptr, val, mo) __atomic_fetch_and((ptr), (val), (mo))

/* ------------------------------------------------------------------ */
/* Mirror logic (pure C, no Qt)                                        */
/* ------------------------------------------------------------------ */

/* Compute the process-data size as max(entry.offset + bytes). */
size_t computeProcessDataSize(const ShmMirrorEntry* entries, size_t count);

/*
 * One cycle of the shared-memory mirror:
 *   1. copy the fresh domain data into the inactive buffer
 *   2. overlay validated client writes (RxPDO) from the active buffer
 *      back into the inactive buffer AND into domainData (last-write-wins)
 *   3. publish atomically: timestamp -> status_flags -> cycle_count ->
 *      active_buffer (release) -> version (release)
 * Out-of-bounds client writes are ignored and counted into ignoredWrites.
 */
void mirrorToShm(const ShmMirrorContext* ctx);

/* ------------------------------------------------------------------ */
/* Compile-time layout assertions (both languages)                     */
/* ------------------------------------------------------------------ */

#if defined(__cplusplus)
#define NEKOECAT_SHM_STATIC_ASSERT(cond, msg) static_assert((cond), msg)
#else
#define NEKOECAT_SHM_STATIC_ASSERT(cond, msg) _Static_assert((cond), msg)
#endif

NEKOECAT_SHM_STATIC_ASSERT(sizeof(ShmHeader) == 56, "ShmHeader must be 56 bytes");
NEKOECAT_SHM_STATIC_ASSERT(offsetof(ShmHeader, version) == 0, "version must be first");
NEKOECAT_SHM_STATIC_ASSERT(offsetof(ShmHeader, cycle_count) == 8, "cycle_count offset");
NEKOECAT_SHM_STATIC_ASSERT(offsetof(ShmHeader, timestamp_ns) == 16, "timestamp_ns offset");
NEKOECAT_SHM_STATIC_ASSERT(offsetof(ShmHeader, active_buffer) == 24, "active_buffer offset");
NEKOECAT_SHM_STATIC_ASSERT(offsetof(ShmHeader, data_size) == 28, "data_size offset");
NEKOECAT_SHM_STATIC_ASSERT(offsetof(ShmHeader, layout_version) == 32, "layout_version offset");
NEKOECAT_SHM_STATIC_ASSERT(offsetof(ShmHeader, status_flags) == 36, "status_flags offset");
NEKOECAT_SHM_STATIC_ASSERT(offsetof(ShmHeader, ignored_writes) == 40, "ignored_writes offset");
NEKOECAT_SHM_STATIC_ASSERT(offsetof(ShmHeader, reserved) == 44, "reserved offset");
NEKOECAT_SHM_STATIC_ASSERT(sizeof(ShmMirrorEntry) == 20, "ShmMirrorEntry must be 20 bytes");
NEKOECAT_SHM_STATIC_ASSERT(sizeof(ShmLayoutEntry) == 20, "ShmLayoutEntry must be 20 bytes");

#ifdef __cplusplus
}
#endif
