#pragma once

namespace dv
{
    class IFileSystem
    {
        public:
        // Node pointer or file descriptor?
        // depth
        virtual bool GetDataStartingFrom() = 0;

        //Write current changes in filesystem tree hierarchy 
        virtual bool Write() = 0;

        // Refresh current system state (maybe something has changed outside)
        virtual bool RefreshState() = 0;

        //Obtain additional data for current node? (modes etc?)
        virtual bool ObtainAdditionalData() = 0;
    };
};
