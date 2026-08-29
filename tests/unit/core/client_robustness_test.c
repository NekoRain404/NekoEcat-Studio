/*
 * client_robustness_test.c
 *
 * Defensive-path tests for the pure-C client library (nekoecat_client.c),
 * compiled with NEKOECAT_TESTING so the test helpers are available.  No TCP or
 * real daemon is used -- every test drives the shipped SHM attach/read/write
 * logic against a caller-owned arena.
 *
 * Coverage:
 *   1. by-index read/write whose offset would overflow the data area -> false
 *      and last_error set (never reads/writes out of bounds).
 *   2. active_buffer forced to 2 (corrupt) -> get_process_data_ptr / reads /
 *      writes refuse instead of indexing out of bounds.
 *   3. unaligned layout offsets (u32 at byte offset 3) -> memcpy-based access
 *      produces correct values (no SIGBUS / no UB).
 *   4. unknown slave/index/sub in the layout -> read fails (sentinel path).
 *   5. layout JSON with >64 entries -> truncation is surfaced (truncated == 1)
 *      without overflow.
 */

#include "nekoecat_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int failures = 0;

static void fail(const char *what) {
    fprintf(stderr, "FAIL: %s\n", what);
    ++failures;
}

static void check_true(int cond, const char *what) {
    if (!cond) fail(what);
}

/* Build a ShmLayout with a single entry.  Returns it via out. */
static void make_single_entry_layout(struct ShmLayout *out, uint16_t index,
                                     uint8_t sub, uint32_t offset,
                                     uint8_t bitLength, uint32_t data_size) {
    memset(out, 0, sizeof(*out));
    out->data_size = data_size;
    out->count = 1;
    out->entries[0].slave = 0;
    out->entries[0].index = index;
    out->entries[0].sub = sub;
    out->entries[0].bitLength = bitLength;
    out->entries[0].offset = offset;
    strncpy(out->entries[0].direction, "RxPDO", sizeof(out->entries[0].direction) - 1);
    out->entries[0].direction[sizeof(out->entries[0].direction) - 1] = 0;
}

/* 1 + 5: out-of-bounds by-index access and oversized layout truncation. */
static void test_oob_by_index(void) {
    uint8_t arena[sizeof(ShmHeader) + 2 * 8] = {0};
    struct ShmLayout l;
    make_single_entry_layout(&l, 0x6000, 1, 7 /* 7 + 2 > data_size 8 */, 16, 8);

    NekoEcatClient *c = nekoecat_client_create();
    check_true(c != NULL, "1: create");
    if (!c) return;

    check_true(nekoecat_client_test_attach_to_shm(c, NULL, arena, 8),
               "1: attach");
    nekoecat_client_test_set_layout(c, &l);

    /* by-index write must be rejected (offset 7 + 2 > 8). */
    if (nekoecat_client_write_u16_by_index(c, 0, 0x6000, 1, 0xABCD)) {
        fail("1: oob by-index write must return false");
    }
    /* by-index read must be rejected too. */
    uint16_t rv = 0;
    if (nekoecat_client_read_u16_by_index(c, 0, 0x6000, 1, &rv)) {
        fail("1: oob by-index read must return false");
    }
    /* raw access at the same offset must be rejected. */
    if (nekoecat_client_write_u16_at(c, 7, 0xABCD)) {
        fail("1: oob raw write must return false");
    }
    if (nekoecat_client_read_u16_at(c, 7, &rv)) {
        fail("1: oob raw read must return false");
    }
    /* last_error must be populated. */
    const char *err = nekoecat_client_get_last_error(c);
    check_true(err != NULL && err[0] != '\0', "1: last_error populated");
    if (err && err[0]) {
        printf("1: oob error = '%s'\n", err);
    }

    /* The arena bytes at the oob offset must be untouched. */
    uint8_t *buf0 = arena + sizeof(ShmHeader);
    check_true(buf0[7] == 0 && buf0[8] == 0, "1: oob bytes untouched");

    nekoecat_client_destroy(c);
}

/* 2: corrupt active_buffer == 2 must refuse, not index OOB. */
static void test_corrupt_active_buffer(void) {
    uint8_t arena[sizeof(ShmHeader) + 2 * 16] = {0};
    struct ShmLayout l;
    make_single_entry_layout(&l, 0x6000, 1, 0, 32, 16);

    NekoEcatClient *c = nekoecat_client_create();
    if (!c) { fail("2: create"); return; }
    check_true(nekoecat_client_test_attach_to_shm(c, NULL, arena, 16), "2: attach");
    nekoecat_client_test_set_layout(c, &l);

    ShmHeader *hdr = (ShmHeader *)arena;
    nekoecat_shm_store(&hdr->active_buffer, 2, NEKOECAT_MO_RELAXED);

    /* get_process_data_ptr must not index out of bounds. */
    int buf = -1;
    uint8_t *p = nekoecat_client_get_process_data_ptr(c, &buf);
    if (p != NULL) {
        fail("2: get_process_data_ptr must return NULL for active_buffer=2");
    }

    /* reads / writes must refuse. */
    uint32_t v = 0;
    if (nekoecat_client_read_u32_at(c, 0, &v)) {
        fail("2: read with corrupt active_buffer must return false");
    }
    if (nekoecat_client_write_u32_at(c, 0, 0xDEADBEEF)) {
        fail("2: write with corrupt active_buffer must return false");
    }
    if (nekoecat_client_read_u32_by_index(c, 0, 0x6000, 1, &v)) {
        fail("2: by-index read with corrupt active_buffer must return false");
    }
    const char *err = nekoecat_client_get_last_error(c);
    if (err && strstr(err, "invalid active buffer") == NULL) {
        printf("2: unexpected error text: '%s'\n", err);
    }
    check_true(err != NULL && err[0] != '\0', "2: last_error populated");

    /* A 3rd/4th garbage value must also be refused. */
    nekoecat_shm_store(&hdr->active_buffer, 7, NEKOECAT_MO_RELAXED);
    if (nekoecat_client_get_process_data_ptr(c, NULL) != NULL) {
        fail("2: get_process_data_ptr must return NULL for active_buffer=7");
    }

    nekoecat_client_destroy(c);
}

/* 3: unaligned u32 at byte offset 3 -- memcpy-based access must work. */
static void test_unaligned_offsets(void) {
    uint8_t arena[sizeof(ShmHeader) + 2 * 32] = {0};
    struct ShmLayout l;
    make_single_entry_layout(&l, 0x6100, 1, 3 /* not 4-byte aligned */, 32, 32);

    NekoEcatClient *c = nekoecat_client_create();
    if (!c) { fail("3: create"); return; }
    check_true(nekoecat_client_test_attach_to_shm(c, NULL, arena, 32), "3: attach");
    nekoecat_client_test_set_layout(c, &l);

    /* by-index write + read at the unaligned offset. */
    if (!nekoecat_client_write_u32_by_index(c, 0, 0x6100, 1, 0xAABBCCDD)) {
        fail("3: unaligned by-index write failed");
    }
    uint32_t r = 0;
    if (!nekoecat_client_read_u32_by_index(c, 0, 0x6100, 1, &r)) {
        fail("3: unaligned by-index read failed");
    }
    if (r != 0xAABBCCDD) {
        printf("3: unaligned roundtrip got 0x%08x\n", r);
        fail("3: unaligned roundtrip value mismatch");
    }

    /* raw access at the unaligned offset. */
    if (!nekoecat_client_write_u32_at(c, 3, 0x11223344)) {
        fail("3: unaligned raw write failed");
    }
    if (!nekoecat_client_read_u32_at(c, 3, &r) || r != 0x11223344) {
        printf("3: unaligned raw roundtrip got 0x%08x\n", r);
        fail("3: unaligned raw roundtrip mismatch");
    }

    /* memory layout: little-endian bytes must sit at 3..6 (memcpy, not an
     * aligned u32 store). */
    uint8_t *buf0 = arena + sizeof(ShmHeader);
    if (buf0[3] != 0x44 || buf0[4] != 0x33 || buf0[5] != 0x22 || buf0[6] != 0x11) {
        printf("3: bytes at 3..6 = %02x %02x %02x %02x\n",
               buf0[3], buf0[4], buf0[5], buf0[6]);
        fail("3: unaligned bytes not memcpy'd into place");
    }

    /* unaligned float access too. */
    float f = 3.5f;
    if (!nekoecat_client_write_float_at(c, 5, f)) fail("3: unaligned float write failed");
    float fr = 0.0f;
    if (!nekoecat_client_read_float_at(c, 5, &fr)) fail("3: unaligned float read failed");
    if (fr != f) {
        printf("3: float got %f\n", fr);
        fail("3: unaligned float roundtrip mismatch");
    }

    nekoecat_client_destroy(c);
}

/* 4: unknown slave/index/sub must fail via the sentinel path. */
static void test_unknown_entries(void) {
    uint8_t arena[sizeof(ShmHeader) + 2 * 16] = {0};
    struct ShmLayout l;
    make_single_entry_layout(&l, 0x6000, 1, 0, 16, 16);

    NekoEcatClient *c = nekoecat_client_create();
    if (!c) { fail("4: create"); return; }
    check_true(nekoecat_client_test_attach_to_shm(c, NULL, arena, 16), "4: attach");
    nekoecat_client_test_set_layout(c, &l);

    uint16_t v = 0;
    if (nekoecat_client_read_u16_by_index(c, 0, 0xFFFF, 0, &v)) {
        fail("4: unknown index read must fail");
    }
    if (nekoecat_client_write_u16_by_index(c, 0, 0xFFFF, 0, 1)) {
        fail("4: unknown index write must fail");
    }
    /* known index, unknown subindex. */
    if (nekoecat_client_read_u16_by_index(c, 0, 0x6000, 9, &v)) {
        fail("4: unknown subindex read must fail");
    }
    /* unknown slave position. */
    if (nekoecat_client_read_u16_by_index(c, 3, 0x6000, 1, &v)) {
        fail("4: unknown slave read must fail");
    }
    const char *err = nekoecat_client_get_last_error(c);
    if (err && strstr(err, "not found") == NULL) {
        printf("4: unexpected error text: '%s'\n", err);
    }
    check_true(err != NULL && err[0] != '\0', "4: last_error populated");

    nekoecat_client_destroy(c);
}

/* 5: layout JSON with >64 entries truncates cleanly and is surfaced. */
static void test_layout_truncation(void) {
    char json[8192];
    size_t off = 0;
    const char *head = "{\"data_size\":1024,\"layout\":[";
    strcpy(json, head);
    off = strlen(head);

    const int total = 70;
    for (int i = 0; i < total; ++i) {
        int n = snprintf(json + off, sizeof(json) - off,
                         "%s{\"slave\":0,\"index\":%d,\"subindex\":1,\"bitLength\":8,\"offset\":%d,\"direction\":\"TxPDO\"}",
                         (i == 0) ? "" : ",", 0x7000 + i, i);
        if (n <= 0 || (size_t)n >= sizeof(json) - off) { fail("5: json build overflow"); return; }
        off += (size_t)n;
    }
    if (off + 2 >= sizeof(json)) { fail("5: json build overflow"); return; }
    strcpy(json + off, "]}");

    struct ShmLayout parsed;
    memset(&parsed, 0, sizeof(parsed));
    int ok = layout_parse_from_shm_info_json(json, &parsed);
    check_true(ok, "5: parse ok");
    check_true(parsed.count == NEKOECAT_SHM_LAYOUT_MAX_ENTRIES,
               "5: count capped at 64");
    check_true(parsed.truncated == 1, "5: truncation surfaced (truncated==1)");
    check_true(parsed.data_size == 1024, "5: data_size parsed");
    printf("5: parsed count=%zu truncated=%d (source had %d entries)\n",
           parsed.count, parsed.truncated, total);
}

/* Extra: operations on a client that was never attached must fail cleanly. */
static void test_not_attached(void) {
    NekoEcatClient *c = nekoecat_client_create();
    if (!c) { fail("extra: create"); return; }
    uint32_t v = 0;
    if (nekoecat_client_read_u32_at(c, 0, &v)) {
        fail("extra: read on detached client must fail");
    }
    if (nekoecat_client_read_u32_by_index(c, 0, 0x6000, 1, &v)) {
        fail("extra: by-index read on detached client must fail");
    }
    if (nekoecat_client_get_process_data_ptr(c, NULL) != NULL) {
        fail("extra: get_process_data_ptr on detached client must be NULL");
    }
    const char *err = nekoecat_client_get_last_error(c);
    check_true(err != NULL && err[0] != '\0', "extra: last_error populated");
    nekoecat_client_destroy(c);
}

int main(void) {
    printf("=== client_robustness_test ===\n");
    test_oob_by_index();
    test_corrupt_active_buffer();
    test_unaligned_offsets();
    test_unknown_entries();
    test_layout_truncation();
    test_not_attached();

    if (failures > 0) {
        fprintf(stderr, "client_robustness_test: %d FAILURE(S)\n", failures);
        return 1;
    }
    printf("client_robustness_test: ALL PASSED\n");
    return 0;
}
