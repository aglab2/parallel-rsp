#pragma once

#define HAS_FASTCALL

#define JIT_DECL __fastcall

#ifdef HAS_FASTCALL
#define JIT_UA_DECL __stdcall
#else
#define JIT_UA_DECL JIT_DECL
#endif
