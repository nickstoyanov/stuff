#pragma once

#include <vector>
#include <string>

#include "Scope.h"

struct Function
{
    Function() = default;
    Function(Function&& other) noexcept;
    Function& operator=(Function&& other) noexcept;

    std::string m_name;
    std::vector<std::string> m_arguments;
    ScopeNode* m_scope;
};