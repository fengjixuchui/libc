/**
 * @file libc.cpp
 * @author created by: Peter Hlavaty
 */

#include "libc.h"
#include <memory>

#pragma warning(push)               
#pragma warning (disable : 4565)

EXTERN_C
__drv_when(return!=0, __drv_allocatesMem(pBlock))
__checkReturn
__drv_maxIRQL(DISPATCH_LEVEL)
__bcount_opt(size)
void* 
__cdecl malloc(
	__in size_t size
	)
{
	MEMBLOCK *pBlock = static_cast<MEMBLOCK*>(
		ExAllocatePoolWithTag(
			NonPagedPoolNxCacheAligned, 
			size + sizeof(MEMBLOCK), 
			_LIBC_POOL_TAG));

	if (nullptr == pBlock)
		return nullptr;

	pBlock->size = size;	
	return pBlock->data;
}

EXTERN_C
__drv_when(return != 0, __drv_allocatesMem(p))
__checkReturn
__drv_maxIRQL(DISPATCH_LEVEL)
__bcount_opt(size * n)
void*
__cdecl calloc(size_t n, size_t size)
{
	size_t total = n * size;
	void *p = malloc(total);

	if (!p) return NULL;

	return memset(p, 0, total);
}

EXTERN_C
__drv_when(return!=0, __drv_allocatesMem(inblock))
__checkReturn
__drv_maxIRQL(DISPATCH_LEVEL)
__bcount_opt(size)
void* 
__cdecl realloc(
	__in_opt void* ptr, 
	__in size_t size
	)
{
	if (!ptr)
		return malloc(size);

	std::unique_ptr<unsigned char> inblock = std::unique_ptr<unsigned char>(static_cast<unsigned char*>(ptr));

	// alloc new block
	void* mem = malloc(size);
	if (!mem)
		return nullptr;

	// copy from old one, not overflow ..
	memcpy(mem, inblock.get(), min(CONTAINING_RECORD(inblock.get(), MEMBLOCK, data)->size, size));
	return mem;
}

EXTERN_C
__drv_maxIRQL(DISPATCH_LEVEL)
void 
__cdecl free(
	__inout_opt __drv_freesMem(Mem) void* ptr
	)
{
	if (ptr)
		ExFreePoolWithTag(CONTAINING_RECORD(ptr, MEMBLOCK, data), _LIBC_POOL_TAG);
}

#pragma warning(pop)

__drv_when(return!=0, __drv_allocatesMem(ptr))
__checkReturn
__drv_maxIRQL(DISPATCH_LEVEL)
__bcount_opt(size)
void* 
__cdecl operator new(
	__in size_t size
	)
{
	return malloc(size);
}

__drv_maxIRQL(DISPATCH_LEVEL)
void 
__cdecl operator delete(
	__inout void* ptr
	)
{
	free(ptr);
}

//http://www.castaglia.org/proftpd/doc/devel-guide/src/lib/vsnprintf.c.html
#include "vsnprintf.c"
