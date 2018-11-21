#pragma once

#include <string>
#include <map>

#include "../LtKinc/ltkinc.h"

struct KDebugContext;

enum RegId
{
	REG_EAX,
	REG_EBX,
	REG_ECX,
	REG_EDX,

	REG_ESP,
	REG_EBP,
	REG_EDI,
	REG_ESI,

	REG_EIP,

	REG_CS,
	REG_SS,
	REG_DS,
	REG_GS,
	REG_FS,
	REG_ES,

	REG_EFLAG,

	REG_CR0,
	REG_CR1,
	REG_CR2,
	REG_CR3
};

enum RegSizeId
{
	REG_8,
	REG_16,
	REG_32,
	REG_64
};

enum RegTypeId
{
	REG_GENERAL,
	REG_SEGMENT,
	REG_INDEX_AND_POINTERS,
	REG_INDICATOR,
	REG_CONTROL
};

class Register
{
public:
	virtual std::string ToString() const = 0;

	std::string name;
	RegId id;
	RegSizeId sizeId;
	RegTypeId type;
};

class Reg8 : public Register
{
public:
	Reg8(std::string name, u8 value);

	std::string ToString() const;

	u8 value;
};

class Reg16 : public Register
{
public:
	Reg16(std::string name, u16 value);

	std::string ToString() const;

	u16 value;
};

class Reg32 : public Register
{
public:
	Reg32(std::string name, u32 value);

	std::string ToString() const;

	u32 value;
};

class Reg64 : public Register
{
public:
	Reg64(std::string & name, unsigned long long value);

	std::string ToString() const;

	unsigned long long value;
};

class RegistersX86
{
public:
	RegistersX86(KDebugContext * kDebugContext);
	~RegistersX86();

	std::string ToString() const;

private:
	std::map<RegId, Register*> _regs;
};