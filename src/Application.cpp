#include "Application.hpp"
#include "IFileSystem.hpp"
#include "Tree.hpp"

using namespace dv;

Application::Application(std::shared_ptr<IFileSystem> fs) : fileSystem(std::move(fs))
{
    // parse current dir from filesystem and create nodes
    
    treeControllers.push_back(std::make_unique<TreeController>(fileSystem));

}

Application::~Application()
{
    treeControllers.clear(); //maybe not needed?
}

void Application::Update()
{
    for(const auto& controller : treeControllers)
        controller->Update();
}