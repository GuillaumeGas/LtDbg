#pragma once

#define COMMAND_LIST                  \
	COMMAND(CMD_CONNECT,  "connect")  \
	COMMAND(CMD_STEP,     "s")        \
	COMMAND(CMD_CONTINUE, "c")        \
	COMMAND(CMD_END,      "<end>" )   \

enum Command
{
#define COMMAND(name, _) name,
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
	COMMAND_LIST
#undef COMMAND
#undef COMMAND_KEYWORD
};