#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

void* StompAllocator::Alloc(int32 size)
{
	const int32 pageCount = (size / PAGE_SIZE - 1) / PAGE_SIZE;
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

void* PoolAllocator::Alloc(int32 size)
{
	void* ptr = GMemory->Allocate(size);
	return ptr;
}

void PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}