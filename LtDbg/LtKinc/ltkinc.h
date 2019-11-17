#pragma once

#define DEFAULT_ASM_BUFFER_SIZE 20

#define KDBG_RESPONSE_FAILURE         0
#define KDBG_RESPONSE_WITHOUT_CONTEXT 1
#define KDBG_RESPONSE_WITH_CONTEXT    2

#include <string>

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
	COMMAND(CMD_IDT,         "idt")       \
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
	DBG_STATUS_BREAKPOINT_REACHED,
	DBG_STATUS_WRONG_PARAMETER
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
} typedef KeDebugDisassParamReq;

struct KeDebugDisassParamRes
{
	unsigned int size;
	unsigned int startingAddress;
	char * data;
} typedef KeDebugDisassParamRes;

struct KeDebugMemoryParamReq
{
	unsigned int nbBytes;
	unsigned int startingAddress;
} typedef KeDebugMemoryParamReq;

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

/// @brief Represents an IDT entry, called an Idt descriptor
#pragma pack(push,1)
struct IdtDescriptor
{
    /// @brief Bits 0 to 15 of the interrupt function's offset
    u16 offset0_15;
    /// @brief A code segment selector in GDT or LDT
    u16 selector;
    /// @brief Example : P D P L . 0 1 1 1 . 0 0 0 x . x x x x
    ///        P   : The segment is present in memory (1) or not (0)
    ///        DPL : Specified which privilege level the calling descriptor minimum should have
    ///        x   : unsed bits
    u16 type;
    /// @brief Bits 16 to 31 of the interrupt function's offset
    u16 offset16_31;
};
#pragma pack(pop)