#include "../../../apps/ecatd/freerun_shm_mirror.h"
#include "../../../client/nekoecat_client.h"
#include <cstddef>
#include <cstring>
#include <QtTest/QtTest>

// ABI drift guards: both the daemon and client headers must agree with the
// canonical layout, otherwise this TU fails to compile.
static_assert(sizeof(ShmHeader) == 56, "ShmHeader size");
static_assert(offsetof(ShmHeader, version) == 0, "version offset");
static_assert(offsetof(ShmHeader, active_buffer) == 24, "active_buffer offset");
static_assert(offsetof(ShmHeader, data_size) == 28, "data_size offset");
static_assert(offsetof(ShmHeader, status_flags) == 36, "status_flags offset");
static_assert(sizeof(ShmMirrorEntry) == 20, "ShmMirrorEntry size");

class FreeRunMirrorTest : public QObject {
    Q_OBJECT
private slots:
    void testMirrorRealAsserts() {
        uint8_t domain[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t shm0[16] = {0};
        uint8_t shm1[16] = {0};
        uint32_t active = 0;
        uint64_t version = 0;
        uint64_t cycle = 0;
        uint64_t tsField = 0;
        uint32_t flags = 0;
        uint32_t ignored = 0;

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
        QVERIFY(dsize == 4);

        ShmMirrorContext ctx{};
        ctx.domainData = domain;
        ctx.shmData[0] = shm0;
        ctx.shmData[1] = shm1;
        ctx.entries = entries;
        ctx.entryCount = 2;
        ctx.dataSize = dsize;
        ctx.timestampNs = 123456789;
        ctx.activeBuffer = &active;
        ctx.version = &version;
        ctx.cycleCount = &cycle;
        ctx.timestampNsField = &tsField;
        ctx.statusFlags = &flags;
        ctx.ignoredWrites = &ignored;

        memcpy(shm0, "\xAA\xBB\xCC\xDD", 4);

        for (int run = 0; run < 2; ++run) {
            printf("=== MIRROR RUN %d Start ===\n", run + 1);
            mirrorToShm(&ctx);
            QCOMPARE(active, (uint32_t)(run == 0 ? 1 : 0));
            QVERIFY(version > (uint64_t)run);
            QVERIFY(cycle > (uint64_t)run);
            QVERIFY(dsize == 4);
            QCOMPARE(shm1[2], (uint8_t)0xCC);
            QCOMPARE(domain[2], (uint8_t)0xCC);
            // Publish ordering: timestamp is written before version bumps.
            QCOMPARE(tsField, (uint64_t)123456789);
            QVERIFY((flags & NEKOECAT_FLAG_RUNNING) != 0);
            printf("=== MIRROR RUN %d Finished ===\n", run + 1);
        }
    }

    // Out-of-bounds client writes must be ignored (not copied) and counted.
    void testOutOfBoundsWriteIgnored() {
        uint8_t domain[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
        uint8_t shm0[8] = {0};
        uint8_t shm1[8] = {0};
        uint32_t active = 0;
        uint64_t version = 0;
        uint64_t cycle = 0;
        uint64_t tsField = 0;
        uint32_t flags = 0;
        uint32_t ignored = 0;

        ShmMirrorEntry entries[1];
        entries[0].slave = 0;
        entries[0].index = 0x6000;
        entries[0].sub = 1;
        entries[0].bitLength = 16;
        strncpy(entries[0].direction, "RxPDO", 7);
        entries[0].offset = 6; // 6 + 2 = 8 > dataSize 4

        ShmMirrorContext ctx{};
        ctx.domainData = domain;
        ctx.shmData[0] = shm0;
        ctx.shmData[1] = shm1;
        ctx.entries = entries;
        ctx.entryCount = 1;
        ctx.dataSize = 4;
        ctx.timestampNs = 1;
        ctx.activeBuffer = &active;
        ctx.version = &version;
        ctx.cycleCount = &cycle;
        ctx.timestampNsField = &tsField;
        ctx.statusFlags = &flags;
        ctx.ignoredWrites = &ignored;

        // Client writes to its active buffer (buffer 0) at the invalid offset.
        shm0[6] = 0xFF;
        shm0[7] = 0xFF;

        mirrorToShm(&ctx);
        QCOMPARE(ignored, (uint32_t)1);     // rejected and counted
        QCOMPARE(domain[6], (uint8_t)0x77); // domain untouched at oob offset
        QCOMPARE(shm1[6], (uint8_t)0);      // inactive buffer not overlayed
    }
};

QTEST_MAIN(FreeRunMirrorTest)
#include "free_run_mirror_test.moc"
