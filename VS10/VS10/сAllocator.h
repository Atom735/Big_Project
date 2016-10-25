#ifndef H_C_ALLOCATOR
#define H_C_ALLOCATOR

#include <cstdlib>

class ZeroedMemoryAllocator
{
public:
	ZeroedMemoryAllocator() {}
	virtual ~ZeroedMemoryAllocator() {}
	inline void *operator new(size_t size) { return calloc(size, 1); }
	inline void *operator new[](size_t size) { return calloc(size, 1); }
	inline void operator delete(void *ptr) { free(ptr); }
	inline void operator delete[](void *ptr) { free(ptr); }
};

#endif