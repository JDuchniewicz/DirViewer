#include "Application.hpp"
#include "IFileSystem.hpp"

using namespace dv;

Application::Application(std::unique_ptr<IFileSystem> fs) : fileSystem(std::move(fs))
{

}

Application::~Application()
{

}

void Application::Update()
{

}