#pragma once

#include <kernel/lib/types.h>
#include "ltdbgcommands.h"

#define DEFAULT_ASM_BUFFER_SIZE 20

#define KDBG_RESPONSE_FAILURE         0
#define KDBG_RESPONSE_WITHOUT_CONTEXT 1
#define KDBG_RESPONSE_WITH_CONTEXT    2

enum BpState 
{
	BP_ENABLED,
	BP_DISABLED
} typedef BpState;

enum KeDebugStatus
{
	STATUS_SUCCESS,
	STATUS_FAILURE,
};

struct KeDebugPacket
{
	unsigned int size;
	u8 * content; // KeDebugRequest ou KeDebugResponse
};

struct KeDebugRequest
{
	CommandId command;
	unsigned int paramSize;
	char * param;
} typedef KeDebugRequest;

struct KeDebugResponseHeader
{
	CommandId command;
	KeDebugStatus status;
	KeDebugContext context;
	unsigned int dataSize;
} typedef KeDebugResponse;

struct KeDebugResponse
{
	KeDebugResponseHeader header;
	char * data;
} typedef KeDebugResponse;

struct KeDebugContext
{
	u32 cr3, cr2, cr0;
	u32 gs, fs, es, ds, ss;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 eip;
	u16 cs;
	u32 eflags;
} typedef KeDebugContext;

struct KeBreakpoint
{
	u32 addr;
	BpState state;
	u8 savedInstByte;
	int id;
} typedef KeBreakpoint;

void DbgInit();