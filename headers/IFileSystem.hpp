#pragma once

#include <string>
#include <memory>

namespace dv
{
    class Tree;
    class IFileSystem
    {
        public:
        // Node pointer or file descriptor?
        // depth
        virtual int GetDataStartingFrom(const std::string& prefix, const std::string& rootName, std::unique_ptr<Tree>& outTree, unsigned int callerIndex) = 0;

        //Write current changes in filesystem tree hierarchy 
        virtual bool Write(unsigned int callerIndex) = 0;

        // Refresh current system state (maybe something has changed outside)
        virtual bool RefreshState() = 0;

        //Obtain additional data for current node? (modes etc?)
        virtual bool ObtainAdditionalData() = 0;
    };
};
