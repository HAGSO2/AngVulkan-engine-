#include "kmemory.h"

#include "core/logger.h"
#include "core/Astring.h"
#include "platform/platform.h"

/*struct memory_stats {
    u64 total_allocated;
    u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

typedef struct memory_system_state {
    struct memory_stats stats;
    u64 alloc_count;
} memory_system_state;

static memory_system_state* state_ptr;*/

void kfree(void* block, u64 size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        KWARN("kfree called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
    }
    /*if (state_ptr) {
        state_ptr->stats.total_allocated -= size;
        state_ptr->stats.tagged_allocations[tag] -= size;
    }*/

    // TODO: Memory alignment
    platform_free(block, false);
}

void* kzero_memory(void* block, u64 size) {
    return platform_zero_memory(block, size);
}
