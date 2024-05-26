/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/default_setup.h>

/// Initializes a default setup for memory that would work for most basic applications.
/// @param self the setup to initialize
/// @param memory the number of octets to allocate
/// @return negative number on error
int imprintDefaultSetupInit(ImprintDefaultSetup* self, size_t memory)
{
    const size_t pageSize = 2 * 1024 * 1024;

    size_t pageCount = (memory + pageSize - 1) / pageSize;

    if (pageCount == 0) {
        return -1;
    }

    // The page allocator is the only allocator which is using malloc to get memory from the operating system.
    imprintPageAllocatorInit(&self->allPageAllocator, pageCount, "all page memory");

    imprintTaggedPageAllocatorInit(&self->allTaggedPageAllocator, &self->allPageAllocator);

    imprintTaggedAllocatorInit(&self->tagAllocator, &self->allTaggedPageAllocator, 0xfefe,
        "general tag allocator (0xfefe)");
    imprintTaggedAllocatorInit(&self->tagAllocatorForSlabs, &self->allTaggedPageAllocator, 0xfeff,
        "tag allocator for slabs (0xfeff)");

    ImprintAllocator* slabTagAllocator = &self->tagAllocatorForSlabs.info;
    imprintSlabAllocatorInit(&self->slabAllocator);

    // Add must be called in order of size!
    imprintSlabAllocatorAdd(&self->slabAllocator, slabTagAllocator, 5, 128, "slab: 32");
    imprintSlabAllocatorAdd(&self->slabAllocator, slabTagAllocator, 7, 96, "slab: 128");
    imprintSlabAllocatorAdd(&self->slabAllocator, slabTagAllocator, 8, 64, "slab: 256");
    imprintSlabAllocatorAdd(&self->slabAllocator, slabTagAllocator, 9, 64, "slab: 512");
    imprintSlabAllocatorAdd(&self->slabAllocator, slabTagAllocator, 10, 64, "slab: 1024");
    imprintSlabAllocatorAdd(&self->slabAllocator, slabTagAllocator, 11, 64, "slab: 2048");
    imprintSlabAllocatorAdd(&self->slabAllocator, slabTagAllocator, 20, 24, "slab: extra big size");

    return 0;
}

/// Free up memory allocated by imprintDefaultSetupInit().
/// @param self the setup to destroy
void imprintDefaultSetupDestroy(ImprintDefaultSetup* self)
{
    imprintTaggedAllocatorFreeAll(&self->tagAllocator);
    imprintTaggedAllocatorFreeAll(&self->tagAllocatorForSlabs);

    imprintTaggedPageAllocatorDestroy(&self->allTaggedPageAllocator);
    imprintPageAllocatorDestroy(&self->allPageAllocator);
}

void imprintDefaultSetupDebugOutput(const ImprintDefaultSetup* self, const char* debugOutput)
{
    #if defined CLOG_LOG_ENABLED
    CLOG_INFO("memory stats '%s':", debugOutput)
    #else
    (void) debugOutput;
    #endif
    imprintPageAllocatorDebugOutput(&self->allPageAllocator);
    imprintLinearAllocatorDebugOutput(&self->tagAllocator.linear);
    imprintLinearAllocatorDebugOutput(&self->tagAllocatorForSlabs.linear);
    imprintSlabAllocatorDebugOutput(&self->slabAllocator);
}
