#include "Tree.hpp"
#include "Node.hpp"
#include <queue>

using namespace dv;

Tree::Tree(Node* root) : Root(root)
{
    Dummy = new Node("", 0, EConnectionType::Invisible);
}

Tree::~Tree()
{
    //for now do manual cleanup
    if(Root)
        delete Root;
    delete Dummy;
}

void Tree::AddNode(Node* node, unsigned int parentID)
{
    if(!Root) 
    {
        Root = node;
        return;
    }
    Node* parent = FindNode(parentID);
    if(!parent)
        return;
    parent->Children.push_back(node);
    node->Reparent(parent);
}

bool Tree::RemoveNode(unsigned int ID)
{
    Node* toRemove = FindNode(ID);
    if(toRemove == nullptr)
        return false;
    Node* parent = toRemove->Parent;
    if(parent != nullptr) //not root
    {
        parent->RemoveChild(toRemove);
    }
    else
    {
        Root = nullptr;
    }
    delete toRemove;
    return true;
}

Node* Tree::FindNode(unsigned int ID)
{
    //trigger bfs here
    std::queue<Node*> queue;
    queue.push(Root);
    while(!queue.empty())
    {
        Node* current = queue.front();
        queue.pop();
        if(current->ID == ID)
            return current;
        for(const auto& child : current->Children)
            queue.push(child);
    }
    return nullptr;
}

TreeSpan Tree::GetTreeLevelOrder() const
{
    TreeSpan currentSpan;
    std::queue<Node*> queue;
    currentSpan.LevelNodeCount.push_back(1); //always one root node
    size_t currentLevelNodesCount = 0;
    queue.push(Root);
    queue.push(Dummy);
    while(!queue.empty())
    {
        Node* current = queue.front();
        queue.pop();
        if(current == Dummy)
        {
            if(!queue.empty())
            {
                queue.push(Dummy);
                currentSpan.LevelNodeCount.push_back(currentLevelNodesCount);
                currentLevelNodesCount = 0;
            }
        }
        else
        {
            for(const auto& child : current->Children)
            {
                queue.push(child);
                ++currentLevelNodesCount;
            }
        }
        if(current != Dummy) //no need for dummy nodes in our tree anymore
            currentSpan.Nodes.push_back(current);
    }
    return currentSpan;
}