#include "Registers.hpp"

#include "../LtKinc/ltkinc.h"

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
	_regs[REG_GS] = new Reg32("gs", kDebugContext->gs);
	_regs[REG_FS] = new Reg32("fs", kDebugContext->fs);
	_regs[REG_ES] = new Reg32("es", kDebugContext->es);
}