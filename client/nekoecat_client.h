#pragma once

/*
 * nekoecat_client.h
 *
 * Lightweight C99 client library for NekoEcat real-time platform.
 *
 * Provides unified access to:
 *   - JSON-RPC control plane (start/stop, layout, etc.)
 *   - Shared memory data plane (high-performance process data)
 *
 * Designed for external real-time programs (C/C++, Python via ctypes, etc.).
 *
 * v1 focus: FreeRun / cyclic process data only.
 *
 * The shared-memory ABI (ShmHeader and friends) lives in the canonical,
 * Qt-free header nekoecat_shm.h so the daemon and client can never drift.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "shm_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================== Opaque Types ====================== */

typedef struct NekoEcatClient NekoEcatClient;

/* ====================== Status / State ====================== */

typedef enum {
    NEKOECAT_STATE_DISCONNECTED = 0,
    NEKOECAT_STATE_CONNECTED,
    NEKOECAT_STATE_RUNNING,
    NEKOECAT_STATE_DATA_STALE,
    NEKOECAT_STATE_ERROR,
} NekoEcatState;

/* ====================== Client Lifecycle ====================== */

NekoEcatClient* nekoecat_client_create(void);
void            nekoecat_client_destroy(NekoEcatClient* client);

/* Connect to ecatd (host may be a hostname or an IP literal). */
bool nekoecat_client_connect(NekoEcatClient* client,
                             const char* host,
                             uint16_t port,
                             char* error_buf,
                             size_t error_buf_size);

/* Disconnect */
void nekoecat_client_disconnect(NekoEcatClient* client);

/* ====================== FreeRun Control ====================== */

/*
 * High-level start.
 * mapping_json can be NULL to use current bus mapping (recommended for most cases).
 */
bool nekoecat_client_start_freerun(NekoEcatClient* client,
                                   const char* mapping_json,   /* optional */
                                   char* error_buf,
                                   size_t error_buf_size);

/* Stop FreeRun */
bool nekoecat_client_stop_freerun(NekoEcatClient* client,
                                  char* error_buf,
                                  size_t error_buf_size);

/*
 * Attach to an already running FreeRun instance.
 * If layout_json is NULL, client will fetch the layout (and shm name/size)
 * via RPC. If layout_json is provided it is used directly and the RPC fetch
 * is skipped (the JSON must contain shm_name, data_size and layout fields).
 */
bool nekoecat_client_attach(NekoEcatClient* client,
                            const char* layout_json,   /* optional */
                            char* error_buf,
                            size_t error_buf_size);

/* ====================== Cycle Synchronization ====================== */

/*
 * Blocking wait for next cycle (returns true if new data is available).
 * timeout_ns must be >= 0; negative values are rejected.
 */
bool nekoecat_client_wait_next_cycle(NekoEcatClient* client,
                                     int64_t timeout_ns);

/* Non-blocking: returns true if a new cycle is available since last call */
bool nekoecat_client_try_wait_next_cycle(NekoEcatClient* client);

/* ====================== Data Access (High Level) ====================== */

/* Read helpers (by slave position + SDO/PDO index). All reads go through the
 * double-read consistency protocol and validate bounds. */
bool nekoecat_client_read_u8_by_index (NekoEcatClient* client, uint16_t slave_pos, uint16_t index, uint8_t sub,  uint8_t*  out_val);
bool nekoecat_client_read_u16_by_index(NekoEcatClient* client, uint16_t slave_pos, uint16_t index, uint8_t sub,  uint16_t* out_val);
bool nekoecat_client_read_u32_by_index(NekoEcatClient* client, uint16_t slave_pos, uint16_t index, uint8_t sub,  uint32_t* out_val);
bool nekoecat_client_read_u64_by_index(NekoEcatClient* client, uint16_t slave_pos, uint16_t index, uint8_t sub,  uint64_t* out_val);
bool nekoecat_client_read_float_by_index(NekoEcatClient* client, uint16_t slave_pos, uint16_t index, uint8_t sub, float* out_val);

/* Write helpers (outputs) - writes go directly to SHM (last-write-wins) */
bool nekoecat_client_write_u8_by_index (NekoEcatClient* client, uint16_t slave_pos, uint16_t index, uint8_t sub,  uint8_t  value);
bool nekoecat_client_write_u16_by_index(NekoEcatClient* client, uint16_t slave_pos, uint16_t index, uint8_t sub,  uint16_t value);
bool nekoecat_client_write_u32_by_index(NekoEcatClient* client, uint16_t slave_pos, uint16_t index, uint8_t sub,  uint32_t value);
bool nekoecat_client_write_u64_by_index(NekoEcatClient* client, uint16_t slave_pos, uint16_t index, uint8_t sub,  uint64_t value);
bool nekoecat_client_write_float_by_index(NekoEcatClient* client, uint16_t slave_pos, uint16_t index, uint8_t sub, float value);

/* ====================== Raw Access (Maximum Performance) ====================== */

uint8_t* nekoecat_client_get_process_data_ptr(NekoEcatClient* client, int* active_buffer_out);
uint64_t nekoecat_client_get_current_version(NekoEcatClient* client);
uint32_t nekoecat_client_get_data_size(NekoEcatClient* client);

/*
 * Raw offset-based helpers. off is relative to the start of the active data
 * buffer. Every access validates 0 <= off and off + width <= data_size and
 * returns false (with last_error set) on violation.
 */
bool nekoecat_client_read_raw_at (NekoEcatClient* client, uint32_t off, void* buf, uint32_t nbytes);
bool nekoecat_client_write_raw_at(NekoEcatClient* client, uint32_t off, const void* buf, uint32_t nbytes);
bool nekoecat_client_read_u8_at  (NekoEcatClient* client, uint32_t off, uint8_t*  out);
bool nekoecat_client_read_u16_at (NekoEcatClient* client, uint32_t off, uint16_t* out);
bool nekoecat_client_read_u32_at (NekoEcatClient* client, uint32_t off, uint32_t* out);
bool nekoecat_client_read_u64_at (NekoEcatClient* client, uint32_t off, uint64_t* out);
bool nekoecat_client_read_float_at(NekoEcatClient* client, uint32_t off, float* out);
bool nekoecat_client_write_u8_at  (NekoEcatClient* client, uint32_t off, uint8_t  value);
bool nekoecat_client_write_u16_at (NekoEcatClient* client, uint32_t off, uint16_t value);
bool nekoecat_client_write_u32_at (NekoEcatClient* client, uint32_t off, uint32_t value);
bool nekoecat_client_write_u64_at (NekoEcatClient* client, uint32_t off, uint64_t value);
bool nekoecat_client_write_float_at(NekoEcatClient* client, uint32_t off, float value);

/* ====================== Status ====================== */

NekoEcatState nekoecat_client_get_state(NekoEcatClient* client);
int64_t       nekoecat_client_get_cycle_count(NekoEcatClient* client);
const char*   nekoecat_client_get_last_error(NekoEcatClient* client);

/*
 * Test helpers — compiled only when NEKOECAT_TESTING is defined. They simulate
 * the daemon side of the SHM arena for unit tests and are never shipped.
 */
#ifdef NEKOECAT_TESTING
void nekoecat_client_test_set_layout(NekoEcatClient* c, const struct ShmLayout* l);
bool nekoecat_client_test_attach_to_shm(NekoEcatClient* client, const char* layout_json, uint8_t* shm_base, uint32_t dsize);
void nekoecat_client_test_setup_shm(NekoEcatClient* c, uint8_t* shm_base, uint32_t dsize, uint64_t initial_version);
#endif /* NEKOECAT_TESTING */

#ifdef __cplusplus
}
#endif
