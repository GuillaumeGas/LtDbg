#include "Disassembler.hpp"
#include "SymbolsHelper.hpp"
#include "../udis/libudis86/extern.h"

#include <sstream>

#define DEFAULT_MODE   32
#define DEFAULT_SYNTAX UD_SYN_INTEL

Disassembler::Disassembler()
{
	ud_init(&_udObj);
	ud_set_mode(&_udObj, DEFAULT_MODE);
	ud_set_syntax(&_udObj, DEFAULT_SYNTAX);
}

void Disassembler::SetInputBuffer(const unsigned char * buffer, size_t size)
{
	ud_set_input_buffer(&_udObj, buffer, size);
}

void Disassembler::SetMode(unsigned char mode)
{
	ud_set_mode(&_udObj, mode);
}

void Disassembler::SetSyntax(void(*syntax)(struct ud*))
{
	ud_set_syntax(&_udObj, syntax);
}

std::string Disassembler::Disassemble(unsigned int startingAddr, unsigned int nbInstructions)
{
	std::stringstream ss;
	unsigned int i = 0;

	ud_set_pc(&_udObj, startingAddr);

	while (ud_disassemble(&_udObj) && i++ < nbInstructions)
	{
		ud_mnemonic_code instId = ud_insn_mnemonic(&_udObj);
		unsigned int instSize = ud_insn_len(&_udObj);
		if (instId != UD_Iinvalid)
		{
			if (i == 1)
			{
				ss << " -> ";
			}
			else
			{
				ss << "    ";
			}

			ss << std::hex << startingAddr << " : " << ud_insn_asm(&_udObj);

			if (instId == UD_Icall)
			{
				const ud_operand_t * operand = ud_insn_opr(&_udObj, 0);
				unsigned int symAddr = operand->lval.udword + startingAddr + instSize;

				if (operand->type == UD_OP_JIMM)
				{
					ss << " (" << SymbolsHelper::Instance()->GetSymbolFromAddr(symAddr).name << ")";
				}
			}
	
			ss <<  std::endl;
		}
		else
		{
			ss << std::hex << startingAddr << " : ";
			for (unsigned int i = 0; i < instSize; i++)
				ss << "?? ";
			ss << std::endl;
		}
		startingAddr += instSize;
	}

	return ss.str();
}
