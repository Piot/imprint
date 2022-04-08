#include <imprint/page_allocator.h>

#include <tiny-libc/tiny_libc.h>
#include <limits.h>
#include <clog/clog.h>
#include <stdbool.h>

void imprintPageAllocatorInit(ImprintPageAllocator* self, size_t pageCount)
{
    self->pageCount = pageCount;
    self->pageSizeInOctets = 2 * 1024 * 1024;
    self->basePointerForPages = tc_malloc(self->pageSizeInOctets * pageCount);
    self->freePages = ULONG_MAX;
}

void imprintPageAllocatorDestroy(ImprintPageAllocator* self)
{
    tc_free(self->basePointerForPages);
    self->basePointerForPages = 0;
}

void imprintPageAllocatorAlloc(ImprintPageAllocator* self, size_t pageCount, ImprintPageResult* result) {
    uint64_t requestMask = ((1 << pageCount) -1);
    for (size_t i = 0; i < 64 - pageCount; ++i) {
        uint64_t usedAndRequestMask = self->freePages & requestMask;
        if (usedAndRequestMask == requestMask) {
            self->freePages &= ~requestMask;
            result->pageIds = requestMask;
            result->memory = self->basePointerForPages + i * self->pageSizeInOctets;
            return;
        }

        requestMask <<= 1;
    }

    CLOG_ERROR("page allocator: out of memory");
}

void imprintPageAllocatorFree(ImprintPageAllocator* self, ImprintPageIdList pageIds)
{
    self->freePages |= pageIds;

    #if CONFIGURATION_DEBUG

    uint64_t mask = 1;
    size_t start = 0;
    bool hasFoundStart = false;
    size_t stop = 63;
    for (size_t i=0; i<64; ++i) {
        if (pageIds & mask) {
            if (hasFoundStart) {
                start = i;
                hasFoundStart = true;
            }
        } else if (hasFoundStart) {
            stop = i;
            break;
        }
        mask <<= 1;
    }

    size_t pageCount = stop - start;

    tc_memset_octets(self->basePointerForPages + start * self->pageSizeInOctets, 0xbd, pageCount * self->pageSizeInOctets);

    #endif

}

void imprintPageAllocatorFreeSeparate(ImprintPageAllocator* self, ImprintPageIdList pageIds)
{
    self->freePages |= pageIds;

    #if CONFIGURATION_DEBUG

    for (size_t i=0; i<64; ++i) {
        uint64_t mask = 1 << i;
        if (pageIds & mask) {
            tc_memset_octets(self->basePointerForPages + i * self->pageSizeInOctets, 0xbd, self->pageSizeInOctets);
        }
    }

    #endif
}
