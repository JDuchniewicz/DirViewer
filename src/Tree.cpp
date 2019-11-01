#include "Tree.hpp"
#include "Node.hpp"

using namespace dv;

Tree::Tree(Node* root) : Root(root)
{
    Dummy = new Node("", EConnectionType::Invisible, EFileType::Invalid);
}

Tree::~Tree()
{
    //for now do manual cleanup
    if(Root)
        delete Root;
    delete Dummy;
}

void Tree::AddNode(Node* node, Node* parent)
{
   parent->Children.push_back(node);
   node->Reparent(parent);
}

bool Tree::RemoveNode(Node* toRemove)
{
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

Node* Tree::FindNode(Node* toFind) const
{
    //trigger bfs here
    std::queue<Node*> queue;
    queue.push(Root);
    while(!queue.empty())
    {
        Node* current = queue.front();
        queue.pop();
        if(current == toFind)
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

std::stack<Node*> Tree::GetPathToRootFrom(Node* from) const
{
    std::stack<Node*> nodes;
    while(from != Root)
    {
        nodes.push(from);
        from = from->Parent;
    }
    return nodes;
}