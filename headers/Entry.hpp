#pragma once

#include "Util.hpp"
#include <string>
#include <vector>

namespace dv
{
    struct Entry
    {
        Entry(const std::string& name, EFileType fType, unsigned int size = 4096) : Name(name), Type(fType), Size(size) {};
        std::string Name;
        EFileType Type;
        unsigned int Size; //default size is 4096 for a directory(in bytes) //may be bigger than int?
        //permissons?
        std::vector<Entry> Children;
    };
}