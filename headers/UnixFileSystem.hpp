#pragma once

#include "IFileSystem.hpp"

struct DIR;

namespace dv
{
    class UnixFileSystem : public IFileSystem
    {
        public:
        UnixFileSystem();
        ~UnixFileSystem();
        virtual bool GetDataStartingFrom(const std::string& rootdirectory, std::vector<Entry>& entries, unsigned int callerIndex) override;
        virtual bool Write(unsigned int callerIndex) override;
        virtual bool RefreshState() override;
        virtual bool ObtainAdditionalData() override;

        private:
        bool FillFileTree(DIR* from, std::vector<Entry> fileTree); //do not know if needed
        std::map<unsigned int, std::vector<Entry>> FileTrees;
    };
}