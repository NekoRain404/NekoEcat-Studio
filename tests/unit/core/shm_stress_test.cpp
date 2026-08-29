// shm_stress_test.cpp
//
// SHM publish/consume consistency under concurrency.
//
// The pure mirror logic (apps/ecatd/freerun_shm_mirror.cpp) is compiled into
// this TU. A single publisher thread mutates a domain buffer and calls
// mirrorToShm(); 1..N reader threads replay the exact double-read protocol the
// C client uses (version -> active_buffer -> data -> version) against the same
// arena:
//
//   1. A reader must never accept a torn view: for every accepted snapshot the
//      per-cycle marker (a u64 at offset 0) must be consistent with the rest of
//      the buffer (the whole buffer is filled with (marker & 0xFF) each cycle),
//      i.e. no mixing of two adjacent cycles.
//   2. Reader-observed version is monotonic and eventually reaches the final
//      version (floor = baseVersion + cycles).
//   3. active_buffer is always 0 or 1 at every atomic read the reader makes.
//
// Time-bounded: N cycles of small memcpy + atomics complete in well under the
// 5s budget.

#include <pthread.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "../../../apps/ecatd/freerun_shm_mirror.h"

namespace {

struct SharedState {
    uint8_t *arena = nullptr;   // header + 2*stride
    ShmHeader *hdr = nullptr;
    uint8_t *data[2] = {nullptr, nullptr};
    uint8_t *domain = nullptr;  // publisher's "ecrt process image"
    size_t stride = 0;
    uint64_t baseVersion = 0;
    uint64_t cycles = 0;
    uint64_t targetFloor = 0;

    std::atomic<bool> stop{false};
};

struct ReaderStats {
    uint64_t reads = 0;          // accepted (v1 == v2) snapshots
    uint64_t retries = 0;        // snapshots rejected because version moved
    uint64_t maxV1 = 0;
    uint64_t minV1 = ~0ULL;
    bool monotonic = true;
    bool homogeneous = true;
    bool activeValid = true;
    bool markerInRange = true;

    // diagnostics of the first violation (0-initialised means "none")
    uint64_t badV1 = 0;
    uint64_t badMarker = 0;
    int badByteIndex = -1;
    int badByteValue = -1;
    uint32_t badActive = 99;
};

struct ReaderCtx {
    SharedState *s = nullptr;
    ReaderStats st;
};

// One full double-read protocol read, exactly like nekoecat_client_read_raw_at.
// Returns true when the snapshot was accepted (v1 == v2); false on contention.
static bool doubleRead(SharedState *s, uint8_t *snap, ReaderStats *st) {
    const uint64_t v1 = nekoecat_shm_load(&s->hdr->version, NEKOECAT_MO_ACQUIRE);
    const uint32_t b = nekoecat_shm_load(&s->hdr->active_buffer, NEKOECAT_MO_ACQUIRE);
    if (b > 1) {
        st->activeValid = false;
        st->badActive = b;
    }
    // Never index out of bounds even if the header is corrupt (b > 1).
    const uint8_t *src = (b <= 1) ? s->data[b] : s->data[0];
    std::memcpy(snap, src, s->stride);
    const uint64_t v2 = nekoecat_shm_load(&s->hdr->version, NEKOECAT_MO_ACQUIRE);
    if (v1 != v2) {
        ++st->retries;
        return false;
    }

    ++st->reads;
    if (v1 < st->minV1) st->minV1 = v1;
    if (v1 > st->maxV1) st->maxV1 = v1;

    // Per-cycle marker is stored little-endian at offset 0.
    uint64_t marker = 0;
    std::memcpy(&marker, snap, sizeof(marker));

    // Invariant: an accepted snapshot must come from a single publish, so the
    // generation of the active buffer is v1 or v1 + 1 (a publish can flip the
    // active buffer just before bumping the version).
    if (marker < v1 || marker > v1 + 1) {
        st->markerInRange = false;
        if (st->badByteIndex < 0) { st->badV1 = v1; st->badMarker = marker; }
    }

    // Homogeneity: every byte of the buffer (except marker bytes 1..7) must
    // equal (marker & 0xFF) -- any two-cycle mix would show a mismatch.
    const uint8_t expect = static_cast<uint8_t>(marker & 0xFF);
    for (size_t i = 0; i < s->stride; ++i) {
        if (i >= 1 && i <= 7) continue;  // high bytes of the marker u64
        if (snap[i] != expect) {
            st->homogeneous = false;
            if (st->badByteIndex < 0) {
                st->badByteIndex = static_cast<int>(i);
                st->badByteValue = snap[i];
                st->badV1 = v1;
                st->badMarker = marker;
            }
        }
    }
    return true;
}

static void *readerLoop(void *arg) {
    ReaderCtx *rc = static_cast<ReaderCtx *>(arg);
    SharedState *s = rc->s;

    std::vector<uint8_t> snap(s->stride);
    uint64_t prevV1 = 0;
    uint64_t totalAttempts = 0;
    const uint64_t kMaxAttempts = 4000000ULL;

    for (;;) {
        if (s->stop.load(std::memory_order_relaxed)) {
            // Flush the final version so the floor assertion is deterministic:
            // keep reading until we observe the last publish's version.
            if (rc->st.maxV1 >= s->targetFloor) break;
        }
        if (++totalAttempts > kMaxAttempts) break;  // safety, should never hit

        const bool accepted = doubleRead(s, snap.data(), &rc->st);
        if (!accepted) continue;

        // Accepted reads in one thread see non-decreasing versions.
        if (rc->st.reads > 1 && rc->st.maxV1 < prevV1) rc->st.monotonic = false;
        prevV1 = rc->st.maxV1;
    }
    return nullptr;
}

static void *publisherLoop(void *arg) {
    struct PubCtx {
        SharedState *s;
        uint64_t cycles;
    } *pc = static_cast<PubCtx *>(arg);
    SharedState *s = pc->s;

    ShmMirrorContext ctx{};
    ctx.domainData = s->domain;
    ctx.shmData[0] = s->data[0];
    ctx.shmData[1] = s->data[1];
    ctx.entries = nullptr;
    ctx.entryCount = 0;
    ctx.dataSize = s->stride;
    ctx.timestampNs = 1;
    ctx.activeBuffer = &s->hdr->active_buffer;
    ctx.version = &s->hdr->version;
    ctx.cycleCount = &s->hdr->cycle_count;
    ctx.timestampNsField = &s->hdr->timestamp_ns;
    ctx.statusFlags = &s->hdr->status_flags;
    ctx.ignoredWrites = &s->hdr->ignored_writes;

    uint64_t cycle = s->baseVersion;
    for (uint64_t i = 0; i < pc->cycles; ++i) {
        ++cycle;
        // Fill the domain with a homogeneous per-cycle pattern, then stamp the
        // cycle number (u64) at offset 0.
        std::memset(s->domain, static_cast<uint8_t>(cycle & 0xFF), s->stride);
        std::memcpy(s->domain, &cycle, sizeof(cycle));
        mirrorToShm(&ctx);
    }
    s->stop.store(true, std::memory_order_relaxed);
    return nullptr;
}

// Runs one scenario: stride bytes, `cycles` publishes, `nReaders` readers.
// Returns 0 on success.
int runScenario(const char *name, size_t stride, uint64_t cycles, int nReaders) {
    if (stride > NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE) stride = NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE;
    const uint64_t baseVersion = 1000;

    SharedState s;
    s.stride = stride;
    s.baseVersion = baseVersion;
    s.cycles = cycles;
    s.targetFloor = baseVersion + cycles;

    const size_t arenaSize = sizeof(ShmHeader) + 2 * stride;
    s.arena = static_cast<uint8_t *>(std::calloc(1, arenaSize));
    if (!s.arena) { std::fprintf(stderr, "%s: calloc failed\n", name); return 1; }
    s.hdr = reinterpret_cast<ShmHeader *>(s.arena);
    s.data[0] = s.arena + sizeof(ShmHeader);
    s.data[1] = s.data[0] + stride;
    s.domain = static_cast<uint8_t *>(std::calloc(1, stride));
    if (!s.domain) { std::free(s.arena); std::fprintf(stderr, "%s: domain calloc failed\n", name); return 1; }

    // Pre-seed a consistent "cycle 0" state so readers may start immediately:
    // version == baseVersion, active == 0, buffer 0 carries marker == baseVersion.
    const uint8_t fill0 = static_cast<uint8_t>(baseVersion & 0xFF);
    std::memset(s.data[0], fill0, stride);
    std::memcpy(s.data[0], &baseVersion, sizeof(baseVersion));
    std::memset(s.data[1], fill0, stride);
    std::memcpy(s.data[1], &baseVersion, sizeof(baseVersion));
    std::memset(s.domain, fill0, stride);
    std::memcpy(s.domain, &baseVersion, sizeof(baseVersion));
    nekoecat_shm_store(&s.hdr->version, baseVersion, NEKOECAT_MO_RELAXED);
    nekoecat_shm_store(&s.hdr->active_buffer, 0, NEKOECAT_MO_RELAXED);
    nekoecat_shm_store(&s.hdr->data_size, static_cast<uint32_t>(stride), NEKOECAT_MO_RELAXED);

    std::vector<pthread_t> tids(nReaders);
    std::vector<ReaderCtx> rcs(nReaders);
    for (int i = 0; i < nReaders; ++i) {
        rcs[i].s = &s;
        if (pthread_create(&tids[i], nullptr, readerLoop, &rcs[i]) != 0) {
            std::fprintf(stderr, "%s: pthread_create failed\n", name);
            s.stop.store(true, std::memory_order_relaxed);
            for (int j = 0; j < i; ++j) pthread_join(tids[j], nullptr);
            std::free(s.domain);
            std::free(s.arena);
            return 1;
        }
    }

    struct PubCtx { SharedState *s; uint64_t cycles; } pc{&s, cycles};
    pthread_t pthr;
    if (pthread_create(&pthr, nullptr, publisherLoop, &pc) != 0) {
        std::fprintf(stderr, "%s: pthread_create(publisher) failed\n", name);
        s.stop.store(true, std::memory_order_relaxed);
        for (int j = 0; j < nReaders; ++j) pthread_join(tids[j], nullptr);
        std::free(s.domain);
        std::free(s.arena);
        return 1;
    }
    pthread_join(pthr, nullptr);
    for (int i = 0; i < nReaders; ++i) pthread_join(tids[i], nullptr);

    // Merge + report.
    int rc = 0;
    std::printf("=== %s: stride=%zu cycles=%llu readers=%d ===\n",
                name, stride, (unsigned long long)cycles, nReaders);
    for (int i = 0; i < nReaders; ++i) {
        const ReaderStats &st = rcs[i].st;
        std::printf("  reader %d: reads=%llu retries=%llu v1=[%llu..%llu] floor=%llu monotonic=%d homogeneous=%d activeValid=%d markerInRange=%d\n",
                    i, (unsigned long long)st.reads, (unsigned long long)st.retries,
                    (unsigned long long)st.minV1, (unsigned long long)st.maxV1,
                    (unsigned long long)s.targetFloor, st.monotonic, st.homogeneous,
                    st.activeValid, st.markerInRange);
        if (st.reads == 0) {
            std::fprintf(stderr, "  reader %d: accepted no snapshots\n", i);
            rc = 1;
            continue;
        }
        if (!st.homogeneous) {
            std::fprintf(stderr, "  reader %d: TORN VIEW: marker=%llu v1=%llu first bad byte @%d = 0x%02x (expected 0x%02x)\n",
                         i, (unsigned long long)st.badMarker, (unsigned long long)st.badV1,
                         st.badByteIndex, st.badByteValue,
                         static_cast<int>(st.badMarker & 0xFF));
            rc = 1;
        }
        if (!st.markerInRange) {
            std::fprintf(stderr, "  reader %d: marker=%llu outside {v1, v1+1} with v1=%llu\n",
                         i, (unsigned long long)st.badMarker, (unsigned long long)st.badV1);
            rc = 1;
        }
        if (!st.activeValid) {
            std::fprintf(stderr, "  reader %d: invalid active_buffer=%u observed\n", i, st.badActive);
            rc = 1;
        }
        if (!st.monotonic) {
            std::fprintf(stderr, "  reader %d: version regressed\n", i);
            rc = 1;
        }
        if (st.maxV1 < s.targetFloor) {
            std::fprintf(stderr, "  reader %d: max version %llu below floor %llu\n",
                         i, (unsigned long long)st.maxV1, (unsigned long long)s.targetFloor);
            rc = 1;
        }
    }
    if (s.hdr->version != baseVersion + cycles) {
        std::fprintf(stderr, "  final version %llu != expected %llu\n",
                     (unsigned long long)s.hdr->version, (unsigned long long)(baseVersion + cycles));
        rc = 1;
    }

    std::free(s.domain);
    std::free(s.arena);
    return rc;
}

}  // namespace

int main() {
    int rc = 0;
    rc |= runScenario("single-reader-small", 256, 80000, 1);
    rc |= runScenario("three-readers-max", NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE, 50000, 3);
    rc |= runScenario("three-readers-tiny", 64, 150000, 3);

    if (rc == 0) {
        std::printf("shm_stress_test: ALL SCENARIOS PASSED (no torn views, monotonic versions, valid active_buffer)\n");
    } else {
        std::printf("shm_stress_test: FAILURES DETECTED\n");
    }
    return rc;
}
