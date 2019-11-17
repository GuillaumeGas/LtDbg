#ifndef IDTUTILS_H
#define IDTUTILS_H

#include "../LtDbg/LtKinc/ltkinc.h"

class IdtUtils
{
public:
    static std::string Interpret(IdtDescriptor * const descriptors, const unsigned int size);
};

#endif // IDTUTILS_H
