#pragma once
 
#include <map>
#include <memory>
#include "imgui.h"
#include "IFileSystem.hpp"

namespace dv
{
    class Tree;
    
    class TreeController
    {
        public:
            explicit TreeController(std::weak_ptr<IFileSystem> fs);
            ~TreeController();
            void DrawTree();
            void Update();


        private:
            std::unique_ptr<Tree> CurrentTree;
            std::weak_ptr<IFileSystem> FileSystem;
    };
}
