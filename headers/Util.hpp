#pragma once

#include <Defines.hpp>
#include "imgui_internal.h"

namespace dv
{
    constexpr auto screenSizeX = 1280.0f;
    constexpr auto screenSizeY = 720.0f;

    enum class EOperationType
    {
        Add = 0,
        Remove,
        Reparent,
        Invalid
    };

    enum class EConnectionType
    {
        Normal = 0,
        Hidden,
        Future,
        Removed,
        Invisible,
    };

    //For "." and ".." OR for something completely different as these two are not needed
    // SPECIAL is currently for root only SPECIAL nodes are not movable
    // _COUNT is for compatibility with EnumIterator helper class
    #define E_FILE_TYPE \
    X(File) \
    X(Directory) \
    X(Symlink) \
    X(Special) \
    X(Other) \
    X(Invalid) \
    X(_COUNT)

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

    // IMGUI EXTENSIONS
    // Hackish, as most things related to dragging in imgui
    static inline bool IsItemHoveredFor(float duration) { return GImGui->HoveredIdTimer >= duration; }

    inline auto operator+=(ImVec2& a, ImVec2 b) { a.x += b.x; a.y += b.y; }
    inline auto operator-=(ImVec2& a, ImVec2 b) { a.x -= b.x; a.y -= b.y; }
    template<typename T>
    auto operator*=(ImVec2& a, T m) { a.x *= m; a.y *= m; }
    template<typename T>
    auto operator/(ImVec2 a, T d) { return ImVec2 { a.x / d, a.y / d}; }
    inline auto operator-(ImVec2 a, ImVec2 b) { return ImVec2 { a.x - b.x, a.y - b.y }; }
    inline auto operator+(ImVec2 a, ImVec2 b) { return ImVec2 { a.x + b.x, a.y + b.y }; }
    inline std::ostream& operator<<(std::ostream& ostream, const ImVec2 a) { ostream<< "{ " << a.x << ", " << a.y << " }"; return ostream; }


    static inline ImVec2 Clamp(const ImVec2 vec) //TODO: add bounds and make clamping smart taking them into account
    {
        return ImVec2 { std::clamp(vec.x, 0.0f, screenSizeX), std::clamp(vec.y, 0.0f, screenSizeY) };
    }
    
    static inline ImVec2 CalculateMiddle(const std::string& name, float multiplier)
    {
        ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
        textSize *= multiplier;
        return textSize / 2.0f;
    }

    // TODO: this should be changed to something more conformant with newer C++ instead of old C-style enums
    // as well as more extensible (not just one set of operations)
    enum ENodeStateFlags
    {
        ENodeState_None             = 0,
        ENodeState_RClicked         = 1 << 0,           // Node right-clicked
        ENodeState_Dragged          = 1 << 1,           // Node dragged
        ENodeState_Collapsed        = 1 << 2,           // Node Collapsed (being a parent)
        ENodeState_Hidden           = 1 << 3,           // Node Collapsed (being a child)
        ENodeState_Detached         = 1 << 4,           // Node in a detached state (may be dragged but does not have to be)
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

    constexpr float HOVERING_PIN_DURATION = 1.5f;
};
// TODO: wrap into some util namespace

