#pragma once
 
#include <map>
#include <memory>
#include "imgui.h"
#include "IFileSystem.hpp"

namespace dv
{
    class Tree;
    class Node;
    struct NodeState;
    
    class TreeController
    {
        public:
            explicit TreeController(std::weak_ptr<IFileSystem> fs);
            ~TreeController();
            void RedrawTree();
            void DrawTreeConnections() const;
            void DrawNode(Node* node, ImVec2 location) const;
            void DrawContextMenu(Node* node, ImVec2 nodeLocation) const;
            void Update();


        private:
            std::unique_ptr<Tree> CurrentTree;
            std::weak_ptr<IFileSystem> FileSystem;
            std::map<Node*, NodeState> NodeStates; // IF this will be too much of a bottleneck, just store location in node lel
            bool NeedsRedrawing = true;
            
            float screenX = 0.0f, screenY = 0.0f;
    };
}
