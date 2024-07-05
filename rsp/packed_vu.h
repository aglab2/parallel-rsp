#pragma once

#include <stdint.h>

union PackedVU
{
	struct
	{
		uint16_t vd;
		uint8_t vs;
		uint8_t vt;
	};
	uint32_t value;
};
