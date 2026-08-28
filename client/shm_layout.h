#pragma once

/* Client-side layout parsing for the NekoEcat shared-memory data plane.
 * Struct definitions live in the canonical, Qt-free header nekoecat_shm.h
 * shared verbatim with the C++ daemon. */

#include "nekoecat_shm.h"

#ifdef __cplusplus
extern "C" {
#endif

// Parse from JSON string (minimal, from shmInfo response)
bool layout_parse_from_shm_info_json(const char* json, struct ShmLayout* out);

// Get byte offset for entry; returns UINT32_MAX when the entry is not found.
// Callers must treat UINT32_MAX as "not found" and fail the operation.
uint32_t layout_byte_offset(const struct ShmLayout* layout, uint16_t slave, uint16_t index, uint8_t sub);

#ifdef __cplusplus
}
#endif
