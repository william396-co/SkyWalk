#include "pool_alloc.h"

#include <cassert>
#include <climits>
#include <algorithm>
#include <vector>
#include <cstring>
#include <iostream>
#include <cstdint>

namespace utils {

#define USE_NEW_DELETE

	void Chunk::init(size_t blockSize, uint32_t blocks)
	{
		assert(blockSize > 0);
		assert(blocks > 0);

		// Overflow check
		assert((blockSize * blocks) / blockSize == blocks);

#ifndef USE_EMBEDDED_PTR
		m_Data = (unsigned char*)malloc(blockSize * blocks);
#else
		m_Data = new unsigned char[blocks * blockSize];
#endif
		assert(m_Data);
		reset(blockSize, blocks);
	}

	void Chunk::release()
	{
		assert(m_Data);
#ifndef USE_NEW_DELETE
		free(m_Data);
#else
		delete[] m_Data;
#endif
	}

	void Chunk::reset(size_t blockSize, uint32_t blocks)
	{
		assert(m_Data);
		assert(blockSize > 0);
		assert(blocks > 0);

#ifndef USE_EMBEDDED_PTR
		m_blockAvaiable = blocks;
		m_firstAvaiableChunk = 0;
		unsigned char* p = m_Data;
		for (uint32_t i = 0; i != blocks; p += blockSize) {
			*p = ++i; // 保存下一个可使用内存块index
		}
#else
		if (nullptr == m_freeList) {
			Obj* p = (Obj*)m_Data;
			m_freeList = (Obj*)m_Data;

			//将内存连接起来
			for (uint32_t i = 0; i < blocks - 1; ++i) {
				p->next = (Obj*)((unsigned char*)p + blockSize);
				p = p->next;
			}
			p->next = nullptr;
		}
#endif
	}

	void* Chunk::alloc(size_t blockSize)
	{
#ifndef USE_EMBEDDED_PTR
		if (!m_blockAvaiable) return nullptr;
		unsigned char* p = m_Data + (blockSize * m_firstAvaiableChunk); // 获取第一个可分配的空闲块

		m_firstAvaiableChunk = *p; // 更新第一个空闲块index
		--m_blockAvaiable;         // 减少可用块计数
#else
		if (!m_freeList) return nullptr;

		Obj* p = m_freeList;
		m_freeList = m_freeList ? m_freeList->next : nullptr;
#endif
		return p;
	}

	void Chunk::dealloc(void* p, size_t blockSize)
	{
		assert(p >= m_Data);

#ifndef USE_EMBEDDED_PTR
		unsigned char* pRelease = static_cast<unsigned char*>(p);

		*pRelease = m_firstAvaiableChunk;                                                       // 第一个空闲块index存入待释放块
		m_firstAvaiableChunk = static_cast<unsigned char>((pRelease - m_Data) / blockSize); // 把待释放块index保存为第一个空闲块
		++m_blockAvaiable;                                                                      // 增加可用块计数
#else
		// insert before
		((Obj*)p)->next = m_freeList;
		m_freeList = (Obj*)p;
#endif
	}

	FixedAlloc::FixedAlloc(size_t blockSize)
		: m_blockSize{ blockSize },
		m_allocChunk{ nullptr }, m_deallocChunk{ nullptr }
	{
		assert(blockSize > 0);

		if (blockSize < sizeof(nullptr)) // 最少为一个字长
			blockSize = sizeof(nullptr);
		m_numChunks = DEFAULT_BLOCK_SIZE / blockSize;
	}

	FixedAlloc::~FixedAlloc()
	{
		for (auto& it : m_Chunks) {
			it.release();
		}
		m_Chunks.clear();
	}

	void* FixedAlloc::allocate()
	{
#ifndef USE_EMBEDDED_PTR
		if (!m_allocChunk || m_allocChunk->m_blockAvaiable == 0) {
#else
		if (!m_allocChunk || !m_allocChunk->m_freeList) {
#endif
			for (auto it = m_Chunks.begin();; ++it) {
				// 未找到，尾部创建一个新的chunk
				if (it == m_Chunks.end()) {
					m_Chunks.emplace_back(Chunk{});
					auto& newChunk = m_Chunks.back();
					newChunk.init(m_blockSize, (uint32_t)m_numChunks);
					m_allocChunk = &newChunk;
					m_deallocChunk = &m_Chunks.front(); // emplace
					break;
				} // 查找可分配的chunk
#ifndef USE_EMBEDDED_PTR
				if (it->m_blockAvaiable > 0) {
#else
				if (it->m_freeList) {
#endif
					m_allocChunk = &*it; // 取迭代器的地址
					break;
				}
			}
		}

		assert(m_allocChunk);
#ifndef USE_EMBEDDED_PTR
		assert(m_allocChunk->m_blockAvaiable > 0);
#else
		assert(m_allocChunk->m_freeList);
#endif
		return m_allocChunk->alloc(m_blockSize);
	}

	void FixedAlloc::deallocate(void* p)
	{
		m_deallocChunk = findChunk(p);
		assert(m_deallocChunk);
		doDeallocate(p);
	}

	Chunk* FixedAlloc::findChunk(void* p)
	{
		assert(!m_Chunks.empty());
		assert(m_deallocChunk);

		const auto ChunkLen = m_blockSize * m_numChunks;

		Chunk* lo = m_deallocChunk;
		Chunk* hi = m_deallocChunk + 1;

		Chunk* loBound = &m_Chunks.front();
		Chunk* hiBound = &m_Chunks.back() + 1;

		if (hi == hiBound) hi = nullptr;

		while (true) {
			if (lo) { // down search
				if (p >= lo->m_Data && p < lo->m_Data + ChunkLen) {
					return lo;
				}
				if (lo == loBound) {
					lo = nullptr;
				}
				else {
					--lo;
				}
			}
			if (hi) { // up search
				if (p >= hi->m_Data && p < hi->m_Data + ChunkLen) {
					return hi;
				}
				if (++hi == hiBound) {
					hi = nullptr;
				}
			}
		}
		return nullptr;
	}

	void FixedAlloc::doDeallocate(void* p)
	{
		assert(p >= m_deallocChunk->m_Data);
		assert(p < m_deallocChunk->m_Data + m_blockSize * m_numChunks);

		// Chunk释放内存块
		m_deallocChunk->dealloc(p, m_blockSize);
		return; //TODO 收缩策略暂时不应用
		// 延时回收Chunk(策略: 有2个chunk全部空闲，则回收一个)
#ifndef USE_EMBEDDED_PTR
		if (m_deallocChunk->m_blockAvaiable == m_numChunks) {
			auto& lastChunk = m_Chunks.back();
			if (&lastChunk == m_deallocChunk) { // 当前就是最后一个chunk
				if (m_Chunks.size() > 1 && m_deallocChunk[-1].m_blockAvaiable == m_numChunks) {
					//free the last chunk
					lastChunk.release();
					m_Chunks.pop_back();
					m_allocChunk = m_deallocChunk = &m_Chunks.front();
				};
			}
			else if (lastChunk.m_blockAvaiable == m_numChunks) { // 最后一个chunk空闲
				lastChunk.release();
				m_Chunks.pop_back();
				m_allocChunk = m_deallocChunk = &m_Chunks.front();
			}
			else {
				std::swap(lastChunk, *m_deallocChunk); // 放到尾部
				m_allocChunk = &m_Chunks.front();
			}
		}
#endif
	}

	ObjAllocator::ObjAllocator(size_t blockSize, size_t maxObjSize)
		: m_blockSize{ blockSize },
		m_maxObjSize{ maxObjSize },
		m_lastAlloc{ nullptr },
		m_lastDealloc{ nullptr }
	{
	}

	void* ObjAllocator::Allocate(size_t numBytes)
	{
		if (numBytes > m_maxObjSize) {
			return ::operator new(numBytes);
		}

		if (m_lastAlloc && m_lastAlloc->blockSize() == numBytes) {
			return m_lastAlloc->allocate();
		}
		auto it = std::lower_bound(m_pool.begin(), m_pool.end(), numBytes);
		if (it == m_pool.end() || it->blockSize() != numBytes) {
			it = m_pool.emplace(it, FixedAlloc{ numBytes });
			m_lastDealloc = &m_pool.front();
		}
		m_lastAlloc = &*it;
		return m_lastAlloc->allocate();
	}

	void ObjAllocator::Deallocate(void* p, size_t size)
	{
		if (size > m_maxObjSize)
			return ::operator delete(p);

		if (m_lastDealloc && m_lastDealloc->blockSize() == size) {
			m_lastDealloc->deallocate(p);
			return;
		}

		auto it = std::lower_bound(m_pool.begin(), m_pool.end(), size);
		assert(it != m_pool.end());
		assert(it->blockSize() == size);
		if (it != m_pool.end() && it->blockSize() == size) {
			m_lastDealloc = &*it;
		}
		m_lastDealloc->deallocate(p);
	}

} // namespace utils
