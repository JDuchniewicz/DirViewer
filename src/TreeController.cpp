#include "TreeController.hpp"
#include "Tree.hpp"
#include "Node.hpp"
#include "imgui.h"
#include <cstdlib>

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
    screenX = ImGui::GetWindowWidth();
    screenY = ImGui::GetWindowHeight();
}

TreeController::~TreeController()
{

}

void TreeController::Update()
{
    //for now update each frame, very costly
    if(NeedsRedrawing) //FIXME:
        DrawTree();
}

void TreeController::DrawTree()
{
    //tree needs smart drawing, with relative offsets from root, and growing with depth, 
    // where to store locations    
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
    float verticalOffset = std::max((std::max(screenY - 50.0f, 0.0f)) / treeSpan.LevelNodeCount.size(), 10.0f); //set it to 10.0f(later set up reasonable constants calculated from window size!!!!) TODO:

    ImVec2 current { screenX / 2.0f, 0 + verticalOffset }; 
    int currentParentIndex = 0;
    Node* root = treeSpan.Nodes.at(0);
    DrawNode(root, current); //draw root node
    NodeLocations.emplace(root->ID, current);
    current.y += verticalOffset;

    //iterate through all nodes at higher level and draw each of their children with proper distance
    for(int i = 0; i < treeSpan.LevelNodeCount.size() - 1; ++i)
    {
        float horizontalOffset = std::max((std::max(screenX - 50.0f, 0.0f)) / treeSpan.LevelNodeCount.at(i + 1), 10.0f); //calculate for next level
        //find parent node position and nr of children and adjust this nodes position accordingly
        //potentially complex offset logic ahead
        for(int j = 0; j < treeSpan.LevelNodeCount.at(i); ++j)
        {
            Node* parent = treeSpan.Nodes.at(currentParentIndex + j);
            ImVec2 parentPos = NodeLocations.at(parent->ID); //TODO: think if this can be optimised?
            float childOffset = 0.0f;
            if(parent->Children.size() % 2 == 0)
                childOffset = -( -0.5f + (parent->Children.size() / 2.0f)) * horizontalOffset; //leftmost horizontal offset (starts negative)
            else
                childOffset = -(parent->Children.size() / 2.0f) * horizontalOffset;

            ImVec2 childPos = parentPos;
            childPos.y += verticalOffset;
            for(const auto& child : parent->Children)
            {
                childPos.x += childOffset;
                //parentPos += ImVec2(verticalOffset, childOffset); Consider overloading ImVec2 for custom class with operators
                DrawNode(child, childPos);
                NodeLocations.emplace(child->ID, childPos);
                childOffset += horizontalOffset;
            }
        }
        currentParentIndex += treeSpan.LevelNodeCount.at(i); //add number of processed parent nodes
    }
}

void TreeController::DrawNode(Node* node, ImVec2 location) const
{
    //For now fix positions in middle of screen
    ImGui::Begin("Main");
    ImGui::Button("Dupa");
    //draw whole nodes and test it

    ImGui::End(); 
}