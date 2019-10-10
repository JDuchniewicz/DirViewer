#pragma once

#include "IFileSystem.hpp"
#include "Util.hpp"

//TODO: create a thread for it, to not stall drawing (it will be done on main thread)
//TODO: change interface functions and implement them
namespace dv
{
    class UnixFileSystem : public IFileSystem
    {
        public:
        UnixFileSystem();
        ~UnixFileSystem();
        virtual int GetDataStartingFrom(const std::string& srcPath, std::unique_ptr<Tree>& outTree, unsigned int callerIndex) override;
        virtual int MakeFile(const std::string& path, EFileType fType) override;
        virtual bool Write(unsigned int callerIndex) override;
        virtual bool RefreshState() override;
        virtual bool ObtainAdditionalData() override;
        
        private:
        static EFileType ConvertFileType(unsigned char fType);

    };
}