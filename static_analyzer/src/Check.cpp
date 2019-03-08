#include "CheckUndefinedVariable.h"
#include "rapidjson/document.h"

Check::Check(const std::string& name,
             rapidjson::Document* ast,
             SymbolTable* st)
        : m_name(name)
        , m_ast(ast)
        , m_st(st)
{
    auto& instances = checks();

    for(auto iter = instances.begin(); iter != instances.end(); iter++)
    {
        if((*iter)->name() == name)
        {
            return;
        }
    }

    instances.push_back(this);
}

Check::Check(const std::string& name)
        : m_name(name)
        , m_ast(nullptr)
        , m_st(nullptr)
{
    auto& instances = checks();

    for(auto iter = instances.begin(); iter != instances.end(); iter++)
    {
        if((*iter)->name() == name)
        {
            return;
        }
    }

    instances.push_back(this);
}

std::string Check::name()
{
    return m_name;
}

void Check::setAST(rapidjson::Document* ast)
{
    m_ast = ast;
}

void Check::setST(SymbolTable* st)
{
    m_st = st;
}