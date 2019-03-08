#include <stack>

#include "Scope.h"
#include "utility.h"

ScopeNode::ScopeNode(const std::string& name)
{
    m_parent = nullptr;
    m_name = name;
}

ScopeNode::ScopeNode()
{
    m_parent = nullptr;
}

void ScopeNode::setName(const std::string& name)
{
    m_name = name;
}

std::string ScopeNode::getName()
{
    return m_name;
}

std::vector<ScopeNode*> ScopeNode::getChildren()
{
    return m_children;
}

std::vector<Variable> ScopeNode::getDefined()
{
    return m_defined;
}

std::vector<Variable> ScopeNode::getUsed()
{
    return m_used;
}

ScopeNode* ScopeNode::getParent()
{
    return m_parent;
}

void ScopeNode::setParent(ScopeNode* parent)
{
    m_parent = parent;
}

void ScopeNode::addChild(ScopeNode* child)
{
    m_children.push_back(child);
}

void ScopeNode::addDefinedVariable(Variable& v)
{
    m_defined.push_back(v);
}

void ScopeNode::addUsedVariable(Variable& v)
{
    m_used.push_back(v);
}

void ScopeNode::addChildren(std::vector<ScopeNode*> children)
{
    m_children.insert(m_children.end(), children.begin(), children.end());
}

ScopeNode& ScopeNode::operator=(ScopeNode&& node)
{
    if(this != &node)
    {
        m_parent = node.m_parent;
        node.m_parent = nullptr;

        m_defined = std::move(node.m_defined);
        m_used = std::move(node.m_used);
        m_children = std::move(node.m_children);
    }
    return *this;
}

// TODO: a leak exists -> the global scope is not destroyed from the function scopes
void ScopeNode::destroyTree()
{
    std::stack<ScopeNode*> nodeStack;

    std::vector<ScopeNode*> nodes;
    ScopeNode* iter = this;
    nodeStack.push(iter);

    while(!nodeStack.empty())
    {
        ScopeNode* tempNode = nodeStack.top();
        nodeStack.pop();

        for(ScopeNode* node : tempNode->m_children)
        {
            nodeStack.push(iter);
            nodes.push_back(node);
        }
    }

    for(ScopeNode* node : nodes)
    {
        ScopeNode::destroyNode(node);
    }
}

ScopeVariables ScopeNode::getCollectiveScope()
{
    ScopeVariables result;
    ScopeNode* curr = this;

    while(curr != nullptr)
    {
        std::vector<Variable> nodeDefined = curr->getDefined();
        std::vector<Variable> nodeUsed = curr->getUsed();

        result.m_defined.insert(result.m_defined.end(), nodeDefined.begin(), nodeDefined.end());
        result.m_used.insert(result.m_used.end(), nodeUsed.begin(), nodeUsed.end());

        curr = curr->m_parent;
    }
    return result;
}

