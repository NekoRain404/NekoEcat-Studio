/* Pure gating test for AC2+AC3 on same arena.
 * Includes BOTH the daemon mirror header and the client header so any ABI
 * drift between the two sides fails this TU at compile time. */
#include "../../../apps/ecatd/freerun_shm_mirror.h"
#include "nekoecat_client.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ABI drift guards: both sides must agree with the canonical layout.
static_assert(sizeof(ShmHeader) == 56, "ShmHeader size");
static_assert(offsetof(ShmHeader, version) == 0, "version offset");
static_assert(offsetof(ShmHeader, cycle_count) == 8, "cycle_count offset");
static_assert(offsetof(ShmHeader, timestamp_ns) == 16, "timestamp_ns offset");
static_assert(offsetof(ShmHeader, active_buffer) == 24, "active_buffer offset");
static_assert(offsetof(ShmHeader, data_size) == 28, "data_size offset");
static_assert(offsetof(ShmHeader, layout_version) == 32, "layout_version offset");
static_assert(offsetof(ShmHeader, status_flags) == 36, "status_flags offset");
static_assert(sizeof(ShmMirrorEntry) == 20, "ShmMirrorEntry size");
static_assert(sizeof(ShmLayoutEntry) == 20, "ShmLayoutEntry size");

int main(void) {
    ShmMirrorEntry entries[2];
    entries[0].slave = 0;
    entries[0].index = 0x6000;
    entries[0].sub = 0;
    entries[0].bitLength = 16;
    strncpy(entries[0].direction, "TxPDO", 7);
    entries[0].offset = 0;
    entries[1].slave = 0;
    entries[1].index = 0x6000;
    entries[1].sub = 1;
    entries[1].bitLength = 16;
    strncpy(entries[1].direction, "RxPDO", 7);
    entries[1].offset = 2;

    size_t dsize = computeProcessDataSize(entries, 2);
    if (dsize == 0)
        return 1;

    size_t arena_sz = sizeof(ShmHeader) + 2 * dsize;
    uint8_t* arena = (uint8_t*)calloc(1, arena_sz);
    if (!arena)
        return 1;

    ShmHeader* hdr = (ShmHeader*)arena;
    uint8_t* buf0 = arena + sizeof(ShmHeader);
    uint8_t* buf1 = buf0 + dsize;

    uint8_t domain[4] = {0x11, 0x22, 0x00, 0x00};

    uint32_t active = 0;
    uint64_t cyc = 0;
    uint64_t tsField = 0;
    uint32_t flags = 0;
    uint32_t ignored = 0;

    ShmMirrorContext ctx{};
    ctx.domainData = domain;
    ctx.shmData[0] = buf0;
    ctx.shmData[1] = buf1;
    ctx.entries = entries;
    ctx.entryCount = 2;
    ctx.dataSize = dsize;
    ctx.timestampNs = 42;
    ctx.activeBuffer = &active;
    ctx.version = &hdr->version;
    ctx.cycleCount = &cyc;
    ctx.timestampNsField = &tsField;
    ctx.statusFlags = &flags;
    ctx.ignoredWrites = &ignored;

    NekoEcatClient* c = nekoecat_client_create();
    if (!c) {
        free(arena);
        return 1;
    }

    /* Attach client view to the arena SHM (test helper for pure unit drive of mirror+client wait; no full RPC/daemon
     * needed) */
    hdr->version = 10;
    if (!nekoecat_client_test_attach_to_shm(c, NULL, arena, dsize)) {
        nekoecat_client_destroy(c);
        free(arena);
        return 1;
    }
    // helper sets last = ver-1

    mirrorToShm(&ctx); /* bumps hdr->ver to 11 */
    printf("after_m1 ver=%llu\n", (unsigned long long)hdr->version);
    int try1 = nekoecat_client_try_wait_next_cycle(c);
    printf("try1=%d\n", try1);
    int try_false = nekoecat_client_try_wait_next_cycle(c);
    printf("try_false=%d\n", try_false);
    if (try_false != 0) {
        nekoecat_client_destroy(c);
        free(arena);
        return 3;
    }

    mirrorToShm(&ctx); /* bumps to 12 */
    printf("after_m2 ver=%llu\n", (unsigned long long)hdr->version);
    int try2 = nekoecat_client_try_wait_next_cycle(c);
    printf("try2=%d\n", try2);
    if (try2 == 0) {
        nekoecat_client_destroy(c);
        free(arena);
        return 4;
    }
    if (nekoecat_client_get_current_version(c) != 12) {
        nekoecat_client_destroy(c);
        free(arena);
        return 5;
    }

    // The daemon publishes the RUNNING flag and the timestamp as part of the
    // atomic publish sequence.
    if ((flags & NEKOECAT_FLAG_RUNNING) == 0) {
        nekoecat_client_destroy(c);
        free(arena);
        return 6;
    }
    if (tsField != 42) {
        nekoecat_client_destroy(c);
        free(arena);
        return 7;
    }

    nekoecat_client_destroy(c);
    free(arena);
    printf("freerun_client_cycle: pure mirror + client attach_to + wait/try OK\n");
    return 0;
}
