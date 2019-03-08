#include "Function.h"

Function::Function(Function&& other) noexcept
{
    if (this != &other)
    {
        m_name = std::move(other.m_name);
        m_arguments = std::move(other.m_arguments);
        m_scope = other.m_scope;
    }
}

Function& Function::operator=(Function&& other) noexcept
{
    if (this != &other)
    {
        m_name = std::move(other.m_name);
        m_arguments = std::move(other.m_arguments);
        m_scope = other.m_scope;
    }
    return *this;
}