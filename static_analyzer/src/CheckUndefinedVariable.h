#pragma once

#include "Check.h"

class CheckUndefinedVariable : public Check
{
public:
    CheckUndefinedVariable();
    void checkGlobalVariables(rapidjson::Document& doc, rapidjson::Value* output);
    void checkFunctions(rapidjson::Document& doc, rapidjson::Value* output);

    virtual std::string execute(rapidjson::Document* ast,
                                SymbolTable* st) override;
};