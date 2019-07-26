#include "Tree.hpp"
#include "Node.hpp"

using namespace dv;

Tree::Tree(Node* root) : Root(root)
{

}

Tree::~Tree()
{
    //for now do manual cleanup
    if(Root)
    {
        delete Root;
        Root = nullptr;
    }
}

bool Tree::AddNode(Node* node, unsigned int parentID)
{
    if(!Root) 
    {
        //add this node as root
    }
    //else we need to reparent it by finding parent with given ID
    return true;
}

bool Tree::RemoveNode(unsigned int ID)
{
    return true;
}
