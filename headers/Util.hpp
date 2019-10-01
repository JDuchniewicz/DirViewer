#pragma once
#include "imgui.h"
#include <algorithm>

namespace dv
{
    constexpr auto screenSizeX = 1280.0f;
    constexpr auto screenSizeY = 720.0f;

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

    //For "." and ".." OR for something completely different as these two are not needed
    #define E_FILE_TYPE \
    X(File) \
    X(Directory) \
    X(Symlink) \
    X(Special) \
    X(Other) \
    X(Invalid)

    #define X(name) name,
    #define E_FILE_TYPE_NAME EFileType

    enum class E_FILE_TYPE_NAME : char
    {
        E_FILE_TYPE
    };
    #undef X

    constexpr const char* FileTypeToString(EFileType fileType) noexcept
    {
    #define X(name) case(E_FILE_TYPE_NAME::name): return #name;
        switch(fileType)
        {
            E_FILE_TYPE
        }
    #undef X
        return "dummy"; //otherwise compiler complains
    }

    static inline unsigned int GenerateID()
    {
        return ++maxID;
    }

    //consider writing a wrapper for 
    static inline ImVec2 Clamp(const ImVec2 vec) //TODO: add bounds and make clamping smart taking them into account
    {
        return ImVec2 { std::clamp(vec.x, 0.0f, screenSizeX), std::clamp(vec.y, 0.0f, screenSizeY) };
    }

    // each node has only one connection to itself
    // hence storing node connection type in it

    // TODO: this should be changed to something more conformant with newer C++ instead of old C-style enums
    enum ENodeStateFlags
    {
        ENodeState_None             = 0,
        ENodeState_RClicked         = 1 << 0,
        ENodeState_Dragged          = 1 << 1,       
        ENodeState_Collapsed        = 1 << 2,
        ENodeState_Hidden           = 1 << 3,
        ENodeState_Detached         = 1 << 4,
    };

    inline constexpr auto operator|=(ENodeStateFlags& a, ENodeStateFlags b) { a = static_cast<ENodeStateFlags>(a | b); }
    inline constexpr auto operator&=(ENodeStateFlags& a, ENodeStateFlags b) { a = static_cast<ENodeStateFlags>(a & b); } // for storing a & b
    inline constexpr auto operator^=(ENodeStateFlags& a, ENodeStateFlags b) { a = static_cast<ENodeStateFlags>(a ^ b); }
    inline constexpr auto operator~(ENodeStateFlags& a) { return static_cast<ENodeStateFlags>(~static_cast<int>(a)); }

    inline constexpr auto ClearFlags(ENodeStateFlags& flags) { flags &= ~flags; }

    struct NodeState
    {
        NodeState(ImVec2 pos) : Position(pos) {};

        ImVec2 Position;
        ENodeStateFlags Flags;
    };


    static unsigned int nodeColor; //for now it is static but should be probably handled on per-instance base
    static unsigned int lineColor;
};

