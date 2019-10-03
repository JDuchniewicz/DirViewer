#include "TreeController.hpp"
#include "Tree.hpp"
#include "imgui.h"
#include <cstdlib>

#include <iostream> //for debug

using namespace dv;

TreeController::TreeController(std::weak_ptr<IFileSystem> fs, unsigned int index) : FileSystem(fs), Index(index)
{
    //get all data for tree from filesystem
    //for now just create dummy
    #if 0
    unsigned int rootID = GenerateID();
    CurrentTree = std::make_unique<Tree>(new Node("root", rootID, EConnectionType::Normal, EFileType::Directory));
    unsigned int oneID = GenerateID();
    unsigned int twoID = GenerateID();
    unsigned int threeID= GenerateID();
    unsigned int fourID= GenerateID();
    CurrentTree->AddNode(new Node("one", oneID, EConnectionType::Normal, EFileType::Directory), rootID);
    CurrentTree->AddNode(new Node("two", twoID, EConnectionType::Normal, EFileType::Directory), rootID);
    CurrentTree->AddNode(new Node("three", threeID, EConnectionType::Normal, EFileType::Directory), rootID);
    CurrentTree->AddNode(new Node("four", fourID, EConnectionType::Normal, EFileType::Directory), rootID);
    CurrentTree->AddNode(new Node("five", GenerateID(), EConnectionType::Normal, EFileType::Directory), oneID);
    CurrentTree->AddNode(new Node("six", GenerateID(), EConnectionType::Normal, EFileType::Directory), threeID);
    #else
    FileSystem.lock()->GetDataStartingFrom("./", "testDir", CurrentTree, index);
    #endif

    //first draw should assign states of nodes, subsequent redraws, will only update map and redraw
    NeedsRedrawing = true;
    screenX = screenSizeX;
    screenY = screenSizeY;
    nodeColor = 0x2A2DF8;
    lineColor = 0xe02af8b; //this color is visible, however shittily, tweak it!
}

TreeController::~TreeController()
{

}

void TreeController::Update()
{
    ImGui::SetNextWindowPos({0.0f, 0.0f});
    ImGui::SetNextWindowSize({screenX, screenY});
    ImGui::Begin("MainWindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
    // Needs redrawing when:
    // 1. app size changes (change needs to be propagated from glfw/imgui)
    // 2. user collapses/expands node
    // 3. user moves/deletes/adds node
    // here mouse dragging needs to be taken into account
    // during drag only dragged node is being redrawn and when it is released it has to be reparented etc
    if(NeedsRedrawing) //FIXME:
        RedrawTree();
    else 
        for(auto& [node, state] : NodeStates)
            DrawNode(node, state);
        
    //Probably push list of nodes that need to be redrawn
    DrawTreeConnections();
    ImGui::End();
}

//Needs to be drawn in a better way, with 2 levels of depth nodes are drawn outside of screen
void TreeController::RedrawTree()//rename do ReDrawTree - because it reconstructs localizations etc
{
    TreeSpan treeSpan = CurrentTree->GetTreeLevelOrder();
    if(treeSpan.Nodes.empty())
        return;

    // This is currently not the best way to do it
    // find maximal width of tree (distance between lowest leftmost and righmost node) (HOW??)
   // find tree height
    // when both are found we must adjust scale of nodes to be sure they fit the screen dimensions
    // each time screen is resized the tree needs to be redrawn (same goes for expanding nodes (collapsing is fine ))
    // Node cannot be smaller than its name?
    // For now do not adjust scale (just draw it properly)
    //How to draw? (depending on children amount adjust distance between nodes) (each height jump adjust current drawing height)o

    //calculate drawing constants
    float verticalOffset = std::max((std::max(screenY * 0.65f, 0.0f)) / treeSpan.LevelNodeCount.size(), 50.0f); //set it to 10.0f(later set up reasonable constants calculated from window size!!!!) TODO:

    ImVec2 current { screenX / 2.0f, 0 + verticalOffset }; 
    int currentParentIndex = 0;
    Node* root = treeSpan.Nodes.at(0);
    NodeState rootState {current};
    DrawNode(root, rootState); //draw root node
    NodeStates.emplace(root, rootState);
    current.y += verticalOffset;

    //iterate through all nodes at higher level and draw each of their children with proper distance
    for(size_t i = 0; i < treeSpan.LevelNodeCount.size() - 1; ++i)
    {
        float horizontalOffset = std::max((std::max(screenX * 0.65f, 0.0f)) / treeSpan.LevelNodeCount.at(i + 1), 50.0f); //calculate for next level
        std::cout << "horizontal offset" << horizontalOffset <<std::endl;
        //find parent node position and nr of children and adjust this nodes position accordingly
        for(size_t j = 0; j < treeSpan.LevelNodeCount.at(i); ++j)
        {
            Node* parent = treeSpan.Nodes.at(currentParentIndex + j);
            ImVec2 parentPos = NodeStates.at(parent).Position; //TODO: think if this can be optimised?
            float childOffset = 0.0f;
            if(parent->Children.size() % 2 == 0)
                childOffset = -( -0.5f + (parent->Children.size() / 2.0f)) * horizontalOffset; //leftmost horizontal offset (starts negative)
            else if(parent->Children.size() > 1)
                childOffset = -(parent->Children.size() / 2.0f) * horizontalOffset;

            std::cout << "i: " << i << " j: " << j << " child offset: " << childOffset << " parent position x: " << parentPos.x << " y: " << parentPos.y << std::endl;
            ImVec2 childPos = parentPos;
            childPos.x -= std::abs(childOffset);
            childPos.y += verticalOffset;
            for(const auto& child : parent->Children)
            {
                //parentPos += ImVec2(verticalOffset, childOffset); Consider overloading ImVec2 for custom class with operators
                std::cout << "Drawing node: " << child->Name << " with location x: " << childPos.x << " y: " << childPos.y << std::endl;
                NodeState childState {childPos};
                auto emplaced = NodeStates.emplace(child, childState);
                if(!emplaced.second)
                    NodeStates.at(child) = childState;
                DrawNode(child, childState);
                childPos.x += horizontalOffset;
            }
        }
        currentParentIndex += treeSpan.LevelNodeCount.at(i); //add number of processed parent nodes
    }
    NeedsRedrawing = false;
}

void TreeController::DrawTreeConnections() const
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    for(const auto& [node, state] : NodeStates)
    {
        for(const auto& child : node->Children)
        {
            auto childState = NodeStates.at(child);
            if((childState.Flags & ENodeState_Hidden) || (childState.Flags & ENodeState_Detached)) // do not draw paths to hidden or detached nodes
                continue;
            EConnectionType connnectionType = child->ConnnectionType; //add different connection types
            //change line colour to connectionType, moreover when lines are drawn they should pass through centre of whole node( needs to be calculated for each)
            drawList->AddLine(state.Position, childState.Position, lineColor, 11.0f);
        }
    }
}

void TreeController::DrawNode(Node* node, NodeState& state)
{
    // WINDOW INPUT HANDLING SECTION
    if(state.Flags & ENodeState_Hidden)
        return;
    
    auto windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground;
    
    //if detached, then position will be mousePosition 
    // until node is attached again (by means of hovering it above its future parent)
    // move all nodes the same amount as mouse moved
    if(state.Flags & ENodeState_Detached)
    {
        windowFlags |= ImGuiWindowFlags_NoMouseInputs; // to prevent this window from overlapping underlying to-be-docked-to window
        ImVec2 difference = ImGui::GetIO().MousePos - state.Position;
        state.Position = Clamp(state.Position + difference);
        // Move each child and clamp on leaving edge of screen //TODO: restore optimal position
        // for optimal position, root distance from child has to be known and adjusted for each child independently
        ProcessForEachSubNode(node, 
                [&, this](Node* child){
                    decltype(auto) childState = NodeStates.at(child);
                    childState.Position = Clamp(childState.Position + difference);
                });
    }

    // IMGUI DRAWING SECTION
    float buttonRadius = 30.0f; //TODO: tweak it
    auto location = state.Position - ImVec2{ buttonRadius, buttonRadius};
    ImGui::SetNextWindowPos(Clamp(location));
    ImGui::Begin(node->Name.c_str(), nullptr, windowFlags);

    ImVec2 buttonSize = ImGui::CalcTextSize(node->Name.c_str());
    buttonSize.x *= 1.5f;
        buttonSize.y *= 1.5f;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 25.0f);
    ImGui::Button(node->Name.c_str(), buttonSize);

    // BUTTON INPUT HANDLING SECTION
    if(ImGui::IsItemHovered() && IsItemHoveredFor(HOVERING_PIN_DURATION))
    {
        std::cout << "Hovered: " << node->Name << std::endl;
        auto it = std::find_if(NodeStates.begin(), NodeStates.end(), [&](auto&& nodeState)
        {
            return (nodeState.second.Flags & ENodeState_Detached);
        });
        if(it != NodeStates.end())
        {
            Node* toParent = it->first;
            CurrentTree->AddNode(toParent, node);
            it->second.Flags ^= ENodeState_Detached;
            NeedsRedrawing = true;
            // TODO: add filesystem call MOVE(from , to)
        }
    }

    // node button like behaviour - being pressed and dragged around
    if(ImGui::IsItemClicked(0))
    {
        state.Flags |= ENodeState_Dragged;
    }
    else if(state.Flags & ENodeState_Dragged)
    {
        if(ImGui::IsMouseReleased(0))
            state.Flags ^= ENodeState_Dragged;
        state.Position = Clamp(ImGui::GetIO().MousePos);
    }
     
   if(!(state.Flags & (ENodeState_Hidden | ENodeState_Detached)))
        DrawContextMenu(node, state, location);

    ImGui::PopStyleVar();
    ImGui::End(); 
}

void TreeController::DrawContextMenu(Node* node, NodeState& state, ImVec2 nodeLocation)
{
    // We want to draw context menu only when node has been rightclicked and keep it open until user clicks somewhere else or selects one option
    // then perform correct operation on node reparenting it when done
    if(ImGui::BeginPopupContextItem())
    {
        // print details about this folder as well as some options that can be changed
        ImGui::Text(FileTypeToString(node->Type)); //Add on hover behaviour?
        ImGui::Text("Size: %lu", node->Size);
        if(!(state.Flags & ENodeState_Collapsed))
        {
            if(ImGui::Button("Collapse"))
            {
                // Only root node is collapsed, rest is hidden because we cannot uncollapse just one of them (yet?)
                state.Flags |= ENodeState_Collapsed;
                ProcessForEachSubNode(node, 
                [this](Node* child){ NodeStates.at(child).Flags |= ENodeState_Hidden; 
                std::cout << "Collapsing Node: " << child->Name << std::endl;});
            }
        }
        else
        {
            if(ImGui::Button("Expand"))
            {
                state.Flags ^= ENodeState_Collapsed;
                ProcessForEachSubNode(node, 
                [this](Node* child){ NodeStates.at(child).Flags ^= ENodeState_Hidden; 
                std::cout << "Expanding Node: " << child->Name << std::endl;});
            }

        }
        if(ImGui::Button("Move"))
        {
            // for now only parent is detached, children are hidden
            state.Flags |= ENodeState_Detached;
        }
        if(ImGui::Button("Remove"))
        {

        }
        ImGui::EndPopup();
    }
}