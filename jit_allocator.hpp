#pragma once

#include <vector>
#include <stdint.h>

namespace RSP
{
namespace JIT
{
class Allocator
{
public:
	Allocator() = default;
	~Allocator();
	void operator=(const Allocator &) = delete;
	Allocator(const Allocator &) = delete;

	void *allocate_code(size_t size);
	void *allocate_data(size_t size);
	static bool commit(void *code, size_t code_size
				     , void* data, size_t data_size);

private:
	struct Block
	{
		uint8_t *code = nullptr;
		size_t size = 0;
		size_t offset = 0;
	};
	std::vector<Block> code_blocks;
	std::vector<Block> data_blocks;

	static void *allocate(std::vector<Block>& blocks, size_t size);
	static Block reserve_block(size_t size);
};
}
}
