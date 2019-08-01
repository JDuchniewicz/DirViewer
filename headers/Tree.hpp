#pragma once

#include "Util.hpp"
#include <string>
#include <vector>

namespace dv
{
    class Node;
    class Tree
    {
        public:
            Tree(Node* root);
            ~Tree();
            void AddNode(Node* node, unsigned int parentID);
            bool RemoveNode(unsigned int ID); //add removing discriminator
            std::vector<Node*> GetTreeLevelOrder() const;
        private:
            Node* FindNode(unsigned int ID);
            Node* Root;
            Node* Dummy; //special kind of node to mark invalid
    };
}
