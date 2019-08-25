#pragma once

#include "Util.hpp"
#include <string>
#include <vector>

namespace dv
{
    class Node;

    struct TreeSpan
    {
        TreeSpan(const TreeSpan&& other) //for (N)RVO
        {
            Nodes = std::move(other.Nodes);
            LevelNodeCount = std::move(other.LevelNodeCount);
        }

        TreeSpan(){};

        std::vector<Node*> Nodes;
        std::vector<size_t> LevelNodeCount; //holds count of total node at this level (counting from root)
    };

    class Tree
    {
        public:
            Tree(Node* root);
            ~Tree();
            void AddNode(Node* node, unsigned int parentID);
            bool RemoveNode(unsigned int ID); //add removing discriminator
            TreeSpan GetTreeLevelOrder() const;
        private:
            Node* FindNode(unsigned int ID);
            Node* Root;
            Node* Dummy; //special kind of node to mark invalid
    };
}
