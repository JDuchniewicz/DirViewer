#include "TreeController.hpp"
#include "Tree.hpp"
#include "Node.hpp"
#include "Entry.hpp"
#include "imgui.h"
#include <cstdlib>

#include <iostream> //for debug

using namespace dv;

#define DUMMY = 1;

TreeController::TreeController(std::weak_ptr<IFileSystem> fs, unsigned int index) : FileSystem(fs), Index(index)
{
    //get all data for tree from filesystem
    //for now just create dummy
    #ifdef DUMMY
    unsigned int rootID = GenerateID();
    CurrentTree = std::make_unique<Tree>(new Node("root", rootID, EConnectionType::Normal));
    unsigned int oneID = GenerateID();
    unsigned int twoID = GenerateID();
    unsigned int threeID= GenerateID();
    unsigned int fourID= GenerateID();
    CurrentTree->AddNode(new Node("one", oneID, EConnectionType::Normal), rootID);
    CurrentTree->AddNode(new Node("two", twoID, EConnectionType::Normal), rootID);
    CurrentTree->AddNode(new Node("three", threeID, EConnectionType::Normal), rootID);
    CurrentTree->AddNode(new Node("four", fourID, EConnectionType::Normal), rootID);
    CurrentTree->AddNode(new Node("five", GenerateID(), EConnectionType::Normal), oneID);
    CurrentTree->AddNode(new Node("six", GenerateID(), EConnectionType::Normal), threeID);
    #else


    #endif

    //debug checking fs
    std::vector<Entry> systemEntries;
    FileSystem.lock()->GetDataStartingFrom("./testDir", systemEntries, index);

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
                DrawNode(child, childState);
                NodeStates.emplace(child, childState);
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
            ImVec2 childLocation = NodeStates.at(child).Position;
            EConnectionType connnectionType = child->ConnnectionType; //add different connection types
            //change line colour to connectionType, moreover when lines are drawn they should pass through centre of whole node( needs to be calculated for each)
            drawList->AddLine(state.Position, childLocation, lineColor, 11.0f);
        }
    }
}

void TreeController::DrawNode(Node* node, NodeState& state)
{
    float buttonRadius = 30.0f; //TODO: tweak it
    ImVec2 location = state.Position;
    location.x -= buttonRadius;
    location.y -= buttonRadius;
    ImGui::SetNextWindowPos(location); // TODO: if already has position when dragged, update its position
    ImGui::Begin(node->Name.c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

    ImVec2 buttonSize = ImGui::CalcTextSize(node->Name.c_str());
    buttonSize.x *= 2;
    buttonSize.y *= 2;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 25.0f);
    //ImGui::SetCursorPos(location);
    ImGui::Button(node->Name.c_str(), buttonSize);
    //for now just single node dragging
    if(ImGui::IsItemClicked(0))
    {
        state.IsDragged = true;
    }
    else if(state.IsDragged)
    {
        if(ImGui::IsMouseReleased(0))
            state.IsDragged = false;
        state.Position = ImGui::GetIO().MousePos;
    }
    

    if(ImGui::IsItemClicked(1))
    {
        if(!state.IsRClicked) //show 
        {
            DrawContextMenu(node, location);
            state.IsRClicked = true;
        } else //collapse
            state.IsRClicked = false;
        std::cout << node->Name << " state: " << state.IsRClicked<<std::endl;
    }


    //ImGui::Text("ID: %d", node->ID);
    //ImGui::Text("X: %f, Y: %f", location.x, location.y);

    ImGui::PopStyleVar();
    ImGui::End(); 
}

void TreeController::DrawContextMenu(Node* node, ImVec2 nodeLocation) const
{
    // We want to draw context menu only when node has been rightclicked and keep it open until user clicks somewhere else or selects one option
    // then perform correct operation on node reparenting it when done

}