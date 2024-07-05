#pragma once

#include <stdint.h>

union PackedCP
{
	struct
	{
		uint8_t rt;
		uint8_t rd;
		uint16_t e;
	};

	uint32_t value;
};
