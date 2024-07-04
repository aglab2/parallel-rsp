#pragma once

#define ELEMENT_INSTANTIATE(op, FN) \
	FN(op, 0)               \
	FN(op, 1)               \
	FN(op, 2)               \
	FN(op, 3)               \
	FN(op, 4)               \
	FN(op, 5) FN(op, 6) FN(op, 7) FN(op, 8) FN(op, 9) FN(op, 10) FN(op, 11) FN(op, 12) FN(op, 13) FN(op, 14) FN(op, 15)
