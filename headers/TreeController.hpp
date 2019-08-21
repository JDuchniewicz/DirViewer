#pragma once
 
#include <map>
#include <memory>
#include "imgui.h"
#include "IFileSystem.hpp"

namespace dv
{
    class Tree;
    class Node;
    
    class TreeController
    {
        public:
            explicit TreeController(std::weak_ptr<IFileSystem> fs);
            ~TreeController();
            void DrawTree();
            void DrawNode(Node* node, ImVec2 location) const;
            void Update();


        private:
            std::unique_ptr<Tree> CurrentTree;
            std::weak_ptr<IFileSystem> FileSystem;
            std::map<unsigned int, ImVec2> NodeLocations;
            bool NeedsRedrawing = true;
            
            int screenX, screenY;
    };
}
