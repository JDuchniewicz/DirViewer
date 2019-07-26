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
            bool AddNode(Node* node, unsigned int parentID);
            bool RemoveNode(unsigned int ID); //add removing discriminator
        private:
            Node* Root;
            
    };
}
