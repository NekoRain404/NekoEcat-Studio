#pragma once

/* Shared-memory mirror contract for the Free Run process image.
 * Struct definitions live in the canonical, Qt-free header nekoecat_shm.h
 * which is shared verbatim with the pure-C client library. */

#include "nekoecat_shm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Compute size and init */
size_t computeProcessDataSize(const ShmMirrorEntry* entries, size_t count);

/* Mirror: copy inputs, apply validated outputs, publish atomically */
void mirrorToShm(const ShmMirrorContext* ctx);

#ifdef __cplusplus
}
#endif
