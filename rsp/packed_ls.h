#pragma once

#include <stdint.h>

union PackedLS
{
	struct
	{
		int16_t offset;
		uint8_t rt;
		uint8_t base;
	};
	uint32_t value;
};
