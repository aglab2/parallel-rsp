#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/mman.h>
#endif
#include <limits>
#include <algorithm>

#include "jit_allocator.hpp"

#ifndef _WIN32
#include <unistd.h>
static int sPageSize = 0;
static inline int PAGE_SIZE()
{
	if (__builtin_expect(0 == sPageSize, false))
	{
		sPageSize = getpagesize();
	}

	return sPageSize;
}
#else
#define PAGE_SIZE() 4096
#endif

namespace RSP
{
namespace JIT
{
static constexpr bool huge_va = std::numeric_limits<size_t>::max() > 0x100000000ull;
// On 64-bit systems, we will never allocate more than one block, this is important since we must ensure that
// relative jumps are reachable in 32-bits.
// We won't actually allocate 1 GB on 64-bit, but just reserve VA space for it, which we have basically an infinite amount of.
static constexpr size_t block_size = huge_va ? (1024 * 1024 * 1024) : (2 * 1024 * 1024);
Allocator::~Allocator()
{
#ifdef _WIN32
	for (auto &block : code_blocks)
		VirtualFree(block.code, 0, MEM_RELEASE);
	for (auto &block : data_blocks)
		VirtualFree(block.code, 0, MEM_RELEASE);
#else
	for (auto &block : code_blocks)
		munmap(block.code, block.size);
	for (auto &block : data_blocks)
		munmap(block.code, block.size);
#endif
}

static size_t align_page(size_t offset)
{
	return (offset + PAGE_SIZE() - 1) & ~size_t(PAGE_SIZE() - 1);
}

static bool commit_read_write(void *ptr, size_t size)
{
#ifdef _WIN32
	return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) == ptr;
#else
	return mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0;
#endif
}

static bool commit_execute(void *ptr, size_t size)
{
#ifdef _WIN32
	DWORD old_protect;
	return VirtualProtect(ptr, align_page(size), PAGE_EXECUTE, &old_protect) != 0;
#else
	return mprotect(ptr, size, PROT_EXEC) == 0;
#endif
}

static bool commit_read_only(void *ptr, size_t size)
{
#ifdef _WIN32
	DWORD old_protect;
	return VirtualProtect(ptr, align_page(size), PAGE_READONLY, &old_protect) != 0;
#else
	return mprotect(ptr, size, PROT_READ) == 0;
#endif
}

bool Allocator::commit(void *code, size_t code_size, void *data, size_t data_size)
{
	bool ok = true;
	ok = ok ? ok : commit_execute(code, code_size);
	ok = ok ? ok : commit_read_only(data, data_size);
	return ok;
}

void *Allocator::allocate_code(size_t size)
{
	return allocate(code_blocks, size);
}

void *Allocator::allocate_data(size_t size)
{
	return allocate(data_blocks, size);
}

void *Allocator::allocate(std::vector<Block>& blocks, size_t size)
{
	size = align_page(size);
	if (blocks.empty())
		blocks.push_back(reserve_block(std::max(size, block_size)));

	auto *block = &blocks.back();
	if (!block->code)
		return nullptr;

	block->offset = align_page(block->offset);
	if (block->offset + size > block->size)
		block = nullptr;

	if (!block)
	{
		if (huge_va)
			abort();
		blocks.push_back(reserve_block(std::max(size, block_size)));
		block = &blocks.back();
	}

	if (!block || !block->code)
		return nullptr;

	void *ret = block->code + block->offset;
	block->offset += size;

	if (!commit_read_write(ret, size))
		return nullptr;
	return ret;
}

Allocator::Block Allocator::reserve_block(size_t size)
{
	Block block;
#ifdef _WIN32
	block.code = static_cast<uint8_t *>(VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE));
	block.size = size;
	return block;
#else
	block.code = static_cast<uint8_t *>(mmap(nullptr, size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE,
	                                         -1, 0));
	block.size = size;
	return block;
#endif
}
}
}
