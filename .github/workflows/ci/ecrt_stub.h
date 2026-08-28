/*
 * ecrt_stub.h — Minimal IgH EtherCAT Master (ecrt.h) stub for CI.
 *
 * CI runners without the IgH EtherCAT Master dev headers installed cannot
 * compile the daemon (FreeRunController/RtTestController/DcSyncHandler
 * include <ecrt.h> unconditionally).  This tracked header provides the
 * subset of the ecrt API used by the codebase so that the build step
 * succeeds without a real IgH installation.
 *
 * IMPORTANT:
 *   - All functions are `inline` and return safe defaults (NULL / 0), so
 *     the produced binaries never call into a real master.  Runtime tests
 *     that cannot reach hardware skip gracefully via QSKIP.
 *   - The signatures and struct layouts mirror the real IgH ecrt.h so that
 *     the translation units compile identically with and without IgH.
 *   - This is NOT a functional implementation; it exists only to let CI
 *     compile and run the unit/integration test suites.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* ---- Constants -------------------------------------------------------- */

#define EC_MAX_SYNC_MANAGERS 16
#define EC_MAX_STRING_LENGTH 64
#define EC_MAX_PORTS 4

/* Terminator for the ec_sync_info_t array passed to ecrt_slave_config_pdos(). */
#define EC_END ~0U

/* Byte access helpers (little-endian, matching the x86 CI runners). */
#define EC_READ_BIT(DATA, POS) (((*(const uint8_t *)(DATA)) >> (POS)) & 0x01U)
#define EC_READ_U8(DATA) (*(const uint8_t *)(DATA))
#define EC_READ_U16(DATA) (*(const uint16_t *)(DATA))
#define EC_READ_U32(DATA) (*(const uint32_t *)(DATA))
#define EC_READ_U64(DATA) (*(const uint64_t *)(DATA))
#define EC_READ_S8(DATA) (*(const int8_t *)(DATA))
#define EC_READ_S16(DATA) (*(const int16_t *)(DATA))
#define EC_READ_S32(DATA) (*(const int32_t *)(DATA))
#define EC_READ_S64(DATA) (*(const int64_t *)(DATA))
#define EC_READ_REAL(DATA) (*(const float *)(DATA))

/* ---- Opaque handles --------------------------------------------------- */

typedef struct ec_master ec_master_t;
typedef struct ec_domain ec_domain_t;
typedef struct ec_slave_config ec_slave_config_t;

/* ---- Enumerations ----------------------------------------------------- */

typedef enum {
    EC_DIR_INVALID,
    EC_DIR_OUTPUT,
    EC_DIR_INPUT,
    EC_DIR_COUNT
} ec_direction_t;

typedef enum {
    EC_WD_DEFAULT,
    EC_WD_ENABLE,
    EC_WD_DISABLE
} ec_watchdog_mode_t;

typedef enum {
    EC_WC_ZERO = 0,
    EC_WC_INCOMPLETE,
    EC_WC_COMPLETE
} ec_wc_state_t;

typedef enum {
    EC_PORT_NOT_IMPLEMENTED,
    EC_PORT_NOT_CONFIGURED,
    EC_PORT_EBUS,
    EC_PORT_MII
} ec_slave_port_desc_t;

/* ---- State / info structures ----------------------------------------- */

typedef struct {
    unsigned int slaves_responding;
    unsigned int al_states : 4;
    unsigned int link_up : 1;
} ec_master_state_t;

typedef struct {
    unsigned int slave_count;
    unsigned int link_up : 1;
    uint8_t scan_busy;
    uint64_t app_time;
} ec_master_info_t;

typedef struct {
    uint8_t link_up;
    uint8_t loop_closed;
    uint8_t signal_detected;
} ec_slave_port_link_t;

typedef struct {
    ec_slave_port_desc_t desc;
    ec_slave_port_link_t link;
    uint32_t receive_time;
    uint16_t next_slave;
    uint32_t delay_to_next_dc;
} ec_slave_port_t;

typedef struct {
    uint16_t position;
    uint32_t vendor_id;
    uint32_t product_code;
    uint32_t revision_number;
    uint32_t serial_number;
    uint16_t alias;
    int16_t current_on_ebus;
    ec_slave_port_t ports[EC_MAX_PORTS];
    uint8_t al_state;
    uint8_t error_flag;
    uint8_t sync_count;
    uint16_t sdo_count;
    char name[EC_MAX_STRING_LENGTH];
} ec_slave_info_t;

typedef struct {
    unsigned int working_counter;
    ec_wc_state_t wc_state;
    unsigned int redundancy_active;
} ec_domain_state_t;

typedef struct {
    uint16_t index;
    uint8_t subindex;
    uint8_t bit_length;
} ec_pdo_entry_info_t;

typedef struct {
    uint16_t index;
    unsigned int n_entries;
    ec_pdo_entry_info_t const *entries;
} ec_pdo_info_t;

typedef struct {
    uint8_t index;
    ec_direction_t dir;
    unsigned int n_pdos;
    ec_pdo_info_t const *pdos;
    ec_watchdog_mode_t watchdog_mode;
} ec_sync_info_t;

typedef struct {
    uint16_t alias;
    uint16_t position;
    uint32_t vendor_id;
    uint32_t product_code;
    uint16_t index;
    uint8_t subindex;
    unsigned int *offset;
    unsigned int *bit_position;
} ec_pdo_entry_reg_t;

/* ---- Stubbed API (inline, non-functional) ----------------------------- */

static inline ec_master_t *ecrt_request_master(unsigned int index)
{
    (void) index;
    return NULL;
}

static inline ec_master_t *ecrt_open_master(unsigned int index)
{
    (void) index;
    return NULL;
}

static inline void ecrt_release_master(ec_master_t *master)
{
    (void) master;
}

static inline int ecrt_master_reserve(ec_master_t *master)
{
    (void) master;
    return -1;
}

static inline ec_domain_t *ecrt_master_create_domain(ec_master_t *master)
{
    (void) master;
    return NULL;
}

static inline ec_slave_config_t *ecrt_master_slave_config(
    ec_master_t *master, uint16_t alias, uint16_t position,
    uint32_t vendor_id, uint32_t product_code)
{
    (void) master; (void) alias; (void) position;
    (void) vendor_id; (void) product_code;
    return NULL;
}

static inline int ecrt_master_select_reference_clock(
    ec_master_t *master, ec_slave_config_t *sc)
{
    (void) master; (void) sc;
    return -1;
}

static inline int ecrt_master(ec_master_t *master, ec_master_info_t *master_info)
{
    (void) master;
    if (master_info) {
        master_info->slave_count = 0;
        master_info->link_up = 0;
        master_info->scan_busy = 0;
        master_info->app_time = 0;
    }
    return -1;
}

static inline int ecrt_master_get_slave(
    ec_master_t *master, uint16_t slave_position, ec_slave_info_t *slave_info)
{
    (void) master; (void) slave_position; (void) slave_info;
    return -1;
}

static inline int ecrt_master_get_sync_manager(
    ec_master_t *master, uint16_t slave_position, uint8_t sync_index,
    ec_sync_info_t *sync)
{
    (void) master; (void) slave_position; (void) sync_index; (void) sync;
    return -1;
}

static inline int ecrt_master_get_pdo(
    ec_master_t *master, uint16_t slave_position, uint8_t sync_index,
    uint16_t pos, ec_pdo_info_t *pdo)
{
    (void) master; (void) slave_position; (void) sync_index;
    (void) pos; (void) pdo;
    return -1;
}

static inline int ecrt_master_get_pdo_entry(
    ec_master_t *master, uint16_t slave_position, uint8_t sync_index,
    uint16_t pdo_pos, uint16_t entry_pos, ec_pdo_entry_info_t *entry)
{
    (void) master; (void) slave_position; (void) sync_index;
    (void) pdo_pos; (void) entry_pos; (void) entry;
    return -1;
}

static inline int ecrt_master_activate(ec_master_t *master)
{
    (void) master;
    return -1;
}

static inline int ecrt_master_deactivate(ec_master_t *master)
{
    (void) master;
    return 0;
}

static inline int ecrt_master_set_send_interval(
    ec_master_t *master, size_t send_interval)
{
    (void) master; (void) send_interval;
    return -1;
}

static inline int ecrt_slave_config_pdos(
    ec_slave_config_t *sc, unsigned int n_syncs, const ec_sync_info_t syncs[])
{
    (void) sc; (void) n_syncs; (void) syncs;
    return -1;
}

static inline int ecrt_domain_reg_pdo_entry_list(
    ec_domain_t *domain, const ec_pdo_entry_reg_t *pdo_entry_regs)
{
    (void) domain; (void) pdo_entry_regs;
    return -1;
}

static inline uint8_t *ecrt_domain_data(ec_domain_t *domain)
{
    (void) domain;
    return NULL;
}

static inline int ecrt_master_state(
    ec_master_t *master, ec_master_state_t *state)
{
    (void) master;
    if (state) {
        state->slaves_responding = 0;
        state->al_states = 0;
        state->link_up = 0;
    }
    return 0;
}

static inline int ecrt_domain_state(
    ec_domain_t *domain, ec_domain_state_t *state)
{
    (void) domain;
    if (state) {
        state->working_counter = 0;
        state->wc_state = EC_WC_ZERO;
        state->redundancy_active = 0;
    }
    return 0;
}

static inline int ecrt_master_application_time(
    ec_master_t *master, uint64_t app_time)
{
    (void) master; (void) app_time;
    return 0;
}

static inline int ecrt_master_receive(ec_master_t *master)
{
    (void) master;
    return 0;
}

static inline int ecrt_domain_process(ec_domain_t *domain)
{
    (void) domain;
    return 0;
}

static inline int ecrt_domain_queue(ec_domain_t *domain)
{
    (void) domain;
    return 0;
}

static inline int ecrt_master_send(ec_master_t *master)
{
    (void) master;
    return 0;
}

static inline int ecrt_master_reference_clock_time(
    ec_master_t *master, uint32_t *time)
{
    (void) master;
    if (time) {
        *time = 0;
    }
    return 0;
}

static inline int ecrt_master_sync_monitor_queue(ec_master_t *master)
{
    (void) master;
    return 0;
}

static inline uint32_t ecrt_master_sync_monitor_process(ec_master_t *master)
{
    (void) master;
    return 0;
}

static inline int ecrt_master_sdo_upload(
    ec_master_t *master, uint16_t slave_position, uint16_t index,
    uint8_t subindex, uint8_t *target, size_t target_size,
    size_t *result_size, uint32_t *abort_code)
{
    (void) master; (void) slave_position; (void) index; (void) subindex;
    (void) target; (void) target_size; (void) result_size; (void) abort_code;
    return -1;
}

static inline int ecrt_master_sdo_download(
    ec_master_t *master, uint16_t slave_position, uint16_t index,
    uint8_t subindex, const uint8_t *data, size_t data_size,
    uint32_t *abort_code)
{
    (void) master; (void) slave_position; (void) index; (void) subindex;
    (void) data; (void) data_size; (void) abort_code;
    return -1;
}

#ifdef __cplusplus
}
#endif
