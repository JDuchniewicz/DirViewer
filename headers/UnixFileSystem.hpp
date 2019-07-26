#pragma once

#include "IFileSystem.hpp"

namespace dv
{
    class UnixFileSystem : public IFileSystem
    {
        public:
        UnixFileSystem();
        ~UnixFileSystem();
        virtual bool GetDataStartingFrom() override;
        virtual bool Write() override;
        virtual bool RefreshState() override;
        virtual bool ObtainAdditionalData() override;
    };
}