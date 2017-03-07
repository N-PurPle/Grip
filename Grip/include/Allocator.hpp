#pragma once
#include <cstdint>


namespace Grip {



struct IAllocator
{
	virtual ~IAllocator() = default;
	virtual void* Alloc(size_t size, size_t alignment) = 0;
	virtual void* Realloc(void* ptr, size_t size, size_t alignment) = 0;
	virtual void  Free(void* ptr) = 0;
};





} // namespace Grip


