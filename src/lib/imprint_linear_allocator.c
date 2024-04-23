/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/linear_allocator.h>
#include <imprint/utils.h>

#define IMPRINT_LINEAR_ALLOCATOR_DETAILED_LOG CONFIGURATION_DEBUG

void imprintLinearAllocatorSelfAlloc(
    ImprintLinearAllocator* self, ImprintAllocator* allocator, size_t size, const char* debug)
{
    imprintLinearAllocatorInit(self, IMPRINT_ALLOC(allocator, size, debug), size, debug);
}

void imprintLinearAllocatorReset(ImprintLinearAllocator* self)
{
    self->next = self->memory;
}

void* imprintLinearAllocatorAlloc(ImprintLinearAllocator* self, size_t size)
{
    if (self == 0) {
        CLOG_ERROR(">>> linear allocate: NULL memory %zu  '%s'", size, self->debug)
    }
    if (size == 0) {
        return 0;
    }

    if (size > 64 * 1024) {
        CLOG_NOTICE(">>> linear allocate: big allocation: %zu octets", size)
    }

    size_t align = 8;
    if (self->memory == 0) {
        CLOG_ERROR(">>> linear allocate: Null memory!")
    }

    size_t rest = (uintptr_t)self->next % align;
    if (align != 0) {
        self->next += align - rest;
    }

    size_t allocated = (size_t)(self->next - self->memory);

    if (allocated + size > self->size) {
        CLOG_ERROR(
            ">>> linear allocate: Error: LinearAllocatorAlloc: Out of memory! %s %zu %zu (%zu/%zu)",
            self->debug, size, allocated / size, allocated, self->size)
    }

#if defined IMPRINT_LINEAR_ALLOCATOR_DETAILED_LOG
    CLOG_EXECUTE(char buf1[64];)
    CLOG_EXECUTE(char buf[64];)
    CLOG_VERBOSE(">>> linear allocate: %s '%s' (%s)", imprintSizeToString(buf1, 64, size),
        self->debug, imprintSizeAndPercentageToString(buf, 64, allocated, self->size))
#endif

    uint8_t* next = self->next;
    self->next += size;

    return next;
}

void* imprintLinearAllocatorAllocDebug(ImprintLinearAllocator* self, size_t size,
    const char* source_file, size_t line, const char* description)
{
    if (self == 0) {
        CLOG_ERROR(">>> linear allocate: NULL memory %zu %s:%zu '%s'", size, source_file, line,
            description)
    }
    if (self->memory == 0) {
        CLOG_ERROR(">>> linear allocate: Memory is null")
    }
    void* p = imprintLinearAllocatorAlloc(self, size);
    if (size > 0 && p == 0) {
        CLOG_ERROR(">>> linear allocate: Out of memory %zu %s:%zu '%s'", size, source_file, line,
            description)
    }
    return p;
}

#if !defined CONFIGURATION_DEBUG
static void* imprintLinearAllocatorAllocInternal(void* self_, size_t size)
{
    ImprintLinearAllocator* self = (ImprintLinearAllocator*)self_;

    return imprintLinearAllocatorAlloc(self, size);
}
#endif

#if defined CONFIGURATION_DEBUG
static void* imprintLinearAllocatorAllocDebugInternal(
    void* self_, size_t size, const char* sourceFile, size_t lineNumber, const char* description)
{
    ImprintLinearAllocator* self = (ImprintLinearAllocator*)self_;

    return imprintLinearAllocatorAllocDebug(self, size, sourceFile, lineNumber, description);
}
#endif

void* imprintLinearAllocatorCalloc(ImprintLinearAllocator* self, size_t size)
{
    void* p = imprintLinearAllocatorAlloc(self, size);
    if (p == 0) {
        return p;
    }
    tc_mem_clear(p, size);
    return p;
}

#if defined CONFIGURATION_DEBUG
void* imprintLinearAllocatorCallocDebug(ImprintLinearAllocator* self, size_t size,
    const char* source_file, size_t line, const char* description)
{
    if (self == 0) {
        CLOG_ERROR(">>> linear allocate: NULL memory %zu %s:%zu '%s'", size, source_file, line,
            description)
    }

    void* p = imprintLinearAllocatorCalloc(self, size);
    if (size > 0 && p == 0) {
        CLOG_ERROR(">>> linear allocate: Out of memory %zu %s:%zu '%s'", size, source_file, line,
            description)
    }
    return p;
}

static void* imprintLinearAllocatorCallocDebugInternal(
    void* self_, size_t size, const char* sourceFile, size_t lineNumber, const char* description)
{
    ImprintLinearAllocator* self = (ImprintLinearAllocator*)self_;

    return imprintLinearAllocatorCallocDebug(self, size, sourceFile, lineNumber, description);
}
#endif

#if !defined CONFIGURATION_DEBUG
static void* imprintLinearAllocatorCallocInternal(void* self_, size_t size)
{
    ImprintLinearAllocator* self = (ImprintLinearAllocator*)self_;

    return imprintLinearAllocatorCalloc(self, size);
}
#endif

void imprintLinearAllocatorDebugOutput(const ImprintLinearAllocator* self)
{
    static char buf[64];

    size_t allocated = (size_t)(self->next - self->memory);
    static char allocate[64];
    size_t debugCharacterCount = allocated * 63 / self->size;
    for (size_t i = 0; i < debugCharacterCount; ++i) {
        allocate[i] = 'X';
    }
    for (size_t i = debugCharacterCount; i < 63; ++i) {
        allocate[i] = '.';
    }
    allocate[63] = 0;

    CLOG_INFO(">>> linear allocate: debug output '%s' (%s) %s", self->debug,
        imprintSizeAndPercentageToString(buf, sizeof(buf), allocated, self->size), allocate)
}

void imprintLinearAllocatorInit(
    ImprintLinearAllocator* self, uint8_t* memory, size_t size, const char* debug)
{
    self->memory = memory;
    self->next = self->memory;
    self->size = size;
    self->debug = debug;
#if defined CONFIGURATION_DEBUG
    self->info.allocDebugFn = imprintLinearAllocatorAllocDebugInternal;
    self->info.callocDebugFn = imprintLinearAllocatorCallocDebugInternal;
#else
    self->info.allocFn = imprintLinearAllocatorAllocInternal;
    self->info.callocFn = imprintLinearAllocatorCallocInternal;
#endif
}
