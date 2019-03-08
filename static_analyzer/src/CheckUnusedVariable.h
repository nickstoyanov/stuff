#pragma once

#include "Check.h"
#include "SymbolTable.h"

class CheckUnusedVariable : public Check
{
public:
    CheckUnusedVariable();
    void checkGlobalVariables(rapidjson::Document& doc, rapidjson::Value* output);
    void checkFunctions(rapidjson::Document& doc, rapidjson::Value* output);

    virtual std::string execute(rapidjson::Document* ast,
                                SymbolTable* st) override;
};