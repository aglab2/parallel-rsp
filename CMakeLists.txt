cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)
project(parallel-rsp LANGUAGES CXX C)

add_definitions(-DPARALLEL_INTEGRATION)

# check for INTERPROCEDURAL_OPTIMIZATION support
if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
    cmake_policy(SET CMP0069 NEW)

    include(CheckIPOSupported)
    check_ipo_supported(RESULT ENABLE_IPO)
    if(ENABLE_IPO)
        message("Interprocedural optimizations enabled")
    endif(ENABLE_IPO)
endif()

if(ENABLE_IPO AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif()

if (CMAKE_COMPILER_IS_GNUCXX OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
    set(PARALLEL_RSP_CXX_FLAGS -Wall -Wextra -Wno-missing-field-initializers -Wno-empty-body -ffast-math -Wno-unused-parameter -msse2)
elseif (MSVC)
    set(PARALLEL_RSP_CXX_FLAGS /D_CRT_SECURE_NO_WARNINGS /wd4267 /wd4244 /wd4309 /wd4005 /MP /DNOMINMAX)
endif()

add_library(parallel-rsp SHARED
		parallel.cpp
		jit_allocator.cpp
		jit_allocator.hpp
		jit_decl.h
		rsp/vfunctions.cpp
		rsp_jit.cpp rsp_jit.hpp
		rsp_disasm.cpp rsp_disasm.hpp
		rsp/ls.cpp rsp/pipeline.h
		rsp/reciprocal.cpp rsp/reciprocal.h
		Zilmar_Rsp.h
		rsp/cp0.cpp rsp/cp2.cpp
		arch/simd/rsp/rsp_core.cpp
		arch/simd/rsp/clamp.h
		arch/simd/rsp/rsp_common.h
		arch/simd/rsp/SSE2NEON.h
		arch/simd/rsp/rsp_impl.h
		arch/simd/rsp/vcr.h
		arch/simd/rsp/vabs.h
		arch/simd/rsp/vadd.h
		arch/simd/rsp/vaddc.h
		arch/simd/rsp/vand.h
		arch/simd/rsp/vch.h
		arch/simd/rsp/vcl.h
		arch/simd/rsp/vcr.h
		arch/simd/rsp/vcmp.h
		arch/simd/rsp/vdivh.h
		arch/simd/rsp/vmac.h
		arch/simd/rsp/vmrg.h
		arch/simd/rsp/vmudh.h
		arch/simd/rsp/vmul.h
		arch/simd/rsp/vmull.h
		arch/simd/rsp/vmulh.h
		arch/simd/rsp/vmuln.h
		arch/simd/rsp/vor.h
		arch/simd/rsp/vrcpsq.h
		arch/simd/rsp/vrsq.h
		arch/simd/rsp/vsub.h
		arch/simd/rsp/vsubc.h
		arch/simd/rsp/vxor.h
		arch/simd/rsp/vmulm.h)

option(PARALLEL_RSP_DEBUG_JIT "Enable the debug JIT." OFF)

target_include_directories(parallel-rsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(parallel-rsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/arch/simd/rsp)
target_compile_options(parallel-rsp PRIVATE ${PARALLEL_RSP_CXX_FLAGS})

option(PARALLEL_RSP_BAKED_LIGHTNING "Use built-in Lightning." ON)

target_compile_definitions(parallel-rsp PUBLIC PARALLEL_RSP_LIGHTNING)
if (PARALLEL_RSP_BAKED_LIGHTNING)
	add_library(lightning STATIC
			lightning/lib/jit_disasm.c
			lightning/lib/jit_memory.c
			lightning/lib/jit_names.c
			lightning/lib/jit_note.c
			lightning/lib/jit_print.c
			lightning/lib/jit_size.c
			lightning/lib/lightning.c)
	target_include_directories(lightning PUBLIC
			${CMAKE_CURRENT_SOURCE_DIR}/lightning/include)
	if (WIN32)
		target_sources(lightning PRIVATE win32/mman/sys/mman.c)
		target_include_directories(lightning PRIVATE win32/mman)
	endif()
	target_compile_options(lightning PRIVATE ${PARALLEL_RSP_CXX_FLAGS})
endif()

target_link_libraries(parallel-rsp PUBLIC lightning)
if (MSVC)
	set_target_properties(parallel-rsp
		PROPERTIES
		VS_PLATFORM_TOOLSET ClangCL )
	set_target_properties(lightning
		PROPERTIES
		VS_PLATFORM_TOOLSET ClangCL )
endif()
