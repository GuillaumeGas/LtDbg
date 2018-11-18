#pragma once

#define COMMANDS_LIST                   \
	COMMAND(CMD_CONNECT,   "connect")   \
	COMMAND(CMD_STEP,      "s")         \
	COMMAND(CMD_CONTINUE,  "c")         \
	COMMAND(CMD_QUIT,      "q")         \
	COMMAND(CMD_REGISTERS, "r")		    \
	COMMAND(CMD_DISASS,    "d")		    \
	COMMAND(CMD_UNKNOWN,   "<unknown>") \
	COMMAND(CMD_END,       "<end>" )    \

enum Command
{
#define COMMAND(name, _) name,
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
	COMMANDS_LIST
#undef COMMAND
#undef COMMAND_KEYWORD
};

static Command GetCmdFromStr(const std::string & str) {
#define COMMAND(name, value)			\
	    if (str == value) return name;
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
	COMMANDS_LIST
#undef LEX_TOKEN_KEYWORD
#undef LEX_TOKEN
		return CMD_UNKNOWN;
}