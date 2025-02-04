#pragma once

#include <stdint.h>

// Control
#define REPRL_CRFD 100
#define REPRL_CWFD 101

// Data
#define REPRL_DRFD 102
#define REPRL_DWFD 103

#define REPRL_MAX_DATA_SIZE (16 * 1024 * 1024)

#define SHM_SIZE 0x100000
#define MAX_EDGES ((SHM_SIZE - 4) * 8)

struct shmem_data {
    uint32_t num_edges;
    unsigned char edges[];
};

// These are hooks into sancov's internals, their declaration isn't available in public headers.
// See compiler-rt/lib/sanitizer_common/sanitizer_interface_internal.h
extern "C" {
void __sanitizer_cov_trace_pc_guard_init(uint32_t*, uint32_t*);
void __sanitizer_cov_trace_pc_guard(uint32_t*);
}

void sanitizer_cov_reset_edgeguards();
