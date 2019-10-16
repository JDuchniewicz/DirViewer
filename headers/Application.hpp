#pragma once

#include <Defines.hpp>
#include "TreeController.hpp"
#include "IFileSystem.hpp"

namespace dv
{
    class Application
    {
        public:
            explicit Application(std::shared_ptr<IFileSystem> fs);
            ~Application();
            void Update();

        private:
            std::shared_ptr<IFileSystem> fileSystem; 
            std::vector<std::unique_ptr<TreeController>> treeControllers;
    };
}