/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_LINEAR_ALLOCATOR_H
#define IMPRINT_LINEAR_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

#include <imprint/allocator.h>

typedef struct ImprintLinearAllocator {
    ImprintAllocator info;
    uint8_t* memory;
    size_t size;
    uint8_t* next;
    const char* debug;
} ImprintLinearAllocator;

void imprintLinearAllocatorInit(
    ImprintLinearAllocator* self, uint8_t* memory, size_t size, const char* debug);

void imprintLinearAllocatorSelfAlloc(ImprintLinearAllocator* self,
    struct ImprintAllocator* allocator, size_t size, const char* debug);
void imprintLinearAllocatorReset(ImprintLinearAllocator* self);

void* imprintLinearAllocatorAlloc(ImprintLinearAllocator* self, size_t size);

void* imprintLinearAllocatorAllocDebug(ImprintLinearAllocator* self, size_t size,
    const char* source_file, size_t line, const char* description);

void* imprintLinearAllocatorCalloc(ImprintLinearAllocator* self, size_t size);
void* imprintLinearAllocatorCallocDebug(ImprintLinearAllocator* self, size_t size,
    const char* source_file, size_t line, const char* description);
void imprintLinearAllocatorDebugOutput(const ImprintLinearAllocator* self);

#endif
