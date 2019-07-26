#include "TreeController.hpp"
#include <Tree.hpp>

using namespace dv;

TreeController::TreeController(std::unique_ptr<Tree> tree) : CurrentTree(std::move(tree))
{

}

TreeController::~TreeController()
{

}
