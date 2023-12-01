#pragma once

#include <vector>
#include <cstdint>

#include "singleton.h"

namespace utils {

	constexpr auto DEFAULT_BLOCK_SIZE = 4096 * 10;
	constexpr auto DEFAULT_MAX_SIZE = 512;
	class FixedAlloc;

#define USE_EMBEDDED_PTR // 使用指针方式
	class Chunk
	{
		friend FixedAlloc;

		struct Obj
		{
			struct Obj* next; // embedded pointer(内嵌指针)
		};

	public:
		void init(size_t blockSize, uint32_t blocks);
		void release();
		void* alloc(size_t blockSize);
		void dealloc(void* p, size_t blockSize);

	private:
		void reset(size_t blockSize, uint32_t blocks);

	private:
#ifndef USE_EMBEDDED_PTR
		unsigned char m_firstAvaiableChunk;
		uint32_t m_blockAvaiable;
#else
		Obj* m_freeList = nullptr;
#endif
		unsigned char* m_Data = nullptr;
	};

	class FixedAlloc
	{
		using Chunks = std::vector<Chunk>;

	public:
		explicit FixedAlloc(size_t blockSize = 0);
		~FixedAlloc();

		// 分配内存快
		void* allocate();
		// 释放内存块
		void deallocate(void* p);
		size_t blockSize() const { return m_blockSize; }
		bool operator<(size_t n) const
		{
			return m_blockSize < n;
		}

	private:
		void doDeallocate(void* p);
		Chunk* findChunk(void* p);

	private:
		Chunks m_Chunks;
		size_t m_blockSize; // 内存块大小
		size_t m_numChunks; // 内存块数量

		Chunk* m_allocChunk;
		Chunk* m_deallocChunk;
	};

	// 对外接口类
	class ObjAllocator : public Singleton<ObjAllocator>
	{
		using MemoryPool = std::vector<FixedAlloc>;

	public:
		ObjAllocator(size_t blockSize = DEFAULT_BLOCK_SIZE, size_t maxObjSize = DEFAULT_MAX_SIZE);

		void* Allocate(size_t numBytes);
		void Deallocate(void* p, size_t size);

	private:
		MemoryPool m_pool;
		size_t m_blockSize;
		size_t m_maxObjSize;
		FixedAlloc* m_lastAlloc;
		FixedAlloc* m_lastDealloc;
	};

	template<typename T, typename... Args>
	T* _new(Args &&... args) noexcept
	{
		void* p = ObjAllocator::instance().Allocate(sizeof(T));
		if (!p) return nullptr;
		return new (p) T(std::forward<Args>(args)...); // placement new(constructor)
	}

	template<typename T>
	void _delete(T* p) noexcept
	{
		p->~T(); //placement destructor
		ObjAllocator::instance().Deallocate(p, sizeof(T));
	}
} // namespace utils
