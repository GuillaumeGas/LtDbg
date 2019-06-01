#include <sstream>

#include "Registers.hpp"

#include "../LtKinc/ltkinc.h"

using namespace std;

RegistersX86::RegistersX86(KeDebugContext & kDebugContext)
{
	_regs[REG_EAX] = new Reg32("eax", kDebugContext.eax, REG_GENERAL);
	_regs[REG_EBX] = new Reg32("ebx", kDebugContext.ebx, REG_GENERAL);
	_regs[REG_ECX] = new Reg32("ecx", kDebugContext.ecx, REG_GENERAL);
	_regs[REG_EDX] = new Reg32("edx", kDebugContext.edx, REG_GENERAL);

	_regs[REG_ESP] = new Reg32("esp", kDebugContext.esp, REG_INDEX_AND_POINTERS);
	_regs[REG_EBP] = new Reg32("ebp", kDebugContext.ebp, REG_INDEX_AND_POINTERS);
	_regs[REG_EDI] = new Reg32("edi", kDebugContext.edi, REG_INDEX_AND_POINTERS);
	_regs[REG_ESI] = new Reg32("esi", kDebugContext.esi, REG_INDEX_AND_POINTERS);

	_regs[REG_EIP] = new Reg32("eip", kDebugContext.eip, REG_INDEX_AND_POINTERS);

	_regs[REG_CS] = new Reg16("cs", kDebugContext.cs, REG_SELECTOR);
	_regs[REG_DS] = new Reg16("ds", kDebugContext.ds, REG_SELECTOR);
	_regs[REG_SS] = new Reg16("ss", kDebugContext.ss, REG_SELECTOR);
	_regs[REG_GS] = new Reg16("gs", kDebugContext.gs, REG_SELECTOR);
	_regs[REG_FS] = new Reg16("fs", kDebugContext.fs, REG_SELECTOR);
	_regs[REG_ES] = new Reg16("es", kDebugContext.es, REG_SELECTOR);

	_regs[REG_EFLAG] = new Reg32("eflag", kDebugContext.eflags, REG_INDICATOR);

	_regs[REG_CR0] = new Reg32("cr0", kDebugContext.cr0, REG_CONTROL);
	_regs[REG_CR2] = new Reg32("cr2", kDebugContext.cr2, REG_CONTROL);
	_regs[REG_CR3] = new Reg32("cr3", kDebugContext.cr3, REG_CONTROL);
}

RegistersX86::~RegistersX86()
{
	for (auto & it : _regs)
	{
		if (it.second != nullptr)
			delete it.second;
	}
}

string RegistersX86::ToString() const
{
	stringstream ss;

	for (auto & it : _regs)
	{
		ss << it.second->ToString();

		if (it.first == REG_EDX
			|| it.first == REG_ESI
			|| it.first == REG_EIP
			|| it.first == REG_ES
			|| it.first == REG_EFLAG
			|| it.first == REG_CR3)
		{
			ss << endl;
		}
		else
		{
			ss << ", ";
		}
	}

	return ss.str();
}

Reg8::Reg8(string name, u8 value, RegTypeId typeId) : value(value)
{
	this->name = name;
	this->sizeId = REG_8;
}

string Reg8::ToString() const
{
	stringstream ss;
	ss << name << " = 0x" << std::hex << value;
    return ss.str();
}

Reg16::Reg16(string name, u16 value, RegTypeId typeId) : value(value)
{
    this->name = name;
    this->sizeId = REG_16;
}

string Reg16::ToString() const
{
	stringstream ss;
	ss << name << " = 0x" << std::hex << value;
	return ss.str();
}

Reg32::Reg32(string name, u32 value, RegTypeId typeId) : value(value)
{
    this->name = name;
    this->sizeId = REG_32;
}

string Reg32::ToString() const
{
	stringstream ss;
	ss << name << " = 0x" << std::hex << value;
	return ss.str();
}

Reg64::Reg64(string name, u64 value, RegTypeId typeId) : value(value)
{
    this->name = name;
    this->sizeId = REG_64;
}

string Reg64::ToString() const
{
	stringstream ss;
	ss << name << " = 0x" << std::hex << value;
	return ss.str();
}