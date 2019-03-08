#include <unordered_map>
#include <string>

#include "rapidjson/document.h"

#include "SymbolTable.h"

enum eExpressionTypes
{
    LIST_EXPRESSION,
    FUNCTION_DECLARATION,
    UNARY_EXPRESSION,
    BINARY_EXPRESSION,
    VARIABLE_DEFINITION_EXPRESSION,
    IDENTIFIER_EXPRESSION,
    BLOCK_STATEMENT,
    EMPTY_EXPRESSION,
    IF_STATEMENT,
    ELSE_STATEMENT,
    SWITCH_STATEMENT,
    CASE_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    LITERAL_NUMBER,
};

class SymbolTableBuilder
{
public:
    SymbolTableBuilder();
    int buildSymbolTable(rapidjson::Document& document, SymbolTable& st);

    struct ExpressionTypeMap
    {
        static std::unordered_map<std::string, eExpressionTypes> getMap()
        {
            static std::unordered_map<std::string, eExpressionTypes> map;
            map["LiteralNumber"] = eExpressionTypes::LITERAL_NUMBER;
            map["ListExpression"] = eExpressionTypes::LIST_EXPRESSION;
            map["FunctionDeclaration"] = eExpressionTypes::FUNCTION_DECLARATION;
            map["UnaryExpression"] = eExpressionTypes::UNARY_EXPRESSION;
            map["BinaryExpression"] = eExpressionTypes::BINARY_EXPRESSION;
            map["VariableDefinitionExpression"] = eExpressionTypes::VARIABLE_DEFINITION_EXPRESSION;
            map["IdentifierExpression"] = eExpressionTypes::IDENTIFIER_EXPRESSION;
            map["BlockStatement"] = eExpressionTypes::BLOCK_STATEMENT;
            map["EmptyExpression"] = eExpressionTypes::EMPTY_EXPRESSION;
            map["IfStatement"] = eExpressionTypes::IF_STATEMENT;
            map["SwitchStatement"] = eExpressionTypes::SWITCH_STATEMENT;
            map["CaseStatement"] = eExpressionTypes::CASE_STATEMENT;
            map["WhileStatement"] = eExpressionTypes::WHILE_STATEMENT;
            map["ForStatement"] = eExpressionTypes::FOR_STATEMENT;
            return map;
        }
    };

private:
    void parse(const rapidjson::Value &topStatement, ScopeNode* globalScope, std::vector<Function>& functions);
    void parseListExpression(const rapidjson::Value &listExpressionObject,
                             ScopeNode& scope);
    void findUsedVariablesInDefinition(const rapidjson::Value& complexValueObject,
                                       ScopeNode& scope);

    std::unordered_map<std::string, eExpressionTypes> m_map;
    FunctionTable m_functions;
    VariableTable m_variables;
};

