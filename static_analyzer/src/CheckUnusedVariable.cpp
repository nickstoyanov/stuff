#include <stack>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "CheckUnusedVariable.h"


namespace
{
CheckUnusedVariable check;
}

CheckUnusedVariable::CheckUnusedVariable()
        : Check("CheckUnusedVariable")
{}

/*
 * Description: check if a variable in the global scope is being used in its scope or in a function. Arguments and defined variables with the same identifier shadow the global variable.
 * */
void CheckUnusedVariable::checkGlobalVariables(rapidjson::Document& doc, rapidjson::Value* output)
{
    // check global variables
    auto& defined = m_st->vt.m_defined;
    auto& used = m_st->vt.m_used;
    auto& functions = m_st->ft.m_table;

    for(auto& defVar : defined)
    {
        std::string variableName = defVar.first;
        bool found = false;

        for(auto& usedVar : used)
        {
            if(variableName == usedVar.first)
            {
                found = true;
                break;
            }
        }

        if(found)
        {
            continue;
        }

        for(auto& function : functions)
        {
            auto& arguments = function.second.m_arguments;
            ScopeNode* functionScope = function.second.m_scope;

            std::stack<ScopeNode*> stack;
            stack.push(functionScope);


            while (!stack.empty())
            {
                ScopeNode *scope = stack.top();
                stack.pop();

                for (auto &child : scope->getChildren())
                {
                    stack.push(child);
                }

                auto scopeDefined = scope->getDefined();
                auto scopeUsed = scope->getUsed();

                bool inUsed=false;
                for (auto& usedVar : scopeUsed)
                {
                    if(variableName == usedVar.Name)
                    {
                        inUsed = true;
                        break;
                    }
                }
                bool inDefined = false;
                for(auto& scopeDef : scopeDefined)
                {
                    if(variableName == scopeDef.Name)
                    {
                        inDefined = true;
                        break;
                    }
                }
                bool inArguments = false;
                for(auto& arg : arguments)
                {
                    if(variableName == arg)
                    {
                        inArguments = true;
                        break;
                    }
                }

                if(inUsed && !inArguments && !inDefined)
                {
                    found = true;
                    break;
                }
            }
        }

        if(!found)
        {
            auto& allocator = doc.GetAllocator();
            std::string& name = defVar.second.Name;
            int line = defVar.second.Line;
            int column = defVar.second.Column;

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
 * Description: Check all defined variables in all scopes of a function if it used in the current scope or in a child scope.
 * */
void CheckUnusedVariable::checkFunctions(rapidjson::Document& doc, rapidjson::Value* output)
{
    auto& functions = m_st->ft.m_table;

    for(auto& function : functions)
    {
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

            auto defined = scope->getDefined();
            auto used = scope->getUsed();

            for(auto& defVar : defined)
            {
                std::stack<ScopeNode*> childStack;
                bool found = false;

                for(auto& usedVar : used)
                {
                    if(usedVar.Name == defVar.Name)
                    {
                        found = true;
                    }
                }

                if(found)
                {
                    continue;
                }

                for(auto& child : scope->getChildren())
                {
                    childStack.push(child);
                }

                while(!childStack.empty())
                {
                    ScopeNode* childScope = childStack.top();
                    childStack.pop();

                    for(auto& child : childScope->getChildren())
                    {
                        childStack.push(child);
                    }

                    auto childDefined = childScope->getDefined();
                    auto childUsed = childScope->getUsed();

                    bool inUsed = false;
                    for(auto& childUsedVar : childUsed)
                    {
                        if(defVar.Name == childUsedVar.Name)
                        {
                            inUsed = true;
                            break;
                        }
                    }

                    bool inDefined = false;
                    for(auto& childDefVar : childDefined)
                    {
                        if(defVar.Name == childDefVar.Name)
                        {
                            inDefined = true;
                            break;
                        }
                    }

                    if(inUsed && !inDefined)
                    {
                        found = true;
                    }
                }

                if(!found)
                {
                    auto& allocator = doc.GetAllocator();
                    std::string& name = defVar.Name;
                    int line = defVar.Line;
                    int column = defVar.Column;

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

std::string CheckUnusedVariable::execute(rapidjson::Document* ast,
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