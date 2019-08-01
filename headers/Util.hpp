#pragma once

namespace dv
{
    namespace
    {
        static unsigned int maxID = 0;
    }
    enum class EConnectionType
    {
        Normal = 0,
        Hidden,
        Future,
        Removed,
        Invisible,
    };

    static inline unsigned int GenerateID()
    {
        return ++maxID;
    }
};

