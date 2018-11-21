#include <sstream>

#include "Registers.hpp"

#include "../LtKinc/ltkinc.h"

using namespace std;

RegistersX86::RegistersX86(KDebugContext * kDebugContext)
{
	_regs[REG_EAX] = new Reg32("eax", kDebugContext->eax);
	_regs[REG_EBX] = new Reg32("ebx", kDebugContext->ebx);
	_regs[REG_ECX] = new Reg32("ecx", kDebugContext->ecx);
	_regs[REG_EDX] = new Reg32("edx", kDebugContext->edx);

	_regs[REG_ESP] = new Reg32("esp", kDebugContext->esp);
	_regs[REG_EBP] = new Reg32("ebp", kDebugContext->ebp);
	_regs[REG_EDI] = new Reg32("edi", kDebugContext->edi);
	_regs[REG_ESI] = new Reg32("esi", kDebugContext->esi);

	_regs[REG_EIP] = new Reg32("eip", kDebugContext->eip);

	_regs[REG_CS] = new Reg32("cs", kDebugContext->cs);
	_regs[REG_DS] = new Reg32("ds", kDebugContext->ds);
	_regs[REG_SS] = new Reg32("ss", kDebugContext->ss);
	_regs[REG_GS] = new Reg32("gs", kDebugContext->gs);
	_regs[REG_FS] = new Reg32("fs", kDebugContext->fs);
	_regs[REG_ES] = new Reg32("es", kDebugContext->es);

	_regs[REG_EFLAG] = new Reg32("eflag", kDebugContext->eflags);

	_regs[REG_CR0] = new Reg32("eflag", kDebugContext->cr0);
	_regs[REG_CR2] = new Reg32("cr2", kDebugContext->cr2);
	_regs[REG_CR3] = new Reg32("cr3", kDebugContext->cr3);
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
		ss << it.second->ToString;

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