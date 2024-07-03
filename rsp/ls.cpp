#include "../state.hpp"
#include "../jit_decl.h"

#ifdef TRACE_COP2
#include <stdio.h>
#define TRACE_LS(op) printf(#op " v%u, %u, %d(r%u)\n", rt, e, offset, base)
#else
#define TRACE_LS(op) ((void)0)
#endif

template <int N>
struct RSPVector
{
	uint16_t e[8 * N];
};

#define USE_VEC_OPTS defined(__clang__) || defined(__GNUC__)

#ifdef USE_VEC_OPTS
using Vec16b = uint8_t __attribute__((vector_size(16)));
using Vec8b = uint8_t __attribute__((vector_size(8)));
#endif

#define READ_VEC_U8(vec, addr) (reinterpret_cast<const uint8_t *>(vec.e)[MES(addr)])
#define WRITE_VEC_U8(vec, addr, data) (reinterpret_cast<uint8_t *>(vec.e)[MES(addr)] = data)

#define IMPL_LS(op) \
template<unsigned e> \
void JIT_DECL RSP_##op(RSP::CPUState *rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<0>(RSP::CPUState *rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<1>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<2>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<3>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<4>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<5>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<6>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<7>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<8>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<9>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<10>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<11>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<12>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<13>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<14>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template void JIT_DECL RSP_##op<15>(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base); \
template <unsigned e>                                                                       \
	void JIT_DECL RSP_##op(RSP::CPUState *rsp, unsigned rt, int offset, unsigned base)

namespace LS
{
	// Using mostly Ares' implementation as a base here

	// Load 8-bit
	IMPL_LS(LBV)
	{
		TRACE_LS(LBV);
		unsigned addr = (rsp->sr[base] + offset * 1) & 0xfff;
		reinterpret_cast<uint8_t *>(rsp->cp2.regs[rt].e)[MES(e)] = READ_MEM_U8(rsp->dmem, addr);
	}

	// Store 8-bit
	IMPL_LS(SBV)
	{
		TRACE_LS(SBV);
		unsigned addr = (rsp->sr[base] + offset * 1) & 0xfff;
		uint8_t v = reinterpret_cast<uint8_t *>(rsp->cp2.regs[rt].e)[MES(e)];

#ifdef INTENSE_DEBUG
		fprintf(stderr, "SBV: 0x%x (0x%x)\n", addr, v);
#endif

		WRITE_MEM_U8(rsp->dmem, addr, v);
	}

	// Load 16-bit
    IMPL_LS(LSV)
	{
		TRACE_LS(LSV);
		unsigned addr = rsp->sr[base] + offset * 2;
	    const unsigned end = (e > 14) ? 16 : (e + 2);
		for (unsigned i = e; i < end; i++)
			WRITE_VEC_U8(rsp->cp2.regs[rt], i & 0xf, READ_MEM_U8(rsp->dmem, addr++ & 0xfff));
	}

	// Store 16-bit
	IMPL_LS(SSV)
	{
		TRACE_LS(SSV);
		unsigned addr = (rsp->sr[base] + offset * 2) & 0xfff;
		uint8_t v0 = reinterpret_cast<uint8_t *>(rsp->cp2.regs[rt].e)[MES(e)];
		uint8_t v1 = reinterpret_cast<uint8_t *>(rsp->cp2.regs[rt].e)[MES((e + 1) & 0xf)];

#ifdef INTENSE_DEBUG
		fprintf(stderr, "SSV: 0x%x (0x%x, 0x%x)\n", addr, v0, v1);
#endif

		WRITE_MEM_U8(rsp->dmem, addr, v0);
		WRITE_MEM_U8(rsp->dmem, (addr + 1) & 0xfff, v1);
	}

	// Load 32-bit
	IMPL_LS(LLV)
	{
		TRACE_LS(LLV);
		unsigned addr = rsp->sr[base] + offset * 4;
	    const unsigned end = (e > 12) ? 16 : (e + 4);
		for (unsigned i = e; i < end; i++)
			WRITE_VEC_U8(rsp->cp2.regs[rt], i & 0xf, READ_MEM_U8(rsp->dmem, addr++ & 0xfff));
	}

	// Store 32-bit
	IMPL_LS(SLV)
	{
		TRACE_LS(SLV);
		unsigned addr = (rsp->sr[base] + offset * 4) & 0xfff;

#ifdef INTENSE_DEBUG
		fprintf(stderr, "SLV 0x%x, e = %u\n", addr, e);
#endif

		for (unsigned i = e; i < e + 4; i++)
			WRITE_MEM_U8(rsp->dmem, addr++, READ_VEC_U8(rsp->cp2.regs[rt], i & 0xf));
	}

	// Load 64-bit
	IMPL_LS(LDV)
	{
		TRACE_LS(LDV);
		unsigned addr = rsp->sr[base] + offset * 8;
		const unsigned end = (e > 8) ? 16 : (e + 8);
		for (unsigned i = e; i < end; i++)
			WRITE_VEC_U8(rsp->cp2.regs[rt], i & 0xf, READ_MEM_U8(rsp->dmem, addr++ & 0xfff));
	}

	// Store 64-bit
	IMPL_LS(SDV)
	{
		TRACE_LS(SDV);
		unsigned addr = (rsp->sr[base] + offset * 8) & 0xfff;

#ifdef INTENSE_DEBUG
		fprintf(stderr, "SDV 0x%x, e = %u\n", addr, e);
#endif

		// Handle illegal scenario.
		if (__builtin_expect((e > 8) || (e & 1) || (addr & 1), false))
	    {
			for (unsigned i = 0; i < 8; i++)
			{
				WRITE_MEM_U8(rsp->dmem, (addr + i) & 0xfff,
				             reinterpret_cast<const uint8_t *>(rsp->cp2.regs[rt].e)[MES((e + i) & 0xf)]);
			}
		}
		else
		{
		    unsigned _e = e;
		    _e >>= 1;
			for (unsigned i = 0; i < 4; i++)
			{
				WRITE_MEM_U16(rsp->dmem, (addr + 2 * i) & 0xfff, rsp->cp2.regs[rt].e[_e + i]);
			}
		}
	}

	// Load 8x8-bit into high bits.
	IMPL_LS(LPV)
	{
		TRACE_LS(LPV);
		unsigned addr = (rsp->sr[base] + offset * 8) & 0xfff;
		const unsigned index = (addr & 7) - e;
		addr &= ~7;

		auto *reg = rsp->cp2.regs[rt].e;
		for (unsigned i = 0; i < 8; i++)
			reg[i] = READ_MEM_U8(rsp->dmem, (addr + (i + index & 0xf)) & 0xfff) << 8;
	}

	IMPL_LS(SPV)
	{
		TRACE_LS(SPV);
		unsigned addr = (rsp->sr[base] + offset * 8) & 0xfff;
		auto *reg = rsp->cp2.regs[rt].e;

		for (unsigned i = e; i < e + 8; i++) {
			const unsigned shift = ((i & 0xf) < 8) ? 8 : 7;
			WRITE_MEM_U8(rsp->dmem, addr++ & 0xfff, int16_t(reg[i & 7]) >> shift);
		}
	}

	// Load 8x8-bit into high bits, but shift by 7 instead of 8.
	// Was probably used for certain fixed point algorithms to get more headroom without
	// saturation, but weird nonetheless.
	IMPL_LS(LUV)
	{
		TRACE_LS(LUV);
		unsigned addr = (rsp->sr[base] + offset * 8) & 0xfff;
		const unsigned index = (addr & 7) - e;
		addr &= ~7;

		auto *reg = rsp->cp2.regs[rt].e;
		for (unsigned i = 0; i < 8; i++)
			reg[i] = READ_MEM_U8(rsp->dmem, (addr + (i + index & 0xf)) & 0xfff) << 7;
	}

	IMPL_LS(SUV)
	{
		TRACE_LS(SUV);
		unsigned addr = (rsp->sr[base] + offset * 8) & 0xfff;
		auto *reg = rsp->cp2.regs[rt].e;

		for (unsigned i = e; i < e + 8; i++) {
			const unsigned shift = ((i & 0xf) < 8) ? 7 : 8;
			WRITE_MEM_U8(rsp->dmem, addr++ & 0xfff, int16_t(reg[i & 7]) >> shift);
		}
	}

	// Load 8x8-bits into high bits, but shift by 7 instead of 8.
	// Seems to differ from LUV in that it loads every other byte instead of packed bytes.
	IMPL_LS(LHV)
	{
		TRACE_LS(LHV);
		unsigned addr = rsp->sr[base] + offset * 16;
		const unsigned index = (addr & 7) - e;
		addr &= ~7;

		auto *reg = rsp->cp2.regs[rt].e;
		for (unsigned i = 0; i < 8; i++)
			reg[i] = (uint16_t)READ_MEM_U8(rsp->dmem, (addr + (index + i * 2 & 0xf)) & 0xfff) << 7;
	}

	IMPL_LS(SHV)
	{
		TRACE_LS(SHV);
		unsigned addr = (rsp->sr[base] + offset * 16) & 0xfff;
		const unsigned index = addr & 7;
		addr &= ~7;

		const auto &reg = rsp->cp2.regs[rt];
		for (unsigned i = 0; i < 8; i++)
		{
			const unsigned b = e + (i << 1);
			const uint8_t byte = READ_VEC_U8(reg, b & 0xf) << 1 | READ_VEC_U8(reg, (b + 1) & 0xf) >> 7;
			WRITE_MEM_U8(rsp->dmem, addr + (index + i * 2 & 0xf), byte);
		}
	}

	IMPL_LS(LFV)
	{
		TRACE_LS(LFV);
		RSPVector<1> temp;

		unsigned addr = rsp->sr[base] + offset * 16;
		const unsigned index = (addr & 7) - e;
		const unsigned end = (e > 8) ? 16 : (e + 8);
		addr &= ~7;

		for (unsigned i = 0; i < 4; i++)
		{
			temp.e[i] = (uint16_t)READ_MEM_U8(rsp->dmem, (addr + (index + i * 4 & 0xf)) & 0xfff) << 7;
			temp.e[i+4] = (uint16_t)READ_MEM_U8(rsp->dmem, (addr + (index + i * 4 + 8 & 0xf)) & 0xfff) << 7;
		}

		for (unsigned i = e; i < end; i++)
			WRITE_VEC_U8(rsp->cp2.regs[rt], i, READ_VEC_U8(temp, i));
	}

#define RSP_SFV_CASE(a,b,c,d) \
	WRITE_MEM_U8(rsp->dmem, addr + base, int16_t(reg[a]) >> 7); \
	WRITE_MEM_U8(rsp->dmem, addr + 4 + base, int16_t(reg[b]) >> 7); \
	WRITE_MEM_U8(rsp->dmem, addr + (8 + base & 0xf), int16_t(reg[c]) >> 7); \
	WRITE_MEM_U8(rsp->dmem, addr + (12 + base & 0xf), int16_t(reg[d]) >> 7);

	IMPL_LS(SFV)
	{
		TRACE_LS(SFV);
		unsigned addr = (rsp->sr[base] + offset * 16) & 0xfff;
		base = addr & 7;
		addr &= ~7;

		auto *reg = rsp->cp2.regs[rt].e;
		switch (e)
		{
		case 0:
		case 15:
			RSP_SFV_CASE(0,1,2,3)
			break;
		case 1:
			RSP_SFV_CASE(6,7,4,5)
			break;
		case 4:
			RSP_SFV_CASE(1,2,3,0)
			break;
		case 5:
			RSP_SFV_CASE(7,4,5,6)
			break;
		case 8:
			RSP_SFV_CASE(4,5,6,7)
			break;
		case 11:
			RSP_SFV_CASE(3,0,1,2)
			break;
		case 12:
			RSP_SFV_CASE(5,6,7,4)
			break;
		default:
			WRITE_MEM_U8(rsp->dmem, addr + base, 0);
			WRITE_MEM_U8(rsp->dmem, addr + 4 + base, 0);
			WRITE_MEM_U8(rsp->dmem, addr + (8 + base & 0xf), 0);
			WRITE_MEM_U8(rsp->dmem, addr + (12 + base & 0xf), 0);
			break;
		}
	}

	IMPL_LS(LWV)
	{
		TRACE_LS(LWV);
	    unsigned addr = rsp->sr[base] + offset * 16;
		for (unsigned i = 16 - e; i < 16 + e; i++)
		{
			WRITE_VEC_U8(rsp->cp2.regs[rt], i & 0xf, READ_MEM_U8(rsp->dmem, addr & 0xfff));
			addr += 4;
		}
	}

	IMPL_LS(SWV)
	{
		TRACE_LS(SWV);

		unsigned addr = (rsp->sr[base] + offset * 16) & 0xfff;
		base = addr & 7;
		addr &= ~7;

		for (unsigned i = e; i < e + 16; i++)
			WRITE_MEM_U8(rsp->dmem, addr + (base++ & 0xf), READ_VEC_U8(rsp->cp2.regs[rt], i & 0xf));
	}

	IMPL_LS(LQV)
	{
		TRACE_LS(LQV);
	    unsigned addr = rsp->sr[base] + offset * 16;
		if (__builtin_expect(0 == (addr & 0xf), true))
	    {
		    addr &= 0xfff;
		    unsigned end = 16 + e;
		    if (end > 16)
			    end = 16;

#ifdef USE_VEC_OPTS
			if (0 == e)
		    {
			    Vec16b v = *reinterpret_cast<Vec16b *>(&rsp->dmem[addr / sizeof(uint32_t)]);
			    Vec16b *addrp = reinterpret_cast<Vec16b *>(rsp->cp2.regs[rt].e);
			    *addrp = __builtin_shufflevector(v, v, 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13);
			}
			else
#endif
		    {
			    for (unsigned i = e; i < end; i++)
				    WRITE_VEC_U8(rsp->cp2.regs[rt], i & 0xf, READ_MEM_U8(rsp->dmem, addr++));
			}
		}
		else
	    {
		    unsigned end = 16 + e - (addr & 0xf);
		    if (end > 16)
			    end = 16;

		    for (unsigned i = e; i < end; i++)
			    WRITE_VEC_U8(rsp->cp2.regs[rt], i & 0xf, READ_MEM_U8(rsp->dmem, addr++ & 0xfff));
		}
	}

	IMPL_LS(SQV)
	{
		TRACE_LS(SQV);
		unsigned addr = (rsp->sr[base] + offset * 16) & 0xfff;
	    if (__builtin_expect(0 == (addr & 0xf), true))
	    {
		    unsigned end = e + 16;
#ifdef USE_VEC_OPTS
			if (0 == e)
		    {
			    Vec16b v = *reinterpret_cast<Vec16b *>(rsp->cp2.regs[rt].e);
			    Vec16b *addrp = reinterpret_cast<Vec16b*>(&rsp->dmem[addr / sizeof(uint32_t)]);
			    *addrp = __builtin_shufflevector(v, v, 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13);
			}
			else
#endif
		    {
			    for (unsigned i = e; i < end; i++)
				    WRITE_MEM_U8(rsp->dmem, addr++, READ_VEC_U8(rsp->cp2.regs[rt], i & 15));
			}
		}
		else
	    {
		    const unsigned end = e + (16 - (addr & 15));
		    for (unsigned i = e; i < end; i++)
			    WRITE_MEM_U8(rsp->dmem, addr++, READ_VEC_U8(rsp->cp2.regs[rt], i & 15));
		}
	}

	IMPL_LS(LRV)
	{
		TRACE_LS(LRV);
		unsigned addr = rsp->sr[base] + offset * 16;

	    if (__builtin_expect(0 == (addr & 0xf), true))
	    {
			addr &= 0xfff;
		    constexpr unsigned start = 16 + e;
		    for (unsigned i = start; i < 16; i++)
			    WRITE_VEC_U8(rsp->cp2.regs[rt], i & 0xf, READ_MEM_U8(rsp->dmem, addr++));
	    }
		else
	    {
		    const unsigned start = 16 - ((addr & 0xf) - e);
		    addr &= ~0xf;

		    for (unsigned i = start; i < 16; i++)
			    WRITE_VEC_U8(rsp->cp2.regs[rt], i & 0xf, READ_MEM_U8(rsp->dmem, addr++ & 0xfff));
		}
	}

	IMPL_LS(SRV)
	{
		TRACE_LS(SRV);
		unsigned addr = (rsp->sr[base] + offset * 16) & 0xfff;

		if (__builtin_expect(0 == (addr & 0xf), true))
	    {
		    constexpr unsigned end = e;
		    base = 16;

		    for (unsigned i = e; i < end; i++)
			    WRITE_MEM_U8(rsp->dmem, addr++, READ_VEC_U8(rsp->cp2.regs[rt], i + base & 0xf));
		}
		else
	    {
		    const unsigned end = e + (addr & 0xf);
		    base = 16 - (addr & 0xf);
		    addr &= ~0xf;

		    for (unsigned i = e; i < end; i++)
			    WRITE_MEM_U8(rsp->dmem, addr++, READ_VEC_U8(rsp->cp2.regs[rt], i + base & 0xf));
		}
	}

	IMPL_LS(LTV)
	{
		TRACE_LS(LTV);
		unsigned addr = rsp->sr[base] + offset * 16;
		const unsigned start = addr & ~7;
		const unsigned vt0 = rt & ~7;
		addr = start + ((e + (addr & 8)) & 0xf);
		unsigned j = e >> 1;

		for (unsigned i = 0; i < 16; j++)
		{
			j &= 7;
			WRITE_VEC_U8(rsp->cp2.regs[vt0 + j], i++, READ_MEM_U8(rsp->dmem, addr++ & 0xfff));
			if (addr == start + 16) addr = start;
			WRITE_VEC_U8(rsp->cp2.regs[vt0 + j], i++, READ_MEM_U8(rsp->dmem, addr++ & 0xfff));
			if (addr == start + 16) addr = start;
		}
	}

	IMPL_LS(STV)
	{
		TRACE_LS(STV);
		unsigned _e = e;
		_e &= ~1;
		rt &= ~7;

		unsigned addr = (rsp->sr[base] + offset * 16) & 0xfff;
		unsigned element = 16 - _e;
		base = (addr & 7) - _e;
		addr &= ~7;

		for (unsigned i = rt; i < rt + 8; i++ )
		{
			WRITE_MEM_U8(rsp->dmem, addr + (base++ & 0xf), READ_VEC_U8(rsp->cp2.regs[i], element++ & 0xf));
			WRITE_MEM_U8(rsp->dmem, addr + (base++ & 0xf), READ_VEC_U8(rsp->cp2.regs[i], element++ & 0xf));
		}
	}
}
