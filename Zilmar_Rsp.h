/*
 * RSP Compiler plug in for Project64 (A Nintendo 64 emulator).
 *
 * (c) Copyright 2001 jabo (jabo@emulation64.com) and
 * zilmar (zilmar@emulation64.com)
 *
 * pj64 homepage: www.pj64.net
 * 
 * Permission to use, copy, modify and distribute Project64 in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Project64 is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for Project64 or software derived from Project64.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so if they want them.
 *
 */
#pragma once 

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_WIN32)
#define EXPORT          __declspec(dllexport)
#define CALL            _cdecl
#else
#define EXPORT          __attribute__((visibility("default")))
#define CALL
#endif

/************ Profiling **************/
#define Default_ProfilingOn			FALSE
#define Default_IndvidualBlock		FALSE
#define Default_ShowErrors			FALSE
#define Default_AudioHle			FALSE

#define PLUGIN_TYPE_RSP				1
#define PLUGIN_TYPE_GFX				2
#define PLUGIN_TYPE_AUDIO			3
#define PLUGIN_TYPE_CONTROLLER		4

typedef struct {
    uint16_t Version;        /* Should be set to 0x0101 */
    uint16_t Type;           /* Set to PLUGIN_TYPE_RSP */
	char Name[100];      /* Name of the DLL */

	/* If DLL supports memory these memory options then set them to TRUE or FALSE
	   if it does not support it */
    int NormalMemory;   /* a normal BYTE array */ 
    int MemoryBswaped;  /* a normal BYTE array where the memory has been pre
                              bswap on a dword (32 bits) boundry */
} PLUGIN_INFO;

typedef struct {
    void * hInst;
    int MemoryBswaped;    /* If this is set to TRUE, then the memory has been pre
                              bswap on a dword (32 bits) boundry */
	/*uint8_t * HEADER;*/
    uint8_t * RDRAM;
    uint8_t * DMEM;
    uint8_t * IMEM;

    uint32_t * MI_INTR_REG;

    uint32_t * SP_MEM_ADDR_REG;
    uint32_t * SP_DRAM_ADDR_REG;
    uint32_t * SP_RD_LEN_REG;
    uint32_t * SP_WR_LEN_REG;
    uint32_t * SP_STATUS_REG;
    uint32_t * SP_DMA_FULL_REG;
    uint32_t * SP_DMA_BUSY_REG;
    uint32_t * SP_PC_REG;
    uint32_t * SP_SEMAPHORE_REG;

    uint32_t * DPC_START_REG;
    uint32_t * DPC_END_REG;
    uint32_t * DPC_CURRENT_REG;
    uint32_t * DPC_STATUS_REG;
    uint32_t * DPC_CLOCK_REG;
    uint32_t * DPC_BUFBUSY_REG;
    uint32_t * DPC_PIPEBUSY_REG;
    uint32_t * DPC_TMEM_REG;

	void (*CheckInterrupts)( void );
	void (*ProcessDlist)( void );
	void (*ProcessAlist)( void );
	void (*ProcessRdpList)( void );
	void (*ShowCFB)( void );
} RSP_INFO;

#if defined(__cplusplus)
}
#endif
