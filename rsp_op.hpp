#ifndef RSP_OP_HPP__
#define RSP_OP_HPP__

#include "state.hpp"
#include "jit_decl.h"

extern "C"
{
	int JIT_DECL RSP_MFC0(RSP::CPUState *rsp, unsigned rt, unsigned rd);
	int JIT_DECL RSP_MTC0(RSP::CPUState *rsp, unsigned rd, unsigned rt);

	void JIT_DECL RSP_MTC2(RSP::CPUState *rsp, unsigned rt, unsigned vd, unsigned e);
	void JIT_DECL RSP_MFC2(RSP::CPUState *rsp, unsigned rt, unsigned vs, unsigned e);
	void JIT_DECL RSP_CFC2(RSP::CPUState *rsp, unsigned rt, unsigned rd);
	void JIT_DECL RSP_CTC2(RSP::CPUState *rsp, unsigned rt, unsigned rd);

	void JIT_DECL RSP_CALL(void *opaque, unsigned target, unsigned ret);
	void JIT_DECL RSP_RETURN(void *opaque, unsigned pc);
	void JIT_DECL RSP_EXIT(void *opaque, int mode);
	void JIT_DECL RSP_REPORT_PC(void *rsp, unsigned pc, unsigned instr);

#define DECL_LS(op) void JIT_DECL RSP_##op(RSP::CPUState *rsp, unsigned rt, unsigned element, int offset, unsigned base)

	DECL_LS(LBV);
	DECL_LS(LSV);
	DECL_LS(LLV);
	DECL_LS(LDV);
	DECL_LS(LQV);
	DECL_LS(LRV);
	DECL_LS(LPV);
	DECL_LS(LUV);
	DECL_LS(LHV);
	DECL_LS(LFV);
	DECL_LS(LWV);
	DECL_LS(LTV);

	DECL_LS(SBV);
	DECL_LS(SSV);
	DECL_LS(SLV);
	DECL_LS(SDV);
	DECL_LS(SQV);
	DECL_LS(SRV);
	DECL_LS(SPV);
	DECL_LS(SUV);
	DECL_LS(SHV);
	DECL_LS(SFV);
	DECL_LS(SWV);
	DECL_LS(STV);

#undef DECL_LS

#define DECL_LS0(op) void JIT_DECL RSP_##op##0(RSP::CPUState * rsp, unsigned rt, int offset, unsigned base)
	DECL_LS0(LLV);
	DECL_LS0(LDV);
	DECL_LS0(LQV);
	DECL_LS0(LRV);
	DECL_LS0(LPV);
	DECL_LS0(LUV);
	DECL_LS0(LHV);
	DECL_LS0(LFV);
	DECL_LS0(LWV);
	DECL_LS0(LTV);

	DECL_LS0(SLV);
	DECL_LS0(SDV);
	DECL_LS0(SQV);
	DECL_LS0(SRV);
	DECL_LS0(SPV);
	DECL_LS0(SUV);
	DECL_LS0(SHV);
	DECL_LS0(SFV);
	DECL_LS0(SWV);
	DECL_LS0(STV);
#undef DECL_LS

#define DECL_COP2(op) void JIT_DECL RSP_##op(RSP::CPUState *rsp, unsigned vd, unsigned vs, unsigned vt, unsigned e)
	DECL_COP2(VMULF);
	DECL_COP2(VMULU);
	DECL_COP2(VRNDP);
	DECL_COP2(VMULQ);
	DECL_COP2(VMUDL);
	DECL_COP2(VMUDM);
	DECL_COP2(VMUDN);
	DECL_COP2(VMUDH);
	DECL_COP2(VMACF);
	DECL_COP2(VMACU);
	DECL_COP2(VRNDN);
	DECL_COP2(VMACQ);
	DECL_COP2(VMADL);
	DECL_COP2(VMADM);
	DECL_COP2(VMADN);
	DECL_COP2(VMADH);
	DECL_COP2(VADD);
	DECL_COP2(VSUB);
	DECL_COP2(VABS);
	DECL_COP2(VADDC);
	DECL_COP2(VSUBC);
	DECL_COP2(VSAR);
	DECL_COP2(VLT);
	DECL_COP2(VEQ);
	DECL_COP2(VNE);
	DECL_COP2(VGE);
	DECL_COP2(VCL);
	DECL_COP2(VCH);
	DECL_COP2(VCR);
	DECL_COP2(VMRG);
	DECL_COP2(VAND);
	DECL_COP2(VNAND);
	DECL_COP2(VOR);
	DECL_COP2(VNOR);
	DECL_COP2(VXOR);
	DECL_COP2(VNXOR);
	DECL_COP2(VRCP);
	DECL_COP2(VRCPL);
	DECL_COP2(VRCPH);
	DECL_COP2(VMOV);
	DECL_COP2(VRSQ);
	DECL_COP2(VRSQL);
	DECL_COP2(VRSQH);
	DECL_COP2(VNOP);
	DECL_COP2(RESERVED);
#undef DECL_COP2
}

#endif
