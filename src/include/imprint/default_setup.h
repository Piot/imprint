/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_DEFAULT_SETUP_H
#define IMPRINT_DEFAULT_SETUP_H

#include <imprint/linear_allocator.h>
#include <imprint/page_allocator.h>
#include <imprint/slab_allocator.h>
#include <imprint/tagged_allocator.h>
#include <imprint/tagged_page_allocator.h>

typedef struct ImprintDefaultSetup {
    ImprintPageAllocator allPageAllocator;
    ImprintTaggedPageAllocator allTaggedPageAllocator;
    ImprintTaggedAllocator tagAllocator;
    ImprintTaggedAllocator tagAllocatorForSlabs;
    ImprintSlabAllocator slabAllocator;
} ImprintDefaultSetup;

int imprintDefaultSetupInit(ImprintDefaultSetup* self, size_t memorySize);
void imprintDefaultSetupDestroy(ImprintDefaultSetup* self);
void imprintDefaultSetupDebugOutput(const ImprintDefaultSetup* self, const char* description);

#endif
