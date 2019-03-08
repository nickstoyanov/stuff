#include <iostream>

#include "Analyzer.h"

std::vector<std::string> Analyzer::analyze(rapidjson::Document* m_ast,
                                           SymbolTable* st)
{
    auto checks = Check::checks();
    std::vector<std::string> results;
    results.reserve(checks.size());

    for (auto& check : checks)
    {
        results.push_back(check->execute(m_ast, st));
    }

    for(auto& ref : results)
    {
        std::cout << ref << '\n';
    }

    return results;
}