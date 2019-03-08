#include <iostream>
#include <deque>
#include <stack>
#include <utility>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "SymbolTableBuilder.h"
#include "utility.h"
#include "Scope.h"
#include "Variable.h"
#include "Function.h"
#include "SymbolTable.h"

#define SA_EXPRESSION_TYPE  "ExpressionType"
#define SA_VALUES           "Values"
#define SA_VALUE            "Value"
#define SA_TOP_STATEMENTS   "TopStatements"
#define SA_EXPR             "Expr"
#define SA_LEFT             "Left"
#define SA_RIGHT            "Right"
#define SA_ARGUMENTS        "ArgumentsIdentifiers"
#define SA_NAME             "Name"
#define SA_BODY             "Body"
#define SA_CONDITION        "Condition"
#define SA_FOR_STATEMENT    "ForStatement"
#define SA_WHILE_STATEMENT  "WhileStatement"
#define SA_IF_STATEMENT     "IfStatement"
#define SA_ELSE_STATEMENT   "ElseStatement"
#define SA_SWITCH_STATEMENT "SwitchStatement"
#define SA_CASE_STATEMENT   "CaseStatement"
#define SA_BLOCK_STATEMENT  "BlockStatement"
#define SA_CASES            "Cases"
#define SA_DEFAULT_CASE     "DefaultCase"
#define SA_INITIALIZATION   "Initialization"
#define SA_ITERATION        "Iteration"
#define SA_FUNCTION         "Function"
#define SA_GLOBAL           "Global"

SymbolTableBuilder::SymbolTableBuilder()
{
    m_map = ExpressionTypeMap::getMap();
}

void SymbolTableBuilder::findUsedVariablesInDefinition(const rapidjson::Value& complexValueObject,
                                                       ScopeNode& scope)
{
    std::stack<const rapidjson::Value*> stack;
    stack.push(&complexValueObject);

    while(!stack.empty())
    {
        const rapidjson::Value* value = stack.top();
        stack.pop();

        const rapidjson::Value& expressionObj = (*value)[SA_EXPRESSION_TYPE];
        std::string expressionType = expressionObj.GetString();
        auto type = m_map[expressionType];
        switch(type)
        {
            case eExpressionTypes::UNARY_EXPRESSION:
            {
                const rapidjson::Value* expr = &(*value)[SA_EXPR];
                stack.push(expr);
                break;
            }
            case eExpressionTypes::BINARY_EXPRESSION:
            {
                const rapidjson::Value* left = &(*value)[SA_LEFT];
                stack.push(left);
                const rapidjson::Value* right = &(*value)[SA_RIGHT];
                stack.push(right);
                break;
            }
            case eExpressionTypes::IDENTIFIER_EXPRESSION:
            {
                Variable v;
                v.Line = (*value)["Line"].GetUint();
                v.Column = (*value)["Column"].GetUint();
                v.Name = (*value)["Name"].GetString();
                scope.addUsedVariable(v);
                break;
            }
            default:
            {
                std::cout << "\nexpressionType: " << expressionType << " is discarded.\n";
                break;
            }
        }
    }
}

void SymbolTableBuilder::parseListExpression(const rapidjson::Value &listExpressionObject,
                                             ScopeNode& scope)
{
    for(const rapidjson::Value& value : listExpressionObject.GetArray())
    {
        std::string expressionType = value[SA_EXPRESSION_TYPE].GetString();
        auto type = m_map[expressionType];
        if (type == eExpressionTypes::VARIABLE_DEFINITION_EXPRESSION)
        {
            Variable v;
            v.Line = value["Line"].GetUint();
            v.Column = value["Column"].GetUint();
            v.Name = value["Name"].GetString();

            scope.addDefinedVariable(v);
        }
        const rapidjson::Value& variableValue = value[SA_VALUE];
        findUsedVariablesInDefinition(variableValue, scope);
    }
}

void SymbolTableBuilder::parse(const rapidjson::Value& expression, ScopeNode* globalScope, std::vector<Function>& functions)
{
    std::deque< std::pair<const rapidjson::Value*, ScopeNode*> > queue;
    queue.push_front({&expression, globalScope});

    while(!queue.empty())
    {
        const rapidjson::Value* value = queue.front().first;
        ScopeNode* parentScope = queue.front().second;
        queue.pop_front();

        std::string expressionType = (*value)[SA_EXPRESSION_TYPE].GetString();
        auto type = m_map[expressionType];

        switch (type)
        {
            case eExpressionTypes::VARIABLE_DEFINITION_EXPRESSION:
            {
                Variable v;
                v.Line = (*value)["Line"].GetUint();
                v.Column = (*value)["Column"].GetUint();
                v.Name = (*value)["Name"].GetString();
                parentScope->addDefinedVariable(v);
                break;
            }

            case eExpressionTypes::IDENTIFIER_EXPRESSION:
            {
                Variable v;
                v.Line = (*value)["Line"].GetUint();
                v.Column = (*value)["Column"].GetUint();
                v.Name = (*value)["Name"].GetString();
                parentScope->addUsedVariable(v);
                break;
            }

            case eExpressionTypes::UNARY_EXPRESSION:
            {
                if(value->HasMember(SA_EXPR))
                {
                    const rapidjson::Value& expr = (*value)[SA_EXPR];
                    queue.push_front({&expr, parentScope});
                }
                break;
            }

            case eExpressionTypes::BINARY_EXPRESSION:
            {
                const rapidjson::Value& left = (*value)[SA_LEFT];
                queue.push_front({&left, parentScope});
                const rapidjson::Value& right = (*value)[SA_RIGHT];
                queue.push_front({&right, parentScope});
                break;
            }

            case eExpressionTypes::LIST_EXPRESSION:
            {
                const rapidjson::Value& values = (*value)[SA_VALUES];
                parseListExpression(values, (*parentScope));
                break;
            }
            case eExpressionTypes::FUNCTION_DECLARATION:
            {
                ScopeNode* functionBodyScope = ScopeNode::createNode(SA_FUNCTION);
                functionBodyScope->setParent(parentScope);

                Function f;
                const rapidjson::Value& arguments = expression[SA_ARGUMENTS];
                const rapidjson::Value& functionBodyValues = (expression[SA_BODY])[SA_VALUES];
                f.m_arguments = utility::rapidjsonArrayToVector(arguments);
                f.m_name = expression[SA_NAME].GetString();

                for(const rapidjson::Value& functionBodyValue : functionBodyValues.GetArray())
                {
                    queue.push_front({&functionBodyValue, functionBodyScope});
                }

                f.m_scope = functionBodyScope;
                functions.push_back(std::move(f));

                break;
            }
            case eExpressionTypes::IF_STATEMENT:
            {
                ScopeNode* ifStatementScope = ScopeNode::createNode(SA_IF_STATEMENT);
                ifStatementScope->setParent(parentScope);
                parentScope->addChild(ifStatementScope);

                const rapidjson::Value& ifStatementCondition = (*value)[SA_CONDITION];
                queue.push_front({&ifStatementCondition, ifStatementScope});

                const rapidjson::Value& ifStatementValues = ((*value)[SA_IF_STATEMENT])[SA_VALUES];
                for(const rapidjson::Value& ifStatementValue : ifStatementValues.GetArray())
                {
                    queue.push_front({&ifStatementValue, ifStatementScope});
                }

                if(expression.HasMember(SA_ELSE_STATEMENT))
                {
                    ScopeNode* elseStatementScope = ScopeNode::createNode(SA_ELSE_STATEMENT);
                    elseStatementScope->setParent(parentScope);
                    parentScope->addChild(elseStatementScope);

                    const rapidjson::Value& elseStatementValues = (expression[SA_ELSE_STATEMENT])[SA_VALUES];
                    for(const rapidjson::Value& elseStatementValue : elseStatementValues.GetArray())
                    {
                        queue.push_front({&elseStatementValue, ifStatementScope});
                    }
                }

                break;
            }
            case eExpressionTypes::SWITCH_STATEMENT:
            {
                const rapidjson::Value &switchStatementCondition = (*value)[SA_CONDITION];
                queue.push_front({&switchStatementCondition, parentScope});

                const rapidjson::Value &cases = ((*value)[SA_CASES]);
                for (const rapidjson::Value &caseStatement : cases.GetArray())
                {
                    queue.push_front({&caseStatement, parentScope});
                }

                if(value->HasMember(SA_DEFAULT_CASE))
                {
                    ScopeNode* defaultCaseScope = ScopeNode::createNode(SA_DEFAULT_CASE);
                    defaultCaseScope->setParent(parentScope);
                    parentScope->addChild(defaultCaseScope);

                    const rapidjson::Value& defaultCaseValues = ((*value)[SA_DEFAULT_CASE])[SA_VALUES];
                    for (const rapidjson::Value& defaultCaseValue : defaultCaseValues.GetArray())
                    {
                        queue.push_front({&defaultCaseValue, parentScope});
                    }
                }

                break;
            }
            case eExpressionTypes::CASE_STATEMENT:
            {
                ScopeNode* caseStatementScope = ScopeNode::createNode(SA_CASE_STATEMENT);
                caseStatementScope->setParent(parentScope);
                parentScope->addChild(caseStatementScope);

                const rapidjson::Value& caseStatementCondition = (*value)[SA_CONDITION];
                queue.push_front({&caseStatementCondition, caseStatementScope});

                const rapidjson::Value& caseStatementValues = ((*value)[SA_BODY])[SA_VALUES];
                for(const rapidjson::Value& caseStatementValue : caseStatementValues.GetArray())
                {
                    queue.push_front({&caseStatementValue, caseStatementScope});
                }
                break;
            }
            case eExpressionTypes::FOR_STATEMENT:
            {
                ScopeNode* forStatementScope = ScopeNode::createNode(SA_FOR_STATEMENT);
                forStatementScope->setParent(parentScope);
                parentScope->addChild(forStatementScope);

                const rapidjson::Value& forStatementCondition = ((*value)[SA_CONDITION]);
                queue.push_front({&forStatementCondition, forStatementScope});

                const rapidjson::Value& forStatementIteration = ((*value)[SA_ITERATION]);
                queue.push_front({&forStatementIteration, forStatementScope});

                const rapidjson::Value& init = (*value)[SA_INITIALIZATION];
                if(init.HasMember(SA_VALUES))
                {
                    const rapidjson::Value& forStatementInitializationValues = init[SA_VALUES];
                    for(const rapidjson::Value& forStatementInitializationValue : forStatementInitializationValues.GetArray())
                    {
                        queue.push_front({&forStatementInitializationValue, forStatementScope});
                    }
                }
                else
                {
                    queue.push_front({&init, forStatementScope});
                }

                const rapidjson::Value& forStatementBodyValues = ((*value)[SA_BODY])[SA_VALUES];
                for(const rapidjson::Value& forStatementBodyValue : forStatementBodyValues.GetArray())
                {
                    queue.push_front({&forStatementBodyValue, forStatementScope});
                }
                break;
            }
            case eExpressionTypes::WHILE_STATEMENT:
            {
                ScopeNode* whileStatementScope = ScopeNode::createNode(SA_WHILE_STATEMENT);
                whileStatementScope->setParent(parentScope);
                parentScope->addChild(whileStatementScope);

                const rapidjson::Value& whileStatementCondition = ((*value)[SA_CONDITION]);
                queue.push_front({&whileStatementCondition, whileStatementScope});

                const rapidjson::Value& whileStatementBodyValues = ((*value)[SA_BODY])[SA_VALUES];
                for(const rapidjson::Value& whileStatementBodyValue : whileStatementBodyValues.GetArray())
                {
                    queue.push_front({&whileStatementBodyValue, whileStatementScope});
                }
                break;
            }
            case eExpressionTypes::BLOCK_STATEMENT:
            {
                ScopeNode* blockStatement = ScopeNode::createNode(SA_BLOCK_STATEMENT);
                blockStatement->setParent(parentScope);
                parentScope->addChild(blockStatement);

                const rapidjson::Value& blockStatementValues = (*value)[SA_VALUES];
                for(const rapidjson::Value& blockStatementValue : blockStatementValues.GetArray())
                {
                    queue.push_front({&blockStatementValue, blockStatement});
                }

                break;
            }
            default:
            {
                std::cout << "\nexpressionType: " << expressionType << " is discarded.\n";
            }
        }
    }
}

int SymbolTableBuilder::buildSymbolTable(rapidjson::Document& document, SymbolTable& st)
{
    if(document.HasMember(SA_EXPRESSION_TYPE))
    {
        std::string expressionType = document[SA_EXPRESSION_TYPE].GetString();
        if(expressionType == SA_TOP_STATEMENTS)
        {
            ScopeNode* globalScope = ScopeNode::createNode(SA_GLOBAL);

            const rapidjson::Value& values = document[SA_VALUES];
            std::vector<Function> functions;
            for (const rapidjson::Value& value : values.GetArray())
            {
                parse(value, globalScope, functions);
            }

            auto defined = globalScope->getDefined();
            for(auto& elem : defined)
            {
                st.vt.m_defined[elem.Name] = elem;
            }

            auto used = globalScope->getUsed();
            for(auto& elem : used)
            {
                st.vt.m_used[elem.Name] = elem;
            }

            for(auto& function : functions)
            {
                st.ft.m_table[function.m_name] = std::move(function);
            }
        }
    }

    return 1;
}