/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/page_allocator.h>
#include <imprint/utils.h>
#include <stdbool.h>
#include <inttypes.h>

void imprintPageAllocatorInit(ImprintPageAllocator* self, size_t pageCount)
{
    self->pageCount = pageCount;
    if (pageCount > 63)
    {
      CLOG_ERROR("imprintPageAllocatorInit: 63 pages are max")
    }
    self->allocatedPageCount = 0;
    self->pageSizeInOctets = 2 * 1024 * 1024;
    self->basePointerForPages = tc_malloc(self->pageSizeInOctets * self->pageCount);
    CLOG_EXECUTE(char buf[32]);
    CLOG_DEBUG("=== Allocated all page memory %zu (%zu count) %s", self->pageSizeInOctets, self->pageCount, imprintSizeToString(buf, 32, self->pageCount * self->pageSizeInOctets))
    self->maxFreePagesMask = ((uint64_t )1 << self->pageCount)- 1;
    self->freePages = self->maxFreePagesMask;
}

void imprintPageAllocatorDestroy(ImprintPageAllocator* self)
{
    if (self->freePages != self->maxFreePagesMask) {
        CLOG_ERROR("pages %" PRIx64 " was not cleared", self->freePages)
    }
    tc_free(self->basePointerForPages);
    self->basePointerForPages = 0;
}

void imprintPageAllocatorAlloc(ImprintPageAllocator* self, size_t pageCount, ImprintPageResult* result)
{
    CLOG_EXECUTE(char buf[32]);

    if (pageCount > 5) {
        CLOG_NOTICE("imprintPageAllocatorAlloc: big number of pages allocated: %zu (%s)", pageCount,
                    imprintSizeToString(buf, 32, pageCount*self->pageSizeInOctets))
    }

    uint64_t requestMask = ((1 << pageCount) - 1);
    for (size_t i = 0; i < 64 - pageCount; ++i) {
        uint64_t usedAndRequestMask = self->freePages & requestMask;
        if (usedAndRequestMask == requestMask) {
            self->freePages &= ~requestMask;
            result->pageIds = requestMask;
            result->memory = self->basePointerForPages + i * self->pageSizeInOctets;
            self->allocatedPageCount += pageCount;
            CLOG_DEBUG(">>>> pages %" PRIX64 " allocated (%zu page count) (%zu, %s, %zu/%zu total allocated, freePages: %" PRIX64, requestMask, pageCount,
                       self->allocatedPageCount,
                       imprintSizeToString(buf, 32, self->allocatedPageCount * self->pageSizeInOctets), self->allocatedPageCount, self->pageCount, self->freePages)
            return;
        }

        requestMask <<= 1;
    }

    CLOG_ERROR("page allocator: out of memory pageCount %zu freeMask %" PRIX64, pageCount, self->freePages)
}

void imprintPageAllocatorFree(ImprintPageAllocator* self, ImprintPageIdList pageIds)
{
    self->freePages |= pageIds;

#if defined CONFIGURATION_DEBUG

    uint64_t mask = 1;
    size_t start = 0;
    bool hasFoundStart = false;
    size_t stop = 63;
    for (size_t i = 0; i < 64; ++i) {
        if (pageIds & mask) {
            if (!hasFoundStart) {
                start = i;
                hasFoundStart = true;
            }
        } else if (hasFoundStart) {
            stop = i;
            break;
        }
        mask <<= 1;
    }

    size_t pageCount = stop - start + 1;

    tc_memset_octets(self->basePointerForPages + start * self->pageSizeInOctets, 0xbd,
                     pageCount * self->pageSizeInOctets);

#endif
}

void imprintPageAllocatorFreeSeparate(ImprintPageAllocator* self, ImprintPageIdList pageIds)
{
    self->freePages |= pageIds;

    CLOG_EXECUTE(char buf[32]);
    CLOG_DEBUG(">>>> pages %" PRIX64  " free (%zu, %s allocated)", pageIds, self->allocatedPageCount,
               imprintSizeToString(buf, 32, self->allocatedPageCount * self->pageSizeInOctets))
    uint64_t mask = 1;
    for (size_t i = 0; i < 64; ++i) {
        if (pageIds & mask) {
            if (self->allocatedPageCount == 0) {
                CLOG_ERROR("too many free")
            }
            self->allocatedPageCount--;
        }
        mask <<= 1;
    }

    CLOG_DEBUG(">>>> pages %" PRIX64 " after free (%zu allocated)", pageIds,
                       self->allocatedPageCount)

#if defined CONFIGURATION_DEBUG
    uint64_t xmask = 1;
    for (size_t i = 0; i < 64; ++i) {
        if (pageIds & xmask) {
            tc_memset_octets(self->basePointerForPages + i * self->pageSizeInOctets, 0xbd, self->pageSizeInOctets);
        }
        xmask <<= 1;
    }

#endif
}
