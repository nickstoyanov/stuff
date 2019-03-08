#include "SymbolTable.h"


void VariableTable::addDefined(std::vector<Variable>&  defined)
{
    for (const auto& variable : defined)
    {
        const std::string& name = variable.Name;

        if(m_defined.find(name) == m_defined.end())
        {
            m_defined[name] = variable;
        }
    }
}

void VariableTable::addUsed(std::vector<Variable>&  used)
{
    for (const auto& variable : used)
    {
        const std::string& name = variable.Name;

        if(m_used.find(name) == m_defined.end())
        {
            m_used[name] = variable;
        }
    }
}

