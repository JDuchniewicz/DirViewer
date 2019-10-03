#pragma once
 
#include <unordered_map>
#include <memory>
#include <queue> //TODO: group includes together or not include them everywhere
#include "Util.hpp"
#include "Node.hpp"
#include "IFileSystem.hpp" 

namespace dv
{
    class Tree;
    
    class TreeController
    {
        public:
            explicit TreeController(std::weak_ptr<IFileSystem> fs, unsigned int index); //TODO: think if index is needed
            ~TreeController();
            void RedrawTree(); // if calculations are too severe, maybe push redrawing logic to a different thread
            void DrawTreeConnections() const;
            void DrawNode(Node* node, NodeState& state);
            void DrawContextMenu(Node* node, NodeState& state, ImVec2 nodeLocation);
            void Update();


        private:
            template<typename T>
            void ProcessForEachSubNode(Node* parent, T function)
            {
                std::queue<Node*> queue;
                for(const auto& child : parent->Children)
                    queue.push(child);
                while(!queue.empty())
                {
                    parent = queue.front();
                    queue.pop();
                    function(parent);
                    for(const auto& child : parent->Children)
                        queue.push(child);
                }
            }
            std::unique_ptr<Tree> CurrentTree;
            std::weak_ptr<IFileSystem> FileSystem;
            std::unordered_map<Node*, NodeState> NodeStates; // IF this will be too much of a bottleneck, just store location in node lel
            bool NeedsRedrawing = true;
            
            float screenX = 0.0f, screenY = 0.0f;
            unsigned int Index;
    };
}
