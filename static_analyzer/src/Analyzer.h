#pragma once

#include "Check.h"
#include "Variable.h"
#include "Scope.h"
#include "SymbolTable.h"

class Analyzer
{
public:
    std::vector<std::string> analyze(rapidjson::Document* m_ast,
                                     SymbolTable* st);
};


