#include <stack>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


#include "CheckUndefinedVariable.h"

namespace
{
CheckUndefinedVariable check;
}

CheckUndefinedVariable::CheckUndefinedVariable()
    : Check("CheckUndefinedVariable")
{}

/*
 * Description: check all used variables in the global scope if they are actually defined in the global scope.
 * */
void CheckUndefinedVariable::checkGlobalVariables(rapidjson::Document& doc, rapidjson::Value* output)
{

    auto& defined = m_st->vt.m_defined;
    auto& used = m_st->vt.m_used;

    for(auto& usedVar : used)
    {
        std::string variableName = usedVar.first;
        bool found = false;

        for(auto& defVar : defined)
        {
            if(variableName == defVar.first)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            auto& allocator = doc.GetAllocator();
            std::string& name = usedVar.second.Name;
            int line = usedVar.second.Line;
            int column = usedVar.second.Column;

            rapidjson::Value err(rapidjson::kObjectType);

            rapidjson::Value errName;
            errName.SetString(name.c_str(), name.length(), allocator);
            rapidjson::Value errLine;
            errLine.SetInt(line);
            rapidjson::Value errColumn;
            errColumn.SetInt(column);

            err.AddMember("name", errName, allocator);
            err.AddMember("line", errLine, allocator);
            err.AddMember("column", errColumn, allocator);

            output->PushBack(err, allocator);
        }
    }
}

/*
 * Description: For each function iterate its scope tree and check if there exist a used variable that is not defined in its scope or defined in its parent scopes.
 * */
void CheckUndefinedVariable::checkFunctions(rapidjson::Document& doc, rapidjson::Value* output)
{
    auto& functions = m_st->ft.m_table;

    for(auto& function : functions)
    {
        auto& arguments = function.second.m_arguments;
        ScopeNode* functionScope = function.second.m_scope;

        std::stack<ScopeNode*> stack;
        stack.push(functionScope);

        while(!stack.empty())
        {
            ScopeNode* scope = stack.top();
            stack.pop();

            for(auto& child : scope->getChildren())
            {
                stack.push(child);
            }

            ScopeVariables scopeVariables = scope->getCollectiveScope();

            auto defined = scope->getDefined();
            auto used = scope->getUsed();

            for(auto& usedVar : used)
            {
                bool found = false;

                std::string variableName = usedVar.Name;

                for(auto& defVar : defined)
                {
                    if(variableName == defVar.Name)
                    {
                        found = true;
                        break;
                    }
                }

                if(found)
                {
                    continue;
                }

                for(auto& argName : arguments)
                {
                    if(variableName == argName)
                    {
                        found = true;
                        break;
                    }
                }

                if(found)
                {
                    continue;
                }

                for(auto& scopeDefined : scopeVariables.m_defined)
                {
                    if(variableName == scopeDefined.Name)
                    {
                        found = true;
                        break;
                    }
                }

                if(found)
                {
                    continue;
                }

//                auto it = globalDefined.find(variableName);
//
//                if(it != globalDefined.end())
//                {
//                    found = true;
//                    break;
//                }

                if(!found)
                {
                    auto& allocator = doc.GetAllocator();
                    std::string& name = usedVar.Name;
                    int line = usedVar.Line;
                    int column = usedVar.Column;

                    rapidjson::Value err(rapidjson::kObjectType);

                    rapidjson::Value errName;
                    errName.SetString(name.c_str(), name.length(), allocator);
                    rapidjson::Value errLine;
                    errLine.SetInt(line);
                    rapidjson::Value errColumn;
                    errColumn.SetInt(column);

                    err.AddMember("name", errName, allocator);
                    err.AddMember("line", errLine, allocator);
                    err.AddMember("column", errColumn, allocator);

                    output->PushBack(err, allocator);
                }
            }
        }
    }
}

std::string CheckUndefinedVariable::execute(rapidjson::Document* ast,
                                            SymbolTable* st)
{
    setAST(ast);
    setST(st);

    rapidjson::Document d;
    auto& allocator = d.GetAllocator();
    d.SetObject();
    rapidjson::Value check;
    check.SetString(m_name.c_str(), m_name.length(), allocator);
    d.AddMember("check", check, allocator);

    rapidjson::Value error(rapidjson::kArrayType);

    checkGlobalVariables(d, &error);
    checkFunctions(d, &error);

    d.AddMember("error", error, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);

    return buffer.GetString();
}