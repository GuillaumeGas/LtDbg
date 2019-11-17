#pragma once

#include <string>

#include "..\udis\libudis86\extern.h"

class Disassembler
{
public:
	Disassembler();

	void SetInputBuffer(const unsigned char * buffer, size_t size);
	void SetMode(unsigned char mode);
	void SetSyntax(void(*)(struct ud*));

	std::string Disassemble(unsigned int startingAddr, unsigned int nbInstructions);
	// std::string Disassemble(int length);
	// std::string Disassemble(int length1, int length2);

private:
	unsigned char * _buffer;
	ud_t _udObj;
};
