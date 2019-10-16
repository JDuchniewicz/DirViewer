#pragma once

#include <Defines.hpp>
#include "Util.hpp"

namespace dv
{
    class Node
    {
        public:
            Node(const std::string& name, unsigned int id, EConnectionType connT, EFileType fType, long size = 4096);
            ~Node();
            void Reparent(Node* newParent);
            void RemoveChild(Node* child);

//all fields public
            std::string Name; // name of current file
            unsigned int ID;
            EConnectionType ConnnectionType; 
            EFileType Type;
            //permissons?
            long Size; //default size is 4096 for a directory(in bytes) //may be bigger than int?
            Node* Parent = nullptr;
            std::vector<Node*> Children;
    };
}