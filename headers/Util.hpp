#pragma once
#include "imgui.h"

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

    enum class EFileType
    {
        File = 0,
        Directory,
        Symlink,
        Special, //For "." and ".."
        Other,
        Invalid,
    };

    static inline unsigned int GenerateID()
    {
        return ++maxID;
    }


    // each node has only one connection to itself
    // hence storing node connection type in it
    struct NodeState
    {
        NodeState(ImVec2 pos) : Position(pos) {};

        ImVec2 Position;
        bool IsRClicked = false;
        bool IsDragged = false;
    };


    constexpr auto screenSizeX = 1280;
    constexpr auto screenSizeY = 720;
    static unsigned int nodeColor; //for now it is static but should be probably handled on per-instance base
    static unsigned int lineColor;
};

