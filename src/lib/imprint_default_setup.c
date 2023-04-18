/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <imprint/default_setup.h>

/// Initializes a default setup for memory that would work for most basic applications.
/// @param self the setup to initialize
/// @param memory the number of octets to allocate
/// @return negative number on error
int imprintDefaultSetupInit(ImprintDefaultSetup *self, size_t memory)
{
    const size_t pageSize = 2 * 1024 * 1024;

    size_t pageCount = (memory + pageSize - 1) / pageSize;

    if (pageCount == 0) {
        return -1;
    }

    // The page allocator is the only allocator which is using malloc to get memory from the operating system.
    imprintPageAllocatorInit(&self->allPageAllocator, pageCount);

    imprintTaggedPageAllocatorInit(&self->allTaggedPageAllocator, &self->allPageAllocator);

    imprintTaggedAllocatorInit(&self->tagAllocator, &self->allTaggedPageAllocator, 0xfefe);

    ImprintAllocator* slabTagAllocator = &self->tagAllocator.info;

    imprintSlabAllocatorInit(&self->slabAllocator, slabTagAllocator, 7, 3,
                             128, "blobSlabAllocator");
    imprintSlabAllocatorAdd(&self->slabAllocator, slabTagAllocator, 20, 3,
                            "extra big size");

    return 0;
}


/// Free up memory allocated by imprintDefaultSetupInit().
/// @param self the setup to destroy
void imprintDefaultSetupDestroy(ImprintDefaultSetup *self)
{
    imprintTaggedAllocatorFreeAll(&self->tagAllocator);

    imprintTaggedPageAllocatorDestroy(&self->allTaggedPageAllocator);
    imprintPageAllocatorDestroy(&self->allPageAllocator);
}
