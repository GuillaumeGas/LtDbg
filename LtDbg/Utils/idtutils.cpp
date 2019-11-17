#include "IdtUtils.hpp"
#include "SymbolsHelper.hpp"

#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

std::string IdtUtils::Interpret(IdtDescriptor * const descriptors, const unsigned int size)
{
    stringstream ss;
    unsigned int nbEntries = size / sizeof(IdtDescriptor);

    ss << "<table>";

    ss << "<tr><th>Address</th><th>Routine</th><th>Selector</th><th>Type</th></tr>";

    for(unsigned int i = 0; i < nbEntries; i++)
    {
        unsigned int addr = static_cast<unsigned int>(descriptors[i].offset0_15 | (descriptors[i].offset16_31 << 16));
        ss << "<tr>"
           << "<td>" << addr << "</td>"
           << "<td>" << SymbolsHelper::Instance()->GetSymbolFromAddr(addr).name << "</td>"
           << "<td align=\"center\">" << descriptors[i].selector << "</td>"
           << "<td align=\"center\">" << descriptors[i].type << "</td>"
           << "</tr>";
    }

    ss << "</table>" << endl;

    return ss.str();
}
