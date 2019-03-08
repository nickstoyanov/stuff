#pragma once

#include <vector>
#include <memory>

#include "Variable.h"

struct ScopeVariables
{
    std::vector<Variable> m_defined;
    std::vector<Variable> m_used;
};

struct ScopeNode
{
public:
    ScopeNode();
    ScopeNode(const std::string& name);
    ScopeNode& operator=(ScopeNode&& node);

    void setName(const std::string& name);
    std::string getName();
    void addChild(ScopeNode* child);
    void addChildren(std::vector<ScopeNode*> children);
    void setParent(ScopeNode* parent);
    ScopeNode* getParent();
    ScopeVariables getCollectiveScope();
    void destroyTree();

    void addDefinedVariable(Variable& v);
    void addUsedVariable(Variable& v);
    std::vector<ScopeNode*> getChildren();
    std::vector<Variable> getDefined();
    std::vector<Variable> getUsed();

    static void destroyNode(ScopeNode* node)
    {
        delete node;
    }
    static ScopeNode* createNode(const std::string& name)
    {
        ScopeNode* node = new ScopeNode(name);
        return node;
    }

    static ScopeNode* createNode()
    {
        ScopeNode* node = new ScopeNode();
        return node;
    }

private:
    ScopeNode* m_parent;
    std::string m_name;
    std::vector<Variable> m_defined;
    std::vector<Variable> m_used;
    std::vector<ScopeNode*> m_children;
};

