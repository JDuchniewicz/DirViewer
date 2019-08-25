#include "TreeController.hpp"
#include "Tree.hpp"
#include "Node.hpp"
#include "imgui.h"
#include <cstdlib>

#include <iostream>

using namespace dv;

TreeController::TreeController(std::weak_ptr<IFileSystem> fs) : FileSystem(fs)
{
    //get all data for tree from filesystem
    //for now just create dummy
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

    //first draw should assign locations of nodes, subsequent redraws, will only update map and redraw
    NeedsRedrawing = true;
    screenX = screenSizeX;
    screenY = screenSizeY;
}

TreeController::~TreeController()
{

}

void TreeController::Update()
{
    // Needs redrawing when:
    // 1. app size changes
    // 2. user collapses/expands node
    // 3. user moves/deletes/adds node
    // here mouse dragging needs to be taken into account
    // during drag only dragged node is being redrawn and when it is released it has to be reparented etc
    if(NeedsRedrawing) //FIXME:
        DrawTree();
    else //consider parenting everything to a big window for ease of tab switching
        for(const auto& [node, location] : NodeLocations)
            DrawNode(node, location);
    
}

void TreeController::DrawTree()//rename do ReDrawTree - because it reconstructs localizations etc
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
    DrawNode(root, current); //draw root node
    NodeLocations.emplace(root, current);
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
            ImVec2 parentPos = NodeLocations.at(parent); //TODO: think if this can be optimised?
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
                DrawNode(child, childPos);
                NodeLocations.emplace(child, childPos);
                childPos.x += horizontalOffset;
            }
        }
        currentParentIndex += treeSpan.LevelNodeCount.at(i); //add number of processed parent nodes
    }
    NeedsRedrawing = false;
}

void TreeController::DrawNode(Node* node, ImVec2 location) const
{
    float buttonRadius = 30.0f; //TODO: tweak it?
    location.x -= buttonRadius;
    location.y -= buttonRadius;
    ImGui::SetNextWindowPos(location); // TODO: if already has position when dragged, update its position
    ImGui::Begin(node->Name.c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

    ImVec2 buttonSize = ImGui::CalcTextSize(node->Name.c_str());
    buttonSize.x *= 2;
    buttonSize.y *= 2;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 25.0f);
    //ImGui::SetCursorPos(location);
    if(ImGui::Button(node->Name.c_str(), buttonSize)) //debug
    {
        std::cout << node->Name << std::endl;
    }

    //ImGui::Text("ID: %d", node->ID);
    //ImGui::Text("X: %f, Y: %f", location.x, location.y);

    ImGui::PopStyleVar();
    ImGui::End(); 

    //add dragging, clicking handling later
}