#include "TreeController.hpp"
#include "Tree.hpp"
#include "Node.hpp"
#include <iostream>
using namespace dv;

TreeController::TreeController(std::weak_ptr<IFileSystem> fs) : FileSystem(fs)
{
    //get all data for tree from filesystem
    //for now just create dummy
    unsigned int rootID = GenerateID();
    CurrentTree = std::make_unique<Tree>(new Node("root", rootID, EConnectionType::Normal));
    unsigned int oneID = GenerateID();
    unsigned int twoID = GenerateID();
    unsigned int threeID= GenerateID();
    unsigned int fourID= GenerateID();
    CurrentTree->AddNode(new Node("one", oneID, EConnectionType::Normal), rootID);
    CurrentTree->AddNode(new Node("two", twoID, EConnectionType::Normal), rootID);
    CurrentTree->AddNode(new Node("three", threeID, EConnectionType::Normal), rootID);
    CurrentTree->AddNode(new Node("four", fourID, EConnectionType::Normal), rootID);
    CurrentTree->AddNode(new Node("five", GenerateID(), EConnectionType::Normal), oneID);
    CurrentTree->AddNode(new Node("six", GenerateID(), EConnectionType::Normal), threeID);

    //first draw should assign locations of nodes, subsequent redraws, will only update map and redraw
    DrawTree();
}

TreeController::~TreeController()
{

}

void TreeController::Update()
{

}

void TreeController::DrawTree()
{
    //tree needs smart drawing, with relative offsets from root, and growing with depth, 
    // where to store locations    
    std::vector<Node*> nodeList = CurrentTree->GetTreeLevelOrder();
    if(nodeList.empty())
        return;
    for(const auto& node : nodeList)
    {
        std::cout << node << std::endl;
    }
    
}