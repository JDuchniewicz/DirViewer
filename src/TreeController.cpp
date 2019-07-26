#include "TreeController.hpp"
#include "Tree.hpp"
#include "Node.hpp"

using namespace dv;

TreeController::TreeController(std::weak_ptr<IFileSystem> fs) : FileSystem(fs)
{
    //get all data for tree from filesystem
    //for now just create dummy
    unsigned int rootID = GenerateID();
    CurrentTree = std::make_unique<Tree>(new Node("root", rootID, EConnectionType::Normal));
    CurrentTree->AddNode(new Node("one", GenerateID(), EConnectionType::Normal), rootID);

}

TreeController::~TreeController()
{

}

void TreeController::Update()
{

}