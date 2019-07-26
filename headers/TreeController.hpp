#pragma once
 
#include <map>
#include <memory>
#include "imgui.h"

namespace dv
{
    class Tree;
    
    class TreeController
    {
        public:
            TreeController(std::unique_ptr<Tree> tree);
            ~TreeController();
            void DrawTree();


        private:
            std::unique_ptr<Tree> CurrentTree;
            std::map<unsigned int, ImVec2> NodeLocations;
    };
}
