#pragma once

#include "Util.hpp"
#include <string>
#include <vector>

namespace dv
{
    // each node has only one connection to itself
    // hence storing node connection type in it
    class Node
    {
        public:
            Node(const std::string& n, unsigned int i, EConnectionType ct);
            ~Node();
            void Reparent(Node* newParent);
            void RemoveChild(Node* child);

//all fields public
            std::string Name; // name of current file
            unsigned int ID;
            EConnectionType ConnnectionType; 
            Node* Parent = nullptr;
            std::vector<Node*> Children;
    };
}