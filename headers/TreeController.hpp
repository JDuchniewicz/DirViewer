#pragma once
 
#include <unordered_map>
#include <memory>
#include "Util.hpp"
#include "IFileSystem.hpp" 

namespace dv
{
    class Tree;
    class Node;
    
    class TreeController
    {
        public:
            explicit TreeController(std::weak_ptr<IFileSystem> fs, unsigned int index);
            ~TreeController();
            void RedrawTree();
            void DrawTreeConnections() const;
            void DrawNode(Node* node, NodeState& state);
            void DrawContextMenu(Node* node, ImVec2 nodeLocation) const;
            void Update();


        private:
            std::unique_ptr<Tree> CurrentTree;
            std::weak_ptr<IFileSystem> FileSystem;
            std::unordered_map<Node*, NodeState> NodeStates; // IF this will be too much of a bottleneck, just store location in node lel
            bool NeedsRedrawing = true;
            // when one node is expanded and we click another, we should collapse one currently clicked, hold this info somewhere
            
            float screenX = 0.0f, screenY = 0.0f;
            unsigned int Index;
    };
}
