#include "Disass.hpp"

#include <sstream>

#define DEFAULT_MODE   32
#define DEFAULT_SYNTAX UD_SYN_INTEL

Disass::Disass()
{
	ud_init(&_udObj);
	ud_set_mode(&_udObj, DEFAULT_MODE);
	ud_set_syntax(&_udObj, DEFAULT_SYNTAX);
}

void Disass::SetInputBuffer(const unsigned char * buffer, size_t size)
{
	ud_set_input_buffer(&_udObj, buffer, size);
}

void Disass::SetMode(unsigned char mode)
{
	ud_set_mode(&_udObj, mode);
}

void Disass::SetSyntax(void(*syntax)(struct ud*))
{
	ud_set_syntax(&_udObj, syntax);
}

std::string Disass::Disassemble(unsigned int startingAddr)
{
	std::stringstream ss;

	ud_set_pc(&_udObj, startingAddr);

	while (ud_disassemble(&_udObj))
	{
		ud_mnemonic_code code = ud_insn_mnemonic(&_udObj);
		unsigned int instSize = ud_insn_len(&_udObj);
		if (code != UD_Iinvalid)
		{
			ss << startingAddr << " : " << ud_insn_asm(&_udObj) << std::endl;
		}
		else
		{
			ss << startingAddr << " : ";
			for (unsigned int i = 0; i < instSize; i++)
				ss << "?? ";
			ss << std::endl;
		}
		startingAddr += instSize;
	}

	return ss.str();
}