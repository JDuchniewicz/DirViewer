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

    enum class EFileType
    {
        File = 0,
        Directory,
        Symlink,
        Special, //For "." and ".." OR for something completely different as these two are not needed
        Other,
        Invalid,
    };

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
    struct NodeState
    {
        NodeState(ImVec2 pos) : Position(pos) {};

        ImVec2 Position;
        bool IsRClicked = false;
        bool IsDragged = false;
    };


    static unsigned int nodeColor; //for now it is static but should be probably handled on per-instance base
    static unsigned int lineColor;
};

