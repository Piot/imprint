# Imprint

Very simple memory allocator for C.

Only `ImprintPageAllocator` allocates memory from the operating system using `tc_malloc`.

A tag is a `uint64_t` number which associates each allocation under that tag. The value is chosen by the application.

* `ImprintPageAllocator` can only allocate and free complete pages (each page is 2 KiB).
* `ImprintTaggedPageAllocator` allocates memory pages and associates with a tag. Can only free memory using a tag.
* `ImprintTaggedAllocator` is using `ImprintTaggedPageAllocator` with a linear allocator over the pages received from the taggedPageAllocator.
* `ImprintSlabAllocator` allocates multiple arrays (SlabCaches) of different power of two sizes. For each allocation it selects the closest size relative the requested allocation size.

## Usage

Use macros to allocate memory:

* `IMPRINT_ALLOC(allocatorInfo, size, description)`
* `IMPRINT_CALLOC(allocatorInfo, size, description)`
* `IMPRINT_FREE(allocatorInfoWithFree, ptr)` Only for allocators that supports free (`ImprintSlabAllocator`).

* `IMPRINT_ALLOC_TYPE_COUNT(allocatorInfo, type, count)`. Allocates an array of specified type and count.
* `IMPRINT_CALLOC_TYPE_COUNT(allocatorInfo, type, count)`. Allocates an array of specified type and count. Clears the memory.

* `IMPRINT_ALLOC_TYPE(allocatorInfo, type)`. Allocates the specified type.
* `IMPRINT_CALLOC_TYPE(allocatorInfo, type)`. Allocates the specified type. Clears the memory.
