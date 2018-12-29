#pragma once

#define DEFAULT_ASM_BUFFER_SIZE 20

#define KDBG_RESPONSE_FAILURE         0
#define KDBG_RESPONSE_WITHOUT_CONTEXT 1
#define KDBG_RESPONSE_WITH_CONTEXT    2

#define COMMANDS_LIST                     \
	COMMAND(CMD_CONNECT,     "connect")   \
	COMMAND(CMD_STEP,        "p")         \
	COMMAND(CMD_CONTINUE,    "c")         \
	COMMAND(CMD_QUIT,        "q")         \
	COMMAND(CMD_REGISTERS,   "r")		  \
	COMMAND(CMD_DISASS,      "d")		  \
	COMMAND(CMD_STACK_TRACE, "s")		  \
	COMMAND(CMD_MEMORY,      "m")		  \
	COMMAND(CMD_BP,          "bp")        \
	COMMAND(CMD_BL,          "bl")        \
	COMMAND(CMD_UNKNOWN,     "<unknown>") \
	COMMAND(CMD_END,         "<end>" )    \

enum CommandId
{
#define COMMAND(name, _) name,
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
	COMMANDS_LIST
#undef COMMAND
#undef COMMAND_KEYWORD
} typedef CommandId;

static CommandId GetCmdFromStr(const std::string & str) {
#define COMMAND(name, value)			\
	    if (str == value) return name;
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
	COMMANDS_LIST
#undef COMMAND_KEYWORD
#undef COMMAND
		return CMD_UNKNOWN;
}

//static const char * GetCmdId(CommandId cmdId) {
//	switch (cmdId) {
//#define COMMAND(name, value)			\
//		case name:			\
//		    return #name;
//#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
//		COMMANDS_LIST
//#undef COMMAND_KEYWORD
//#undef COMMAND
//	default:
//		return NULL;
//	}
//}

static const char * GetCmdValue(CommandId cmdId) {
	switch (cmdId) {
#define COMMAND(name, value)			\
		case name:			\
		    return value;
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
		COMMANDS_LIST
#undef COMMAND_KEYWORD
#undef COMMAND
	default:
		return NULL;
	}
}

typedef unsigned char u8;
typedef unsigned char uchar;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

enum BpState
{
	BP_ENABLED,
	BP_DISABLED
} typedef BpState;

enum KeDebugStatus
{
    DBG_STATUS_SUCCESS,
    DBG_STATUS_FAILURE,
    DBG_STATUS_ALREADY_CONNECTED,
	DBG_STATUS_BREAKPOINT_REACHED
} typedef KeDebugStatus;

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

struct KeDebugPacket
{
	unsigned int size;
	u8 * content; // KeDebugRequest ou KeDebugResponse
} typedef KeDebugPacket;

struct KeDebugRequest
{
	CommandId command;
	unsigned int paramSize;
	char * param;
} typedef KeDebugRequest;

struct KeDebugDisassParamReq
{
	unsigned int nbInst;
};

struct KeDebugDisassParamRes
{
	unsigned int size;
	unsigned int startingAddress;
	char * data;
};

struct KeDebugResponseHeader
{
	CommandId command;
	KeDebugStatus status;
	KeDebugContext context;
	unsigned int dataSize;
} typedef KeDebugResponseHeader;

struct KeDebugResponse
{
	KeDebugResponseHeader header;
	char * data;
} typedef KeDebugResponse;