#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "rapidjson/document.h"

#include "Scope.h"
#include "Variable.h"
#include "Function.h"

struct VariableTable
{
    VariableTable() = default;
    void addDefined(std::vector<Variable>&  defined);
    void addUsed(std::vector<Variable>&  used);

    std::unordered_map<std::string, Variable> m_defined;
    std::unordered_map<std::string, Variable> m_used;
};

struct FunctionTable
{
    FunctionTable() = default;
    std::unordered_map<std::string, Function> m_table;
};

struct SymbolTable
{
    FunctionTable ft;
    VariableTable vt;
};


