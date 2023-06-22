/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/tagged_page_allocator.h>
#include <inttypes.h>

void imprintTaggedPageAllocatorInit(ImprintTaggedPageAllocator* self, ImprintPageAllocator* pageAllocator)
{
    self->pageAllocator = pageAllocator;
    self->entryCapacity = 32;
    for (size_t i = 0; i < self->entryCapacity; ++i) {
        self->entries[i].tag = 0;
        self->entries[i].pageIds = 0;
    }
}

void imprintTaggedPageAllocatorDestroy(ImprintTaggedPageAllocator* self)
{
    for (size_t i = 0; i < self->entryCapacity; ++i) {
        ImprintTaggedPageEntry* entry = &self->entries[i];
        if (entry->tag) {
            CLOG_ERROR("tag %" PRIX64 " with pages %" PRIX64 " was not cleared", entry->tag, entry->pageIds);
            // imprintPageAllocatorFree(self->pageAllocator, entry->pageIds);
            entry->tag = 0;
        }
    }
}

void imprintTaggedPageAllocatorAlloc(ImprintTaggedPageAllocator* self, uint64_t tag, size_t pageCount,
                                     ImprintPageResult* result)
{
    ImprintTaggedPageEntry* foundEntry = 0;
    if (!tag) {
        CLOG_ERROR("tag can not be zero");
    }

    for (size_t i = 0; i < self->entryCapacity; ++i) {
        ImprintTaggedPageEntry* entry = &self->entries[i];
        if (entry->tag == tag) {
            foundEntry = entry;
            break;
        }
    }

    if (!foundEntry) {
        for (size_t i = 0; i < self->entryCapacity; ++i) {
            ImprintTaggedPageEntry* entry = &self->entries[i];
            if (!entry->tag) {
                foundEntry = entry;
                foundEntry->tag = tag;
                foundEntry->pageIds = 0;
                break;
            }
        }
    }

    if (!foundEntry) {
        CLOG_ERROR("out of memory");
    }

    imprintPageAllocatorAlloc(self->pageAllocator, pageCount, result);

    foundEntry->pageIds |= result->pageIds;
}

void imprintTaggedPageAllocatorFree(ImprintTaggedPageAllocator* self, uint64_t tag)
{
    for (size_t i = 0; i < self->entryCapacity; ++i) {
        ImprintTaggedPageEntry* entry = &self->entries[i];
        if (entry->tag == tag) {
            imprintPageAllocatorFreeSeparate(self->pageAllocator, entry->pageIds);
            entry->tag = 0;
            return;
        }
    }

    CLOG_ERROR("unknown tag %" PRIX64 , tag);
}
