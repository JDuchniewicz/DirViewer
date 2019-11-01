#include "Node.hpp"

using namespace dv;

Node::Node(const std::string& name, EConnectionType connT, EFileType fType, long size) : Name(name), ConnnectionType(connT), Type(fType)
{

}

Node::~Node()
{
    for(auto& child : Children) 
        delete child;
    Children.clear(); //check if correct approach
}

void Node::Reparent(Node* newParent)
{
    Node* oldParent = Parent;
    if(oldParent)
    {
        auto it = std::find(oldParent->Children.begin(), oldParent->Children.end(), this);
        oldParent->Children.erase(it);
    }
    Parent = newParent;
}

void Node::RemoveChild(Node* child)
{
    auto it = std::find(Children.begin(), Children.end(), child);
    Children.erase(it);
}