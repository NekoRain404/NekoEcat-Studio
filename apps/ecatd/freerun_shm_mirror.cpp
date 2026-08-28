#include "freerun_shm_mirror.h"
#include <cstring>

size_t computeProcessDataSize(const ShmMirrorEntry* entries, size_t count) {
    if (!entries || count == 0) return 0;
    size_t max_end = 0;
    for (size_t i = 0; i < count; ++i) {
        size_t bytes = (entries[i].bitLength + 7) / 8;
        size_t end = (size_t)entries[i].offset + bytes;
        if (end > max_end) max_end = end;
    }
    return max_end;
}

void mirrorToShm(const ShmMirrorContext* ctx) {
    if (!ctx || !ctx->domainData) return;
    if (!ctx->shmData[0] || !ctx->shmData[1]) return;

    // Defense in depth: never trust dataSize past the shared arena cap.
    size_t dataSize = ctx->dataSize;
    if (dataSize > NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE) {
        dataSize = NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE;
    }
    if (dataSize == 0) return;

    // Validate the buffer index (a corrupt active_buffer must not index OOB).
    uint32_t active = nekoecat_shm_load(ctx->activeBuffer, NEKOECAT_MO_RELAXED);
    if (active > 1) active = 0;
    const size_t inactive = 1 - active;

    // 1) Copy the fresh inputs from the domain into the inactive buffer.
    std::memcpy(ctx->shmData[inactive], ctx->domainData, dataSize);

    // 2) Overlay validated client writes (RxPDO outputs) — last-write-wins.
    //    Out-of-bounds writes are ignored and counted for diagnostics.
    uint32_t ignored = 0;
    for (size_t i = 0; i < ctx->entryCount; ++i) {
        const ShmMirrorEntry& e = ctx->entries[i];
        if (std::strcmp(e.direction, "RxPDO") != 0) continue;
        size_t bl = (size_t)((e.bitLength + 7) / 8);
        if (bl == 0) continue;
        // A client must never drive a copy outside the shared data area.
        if ((size_t)e.offset > dataSize || bl > dataSize - (size_t)e.offset) {
            ++ignored;
            continue;
        }
        // Overlay client writes (from the active/client buffer) into the inactive shm
        std::memcpy(ctx->shmData[inactive] + e.offset, ctx->shmData[active] + e.offset, bl);
        // CRITICAL: apply the client output write from SHM back to domainData so the next ecrt send uses it
        std::memcpy(ctx->domainData + e.offset, ctx->shmData[active] + e.offset, bl);
    }
    if (ignored > 0 && ctx->ignoredWrites) {
        nekoecat_shm_fetch_add(ctx->ignoredWrites, ignored, NEKOECAT_MO_RELAXED);
    }

    // 3) Publish. Order matters for weakly-ordered CPUs:
    //    timestamp + status + cycle_count are relaxed; active_buffer is a
    //    release store; version is the FINAL release store so a reader that
    //    acquires version is guaranteed to observe everything above it.
    if (ctx->timestampNsField) {
        nekoecat_shm_store(ctx->timestampNsField, ctx->timestampNs, NEKOECAT_MO_RELAXED);
    }
    if (ctx->statusFlags) {
        nekoecat_shm_fetch_or(ctx->statusFlags, NEKOECAT_FLAG_RUNNING, NEKOECAT_MO_RELAXED);
    }
    if (ctx->cycleCount) {
        nekoecat_shm_fetch_add(ctx->cycleCount, 1, NEKOECAT_MO_RELAXED);
    }
    nekoecat_shm_store(ctx->activeBuffer, (uint32_t)inactive, NEKOECAT_MO_RELEASE);
    if (ctx->version) {
        uint64_t v = nekoecat_shm_load(ctx->version, NEKOECAT_MO_RELAXED);
        nekoecat_shm_store(ctx->version, v + 1, NEKOECAT_MO_RELEASE);
    }
}
