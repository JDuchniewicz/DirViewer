#pragma once

#include "IFileSystem.hpp"
#include "Util.hpp"

namespace dv
{
    class UnixFileSystem : public IFileSystem
    {
        public:
        UnixFileSystem();
        ~UnixFileSystem();
        virtual int GetDataStartingFrom(const std::string& prefix, const std::string& rootName, std::unique_ptr<Tree>& outTree, unsigned int callerIndex) override;
        virtual bool Write(unsigned int callerIndex) override;
        virtual bool RefreshState() override;
        virtual bool ObtainAdditionalData() override;
        
        private:
        static EFileType ConvertFileType(unsigned char fType);

    };
}