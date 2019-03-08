#pragma once

#include "SymbolTable.h"

class Check
{
public:
    Check(const std::string& name,
          rapidjson::Document* ast,
          SymbolTable* st);
    Check(const std::string& name);
    void setAST(rapidjson::Document* ast);
    void setST(SymbolTable* st);
    std::string name();

    static std::vector<Check*>& checks()
    {
        static std::vector<Check*> checks;
        return checks;
    }

    virtual std::string execute(rapidjson::Document* ast,
                                SymbolTable* st) = 0;
protected:
    std::string m_name;
    rapidjson::Document* m_ast;
    SymbolTable* m_st;
};