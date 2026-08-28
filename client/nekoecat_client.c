/*
 * nekoecat_client.c
 * Minimal standalone pure-C implementation for NekoEcatClient (no Qt).
 * Depends only on POSIX (sockets, shm, time) + std C.
 * For v1: supports connect, basic RPC for freerun, SHM attach, wait, read/write.
 *
 * The shared-memory ABI lives in the canonical header nekoecat_shm.h so the
 * client and the ecatd daemon can never drift apart. All cross-process header
 * fields are accessed with acquire/release atomics; reads use the classic
 * double-read protocol for torn-view protection on weakly-ordered CPUs.
 */

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
#include "nekoecat_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#include "shm_layout.h"

struct NekoEcatClient {
    int sock;
    int shm_fd;
    void* shm_ptr;
    size_t shm_size;
    struct ShmHeader* hdr;  /* pointer into the mapped shared memory */
    uint8_t* data[2];
    int attached;
    int state_error;        /* sticky error state (e.g. layout version mismatch) */
    uint32_t mapped_data_size;      /* stride used when attaching */
    uint32_t known_layout_version;  /* layout version the layout was fetched with */
    char last_err[256];
    uint64_t last_version;
    struct ShmLayout layout;
};

static void set_error(NekoEcatClient* c, const char* fmt, ...) {
    if (!c) return;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(c->last_err, sizeof(c->last_err), fmt, ap);
    va_end(ap);
}

static void errcpy(char* err, size_t errsz, const char* msg) {
    if (err && errsz > 0) {
        snprintf(err, errsz, "%s", msg);
    }
}

static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

NekoEcatClient* nekoecat_client_create(void) {
    NekoEcatClient* c = (NekoEcatClient*)calloc(1, sizeof(NekoEcatClient));
    if (c) c->sock = -1;
    return c;
}

void nekoecat_client_destroy(NekoEcatClient* client) {
    if (!client) return;
    nekoecat_client_disconnect(client);
    free(client);
}

static int send_line(int sock, const char* json) {
    char buf[1024];
    int len = snprintf(buf, sizeof(buf), "%s\n", json);
    if (len <= 0 || len >= (int)sizeof(buf)) return -1;
    return send(sock, buf, (size_t)len, 0) == len ? 0 : -1;
}

/* Read one newline-delimited line. If the line does not terminate within cap
 * bytes the stream is desynced — report an error instead of truncating. */
static int recv_line(int sock, char* buf, size_t cap) {
    if (!buf || cap == 0) return -1;
    size_t pos = 0;
    while (pos < cap) {
        char ch;
        ssize_t r = recv(sock, &ch, 1, 0);
        if (r <= 0) return -1;  /* EOF or error */
        if (ch == '\n') {
            buf[pos] = 0;
            return 0;
        }
        buf[pos++] = ch;
    }
    return -1;  /* no newline within cap */
}

bool nekoecat_client_connect(NekoEcatClient* client, const char* host, uint16_t port, char* err, size_t errsz) {
    if (!client) return false;
    client->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sock < 0) {
        set_error(client, "socket fail");
        errcpy(err, errsz, "socket fail");
        return false;
    }

    /* getaddrinfo gives us hostname support ("localhost") and proper errors. */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    char portstr[16];
    snprintf(portstr, sizeof(portstr), "%u", (unsigned)port);
    const char* h = host ? host : "127.0.0.1";
    struct addrinfo* res = NULL;
    const int rc = getaddrinfo(h, portstr, &hints, &res);
    if (rc != 0 || !res) {
        set_error(client, "getaddrinfo failed for %s", h);
        errcpy(err, errsz, "getaddrinfo failed");
        close(client->sock);
        client->sock = -1;
        return false;
    }

    int ok = 0;
    for (struct addrinfo* ai = res; ai; ai = ai->ai_next) {
        if (connect(client->sock, ai->ai_addr, (socklen_t)ai->ai_addrlen) == 0) {
            ok = 1;
            break;
        }
    }
    freeaddrinfo(res);

    if (!ok) {
        set_error(client, "connect fail");
        errcpy(err, errsz, "connect fail");
        close(client->sock);
        client->sock = -1;
        return false;
    }
    return true;
}

void nekoecat_client_disconnect(NekoEcatClient* client) {
    if (!client) return;
    if (client->shm_ptr) {
        munmap(client->shm_ptr, client->shm_size);
        client->shm_ptr = NULL;
    }
    if (client->shm_fd >= 0) {
        close(client->shm_fd);
        client->shm_fd = -1;
    }
    if (client->sock >= 0) {
        close(client->sock);
        client->sock = -1;
    }
    client->attached = 0;
    client->hdr = NULL;
    client->data[0] = client->data[1] = NULL;
    client->shm_size = 0;
}

static bool rpc_call(NekoEcatClient* c, const char* method, const char* params, char* resp, size_t rcap) {
    char req[512];
    snprintf(req, sizeof(req), "{\"id\":\"1\",\"method\":\"%s\",\"params\":%s}", method, params ? params : "{}");
    if (send_line(c->sock, req) < 0) {
        set_error(c, "rpc send failed (%s)", method);
        return false;
    }
    if (recv_line(c->sock, resp, rcap) < 0) {
        set_error(c, "rpc recv failed (%s)", method);
        return false;
    }
    if (strstr(resp, "\"ok\":true") == NULL) {
        set_error(c, "rpc %s rejected", method);
        return false;
    }
    return true;
}

bool nekoecat_client_start_freerun(NekoEcatClient* client, const char* mapping_json, char* err, size_t esz) {
    char params[256];
    snprintf(params, sizeof(params), "{\"mapping\":%s}", mapping_json ? mapping_json : "null");
    char resp[1024];
    if (!rpc_call(client, "freeRunStart", params, resp, sizeof(resp))) {
        errcpy(err, esz, nekoecat_client_get_last_error(client));
        return false;
    }
    // after start, auto attach
    return nekoecat_client_attach(client, NULL, err, esz);
}

bool nekoecat_client_stop_freerun(NekoEcatClient* client, char* err, size_t esz) {
    char resp[256];
    const bool ok = rpc_call(client, "freeRunStop", "{}", resp, sizeof(resp));
    if (!ok) {
        errcpy(err, esz, nekoecat_client_get_last_error(client));
    }
    return ok;
}

bool nekoecat_client_attach(NekoEcatClient* client, const char* layout_json, char* err, size_t esz) {
    if (!client) return false;
    // Robustness for real use: auto-connect to default if not connected (attach-to-running common case)
    // Configurable via env for real deployments (NEKOECAT_DAEMON_HOST / NEKOECAT_DAEMON_PORT)
    if (client->sock < 0) {
        const char* env_host = getenv("NEKOECAT_DAEMON_HOST");
        const char* env_port = getenv("NEKOECAT_DAEMON_PORT");
        const char* host = env_host ? env_host : "127.0.0.1";
        uint16_t port = env_port ? (uint16_t)atoi(env_port) : 5877u;
        if (!nekoecat_client_connect(client, host, port, err, esz)) {
            return false;
        }
    }

    char shmname[64] = "/nekoecat_proc_0";
    uint32_t dsize = NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE;
    uint32_t layout_version = 0;
    struct ShmLayout parsed;
    memset(&parsed, 0, sizeof(parsed));

    if (layout_json && layout_json[0] != '\0') {
        // Caller-provided layout: parse it directly and skip the RPC fetch.
        if (!layout_parse_from_shm_info_json(layout_json, &parsed)) {
            set_error(client, "attach: invalid layout_json");
            errcpy(err, esz, "attach: invalid layout_json");
            return false;
        }
        dsize = parsed.data_size ? parsed.data_size : NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE;
        const char* lv = strstr(layout_json, "\"layout_version\":");
        if (lv) sscanf(lv + 17, "%u", &layout_version);
        const char* nm = strstr(layout_json, "\"shm_name\":\"");
        if (nm) sscanf(nm + 12, "%63[^\"]", shmname);
    } else {
        char resp[4096];
        if (!rpc_call(client, "freeRunShmInfo", "{}", resp, sizeof(resp))) {
            errcpy(err, esz, nekoecat_client_get_last_error(client));
            return false;
        }
        // robust: extract inner result if wrapped {"ok":true,"result":{...}}
        const char* json = resp;
        const char* res = strstr(resp, "\"result\":");
        if (res) {
            const char* start = strchr(res, '{');
            if (start) json = start;
        }
        if (!layout_parse_from_shm_info_json(json, &parsed)) {
            set_error(client, "attach: failed to parse layout from shmInfo");
            errcpy(err, esz, "attach: failed to parse layout from shmInfo");
            return false;
        }
        dsize = parsed.data_size ? parsed.data_size : NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE;
        const char* np = strstr(json, "\"shm_name\":\"");
        if (np) sscanf(np + 12, "%63[^\"]", shmname);
        const char* lv = strstr(json, "\"layout_version\":");
        if (lv) sscanf(lv + 17, "%u", &layout_version);
    }

    if (dsize == 0 || dsize > NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE) {
        dsize = NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE;
    }

    // The daemon maps both buffers with the ACTUAL data_size stride, so the
    // client must use the identical stride (data[1] == data[0] + dsize).
    client->shm_fd = shm_open(shmname, O_RDWR, 0600);
    if (client->shm_fd < 0) {
        set_error(client, "shm_open fail (%s)", shmname);
        errcpy(err, esz, "shm_open fail");
        return false;
    }
    client->shm_size = sizeof(ShmHeader) + 2 * (size_t)dsize;
    client->shm_ptr = mmap(0, client->shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, client->shm_fd, 0);
    if (client->shm_ptr == MAP_FAILED) {
        close(client->shm_fd);
        client->shm_fd = -1;
        client->shm_ptr = NULL;
        set_error(client, "mmap fail");
        errcpy(err, esz, "mmap fail");
        return false;
    }
    client->hdr = (ShmHeader*)client->shm_ptr;
    client->data[0] = (uint8_t*)client->shm_ptr + sizeof(ShmHeader);
    client->data[1] = client->data[0] + dsize;
    client->mapped_data_size = dsize;
    client->layout = parsed;
    client->known_layout_version = layout_version;
    client->attached = 1;
    client->state_error = 0;
    client->last_version = nekoecat_shm_load(&client->hdr->version, NEKOECAT_MO_ACQUIRE);

    // If the header's layout version differs from the one the layout was
    // fetched with, the offset map may be wrong — surface ERROR rather than
    // letting the caller read garbage.
    const uint32_t hdrLayoutVersion = nekoecat_shm_load(&client->hdr->layout_version, NEKOECAT_MO_ACQUIRE);
    if (layout_version != 0 && hdrLayoutVersion != layout_version) {
        set_error(client, "attach: layout version mismatch (header=%u, layout=%u)",
                  hdrLayoutVersion, layout_version);
        client->state_error = 1;
        errcpy(err, esz, "layout version mismatch");
    }
    return true;
}

bool nekoecat_client_wait_next_cycle(NekoEcatClient* client, int64_t timeout_ns) {
    if (!client || !client->attached || !client->hdr) {
        if (client) set_error(client, "wait_next_cycle: not attached");
        return false;
    }
    if (timeout_ns < 0) {
        set_error(client, "wait_next_cycle: negative timeout");
        return false;
    }
    const uint64_t timeout = (uint64_t)timeout_ns;
    const uint64_t start = now_ns();
    const uint64_t target = nekoecat_shm_load(&client->hdr->version, NEKOECAT_MO_ACQUIRE);
    // Honor small timeouts with a reasonable poll floor (50us).
    uint64_t poll_ns = 100000;
    if (timeout > 0 && timeout < poll_ns) poll_ns = timeout;
    if (poll_ns < 50000) poll_ns = 50000;
    for (;;) {
        if (nekoecat_shm_load(&client->hdr->version, NEKOECAT_MO_ACQUIRE) > target) {
            return true;
        }
        const uint64_t elapsed = now_ns() - start;
        if (elapsed >= timeout) return false;
        uint64_t remaining = timeout - elapsed;
        uint64_t sl = remaining < poll_ns ? remaining : poll_ns;
        struct timespec ts;
        ts.tv_sec = (time_t)(sl / 1000000000ULL);
        ts.tv_nsec = (long)(sl % 1000000000ULL);
        nanosleep(&ts, NULL);
    }
}

bool nekoecat_client_try_wait_next_cycle(NekoEcatClient* client) {
    if (!client || !client->hdr) return false;
    const uint64_t v = nekoecat_shm_load(&client->hdr->version, NEKOECAT_MO_ACQUIRE);
    if (v > client->last_version) {
        client->last_version = v;
        return true;
    }
    return false;
}

static uint32_t effective_data_size(NekoEcatClient* c) {
    if (!c || !c->hdr) return 0;
    // Never trust a header stride larger than the mapping we actually created
    // (a corrupted header must not drive reads/writes past our mmap'd region).
    uint32_t mapped = c->mapped_data_size;
    if (mapped == 0 || mapped > NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE) {
        mapped = NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE;
    }
    const uint32_t hsz = nekoecat_shm_load(&c->hdr->data_size, NEKOECAT_MO_RELAXED);
    if (hsz == 0 || hsz > NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE || hsz > mapped) {
        return mapped;
    }
    return hsz;
}

static bool check_bounds(NekoEcatClient* c, uint32_t off, uint32_t width) {
    if (off == UINT32_MAX) return false;
    const uint32_t sz = effective_data_size(c);
    if (width > sz) return false;
    if (off > sz - width) return false;
    return true;
}

/* Returns a pointer to the currently-active data buffer (after validating the
 * buffer index is 0 or 1). */
static uint8_t* get_active_data(NekoEcatClient* c, int* buf_out) {
    if (!c || !c->hdr) return NULL;
    const uint32_t b = nekoecat_shm_load(&c->hdr->active_buffer, NEKOECAT_MO_ACQUIRE);
    if (b > 1) return NULL;
    if (buf_out) *buf_out = (int)b;
    return c->data[b];
}

bool nekoecat_client_read_raw_at(NekoEcatClient* c, uint32_t off, void* buf, uint32_t nbytes) {
    if (!c || !c->attached || !c->hdr || !buf) {
        if (c) set_error(c, "read_raw_at: not attached");
        return false;
    }
    if (!check_bounds(c, off, nbytes)) {
        set_error(c, "read_raw_at: out of bounds (off=%u len=%u)", off, nbytes);
        return false;
    }
    // Double-read consistency protocol: no torn view on weakly-ordered CPUs.
    for (int attempt = 0; attempt < 4; ++attempt) {
        const uint64_t v1 = nekoecat_shm_load(&c->hdr->version, NEKOECAT_MO_ACQUIRE);
        int b = 0;
        uint8_t* d = get_active_data(c, &b);
        if (!d) {
            set_error(c, "read_raw_at: invalid active buffer");
            return false;
        }
        memcpy(buf, d + off, nbytes);
        const uint64_t v2 = nekoecat_shm_load(&c->hdr->version, NEKOECAT_MO_ACQUIRE);
        if (v1 == v2) {
            return true;
        }
    }
    set_error(c, "read_raw_at: version changed during read");
    return false;
}

bool nekoecat_client_write_raw_at(NekoEcatClient* c, uint32_t off, const void* buf, uint32_t nbytes) {
    if (!c || !c->attached || !c->hdr || !buf) {
        if (c) set_error(c, "write_raw_at: not attached");
        return false;
    }
    if (!check_bounds(c, off, nbytes)) {
        set_error(c, "write_raw_at: out of bounds (off=%u len=%u)", off, nbytes);
        return false;
    }
    int b = 0;
    uint8_t* d = get_active_data(c, &b);
    if (!d) {
        set_error(c, "write_raw_at: invalid active buffer");
        return false;
    }
    memcpy(d + off, buf, nbytes);
    return true;
}

bool nekoecat_client_read_u8_at(NekoEcatClient* c, uint32_t off, uint8_t* out) {
    if (!out) { if (c) set_error(c, "read_u8_at: null out"); return false; }
    uint8_t v = 0;
    if (!nekoecat_client_read_raw_at(c, off, &v, sizeof(v))) return false;
    *out = v;
    return true;
}
bool nekoecat_client_read_u16_at(NekoEcatClient* c, uint32_t off, uint16_t* out) {
    if (!out) { if (c) set_error(c, "read_u16_at: null out"); return false; }
    uint16_t v = 0;
    if (!nekoecat_client_read_raw_at(c, off, &v, sizeof(v))) return false;
    *out = v;
    return true;
}
bool nekoecat_client_read_u32_at(NekoEcatClient* c, uint32_t off, uint32_t* out) {
    if (!out) { if (c) set_error(c, "read_u32_at: null out"); return false; }
    uint32_t v = 0;
    if (!nekoecat_client_read_raw_at(c, off, &v, sizeof(v))) return false;
    *out = v;
    return true;
}
bool nekoecat_client_read_u64_at(NekoEcatClient* c, uint32_t off, uint64_t* out) {
    if (!out) { if (c) set_error(c, "read_u64_at: null out"); return false; }
    uint64_t v = 0;
    if (!nekoecat_client_read_raw_at(c, off, &v, sizeof(v))) return false;
    *out = v;
    return true;
}
bool nekoecat_client_read_float_at(NekoEcatClient* c, uint32_t off, float* out) {
    if (!out) { if (c) set_error(c, "read_float_at: null out"); return false; }
    uint32_t bits = 0;
    if (!nekoecat_client_read_raw_at(c, off, &bits, sizeof(bits))) return false;
    float v;
    memcpy(&v, &bits, sizeof(v));
    *out = v;
    return true;
}

bool nekoecat_client_write_u8_at(NekoEcatClient* c, uint32_t off, uint8_t value) {
    return nekoecat_client_write_raw_at(c, off, &value, sizeof(value));
}
bool nekoecat_client_write_u16_at(NekoEcatClient* c, uint32_t off, uint16_t value) {
    return nekoecat_client_write_raw_at(c, off, &value, sizeof(value));
}
bool nekoecat_client_write_u32_at(NekoEcatClient* c, uint32_t off, uint32_t value) {
    return nekoecat_client_write_raw_at(c, off, &value, sizeof(value));
}
bool nekoecat_client_write_u64_at(NekoEcatClient* c, uint32_t off, uint64_t value) {
    return nekoecat_client_write_raw_at(c, off, &value, sizeof(value));
}
bool nekoecat_client_write_float_at(NekoEcatClient* c, uint32_t off, float value) {
    uint32_t bits;
    memcpy(&bits, &value, sizeof(bits));
    return nekoecat_client_write_raw_at(c, off, &bits, sizeof(bits));
}

/* Resolve a layout entry to its byte offset; fails (and reports) when absent. */
static bool resolve_offset(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, uint32_t* out) {
    if (!c) return false;
    const uint32_t off = layout_byte_offset(&c->layout, slave, idx, sub);
    if (off == UINT32_MAX) {
        set_error(c, "layout entry not found (slave=%u index=0x%x sub=0x%x)", slave, idx, sub);
        return false;
    }
    *out = off;
    return true;
}

bool nekoecat_client_read_u8_by_index(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, uint8_t* out) {
    uint32_t off;
    if (!resolve_offset(c, slave, idx, sub, &off)) return false;
    return nekoecat_client_read_u8_at(c, off, out);
}
bool nekoecat_client_read_u16_by_index(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, uint16_t* out) {
    uint32_t off;
    if (!resolve_offset(c, slave, idx, sub, &off)) return false;
    return nekoecat_client_read_u16_at(c, off, out);
}
bool nekoecat_client_read_u32_by_index(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, uint32_t* out) {
    uint32_t off;
    if (!resolve_offset(c, slave, idx, sub, &off)) return false;
    return nekoecat_client_read_u32_at(c, off, out);
}
bool nekoecat_client_read_u64_by_index(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, uint64_t* out) {
    uint32_t off;
    if (!resolve_offset(c, slave, idx, sub, &off)) return false;
    return nekoecat_client_read_u64_at(c, off, out);
}
bool nekoecat_client_read_float_by_index(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, float* out) {
    uint32_t off;
    if (!resolve_offset(c, slave, idx, sub, &off)) return false;
    return nekoecat_client_read_float_at(c, off, out);
}

bool nekoecat_client_write_u8_by_index(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, uint8_t val) {
    uint32_t off;
    if (!resolve_offset(c, slave, idx, sub, &off)) return false;
    return nekoecat_client_write_u8_at(c, off, val);
}
bool nekoecat_client_write_u16_by_index(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, uint16_t val) {
    uint32_t off;
    if (!resolve_offset(c, slave, idx, sub, &off)) return false;
    return nekoecat_client_write_u16_at(c, off, val);
}
bool nekoecat_client_write_u32_by_index(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, uint32_t val) {
    uint32_t off;
    if (!resolve_offset(c, slave, idx, sub, &off)) return false;
    return nekoecat_client_write_u32_at(c, off, val);
}
bool nekoecat_client_write_u64_by_index(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, uint64_t val) {
    uint32_t off;
    if (!resolve_offset(c, slave, idx, sub, &off)) return false;
    return nekoecat_client_write_u64_at(c, off, val);
}
bool nekoecat_client_write_float_by_index(NekoEcatClient* c, uint16_t slave, uint16_t idx, uint8_t sub, float val) {
    uint32_t off;
    if (!resolve_offset(c, slave, idx, sub, &off)) return false;
    return nekoecat_client_write_float_at(c, off, val);
}

uint8_t* nekoecat_client_get_process_data_ptr(NekoEcatClient* c, int* buf_out) {
    if (!c || !c->attached || !c->hdr) return NULL;
    return get_active_data(c, buf_out);
}

uint32_t nekoecat_client_get_data_size(NekoEcatClient* c) {
    if (!c || !c->hdr) return 0;
    return effective_data_size(c);
}

uint64_t nekoecat_client_get_current_version(NekoEcatClient* c) {
    if (!c || !c->hdr) return 0;
    return nekoecat_shm_load(&c->hdr->version, NEKOECAT_MO_ACQUIRE);
}

NekoEcatState nekoecat_client_get_state(NekoEcatClient* c) {
    if (!c || !c->attached) return NEKOECAT_STATE_DISCONNECTED;
    if (c->state_error) return NEKOECAT_STATE_ERROR;
    if (!c->hdr) return NEKOECAT_STATE_ERROR;
    const uint64_t v = nekoecat_shm_load(&c->hdr->version, NEKOECAT_MO_ACQUIRE);
    const uint32_t flags = nekoecat_shm_load(&c->hdr->status_flags, NEKOECAT_MO_ACQUIRE);
    if (v == 0) return NEKOECAT_STATE_CONNECTED;
    if (flags & NEKOECAT_FLAG_RUNNING) return NEKOECAT_STATE_RUNNING;
    // Version advanced but the daemon is no longer publishing — stale data.
    return NEKOECAT_STATE_DATA_STALE;
}

int64_t nekoecat_client_get_cycle_count(NekoEcatClient* c) {
    if (!c || !c->hdr) return 0;
    return (int64_t)nekoecat_shm_load(&c->hdr->cycle_count, NEKOECAT_MO_RELAXED);
}

const char* nekoecat_client_get_last_error(NekoEcatClient* c) {
    return (c && c->last_err[0]) ? c->last_err : "";
}

/* Parse real layout from freeRunShmInfo JSON response (layout array with
 * slave/index/subindex/offset/direction).
 * Minimal scanner (no full JSON lib) to keep client volume small and Qt-free.
 */
bool layout_parse_from_shm_info_json(const char* json, struct ShmLayout* out) {
    if (!json || !out) return false;
    memset(out, 0, sizeof(*out));
    const char* ds = strstr(json, "\"data_size\":");
    if (ds) sscanf(ds + 12, "%u", &out->data_size);

    const char* lay = strstr(json, "\"layout\":[");
    if (!lay) return false;
    const char* p = lay + 10;  // after "layout":[

    while (out->count < NEKOECAT_SHM_LAYOUT_MAX_ENTRIES) {
        const char* obj = strstr(p, "{");
        if (!obj) break;
        const char* endobj = strstr(obj, "}");
        if (!endobj) break;

        uint16_t slave = 0, idx = 0;
        uint8_t sub = 0, bl = 0;
        uint32_t off = 0;
        char dirbuf[8] = {0};

        const char* f;
        if ((f = strstr(obj, "\"slave\":"))) sscanf(f + 8, "%hu", &slave);
        if ((f = strstr(obj, "\"index\":"))) sscanf(f + 8, "%hu", &idx);
        if ((f = strstr(obj, "\"subindex\":"))) sscanf(f + 11, "%hhu", &sub);
        if ((f = strstr(obj, "\"bitLength\":"))) sscanf(f + 12, "%hhu", &bl);
        if ((f = strstr(obj, "\"offset\":"))) sscanf(f + 9, "%u", &off);
        if ((f = strstr(obj, "\"direction\":"))) {
            sscanf(f + 12, "%7[^\"]", dirbuf);
        }

        struct ShmLayoutEntry* e = &out->entries[out->count];
        e->slave = slave;
        e->index = idx;
        e->sub = sub;
        e->bitLength = bl ? bl : 16;
        strncpy(e->direction, dirbuf[0] ? dirbuf : "TxPDO", sizeof(e->direction) - 1);
        e->direction[sizeof(e->direction) - 1] = 0;
        e->offset = off;
        out->count++;

        p = endobj + 1;
        const char* next_open = strstr(p, "{");
        const char* next_close = strstr(p, "]");
        if (!next_open || (next_close && next_open > next_close)) break;
    }

    // Expose silent truncation: if more entries remain than the cap, flag it.
    if (out->count >= NEKOECAT_SHM_LAYOUT_MAX_ENTRIES) {
        const char* more = strstr(p, "{");
        const char* close = strstr(p, "]");
        if (more && (!close || more < close)) {
            out->truncated = 1;
        }
    }
    return out->count > 0;
}

uint32_t layout_byte_offset(const struct ShmLayout* layout, uint16_t slave, uint16_t index, uint8_t sub) {
    if (!layout) return UINT32_MAX;
    for (size_t i = 0; i < layout->count; i++) {
        if (layout->entries[i].slave == slave &&
            layout->entries[i].index == index &&
            layout->entries[i].sub == sub) {
            return layout->entries[i].offset;
        }
    }
    return UINT32_MAX;  // sentinel: entry not found
}

/* ====================== Test helpers (NEKOECAT_TESTING only) ======================
 * These simulate the daemon side of the SHM arena for unit tests and are never
 * compiled into a shipped client library. */

#ifdef NEKOECAT_TESTING
void nekoecat_client_test_set_layout(NekoEcatClient* c, const struct ShmLayout* l) {
    if (c && l) {
        c->layout = *l;
    }
}

bool nekoecat_client_test_attach_to_shm(NekoEcatClient* client, const char* layout_json, uint8_t* shm_base, uint32_t dsize) {
    if (!client || !shm_base) return false;
    client->shm_ptr = shm_base;  // caller owns
    client->shm_size = sizeof(ShmHeader) + 2 * (dsize > 0 ? dsize : 4096);
    client->hdr = (ShmHeader*)client->shm_ptr;
    client->data[0] = (uint8_t*)client->shm_ptr + sizeof(ShmHeader);
    client->data[1] = client->data[0] + (dsize > 0 ? dsize : 4096);
    client->attached = 1;
    client->mapped_data_size = dsize > 0 ? dsize : 4096;
    // Simulate the daemon having published data_size before the client attaches.
    if (client->hdr) nekoecat_shm_store(&client->hdr->data_size, client->mapped_data_size, NEKOECAT_MO_RELAXED);
    if (layout_json) {
        layout_parse_from_shm_info_json(layout_json, &client->layout);
    }
    if (client->hdr) client->last_version = nekoecat_shm_load(&client->hdr->version, NEKOECAT_MO_ACQUIRE) - 1;
    return true;
}

void nekoecat_client_test_setup_shm(NekoEcatClient* c, uint8_t* shm_base, uint32_t dsize, uint64_t initial_version) {
    if (!c || !shm_base) return;
    ShmHeader* h = (ShmHeader*)shm_base;
    // Simulate the daemon's published header state.
    nekoecat_shm_store(&h->version, initial_version, NEKOECAT_MO_RELAXED);
    nekoecat_shm_store(&h->active_buffer, 0, NEKOECAT_MO_RELAXED);
    nekoecat_shm_store(&h->data_size, dsize, NEKOECAT_MO_RELAXED);
    c->hdr = h;
    c->data[0] = shm_base + sizeof(ShmHeader);
    c->data[1] = c->data[0] + dsize;
    c->attached = 1;
    c->mapped_data_size = dsize;
    c->last_version = initial_version - 1;
    c->layout.data_size = dsize;
}
#endif /* NEKOECAT_TESTING */
