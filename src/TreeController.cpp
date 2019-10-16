#include "TreeController.hpp"
#include "Tree.hpp"
#include "EnumUtils.hpp"
#include "IterationUtils.hpp"

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
    RootPath = "./testDir"; //root path will be obtained from modal, or last opened file at this tab
    FileSystem.lock()->GetDataStartingFrom(RootPath, CurrentTree, index); //FIXME - should be not done in constructor
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

    if(NeedsRedrawing)
        UpdateTreeNodeStates();
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
            float childOffset = 0.0f;//FIXME: this needs fixes for various cases, not very robust
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
                std::cout << "Drawing node: " << child->Name << " with location x: " << childPos.x << " y: " << childPos.y << std::endl;
                NodeState childState { Clamp(childPos)};
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
            //EConnectionType connnectionType = child->ConnnectionType; //add different connection types
            //change line colour to connectionType, moreover when lines are drawn they should pass through centre of whole node( needs to be calculated for each) TODO: tweak till looks fine
            drawList->AddLine(ImVec2 { state.Position.x + CalculateMiddle(node->Name, 0.5f).x, state.Position.y },
                              ImVec2 { childState.Position.x + CalculateMiddle(child->Name, 0.5f).x, childState.Position.y }, lineColor, 11.0f);
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
    buttonSize *= 1.5f;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 25.0f);
    ImGui::Button(node->Name.c_str(), buttonSize);

    // BUTTON INPUT HANDLING SECTION
    if(ImGui::IsItemHovered() && IsItemHoveredFor(HOVERING_PIN_DURATION) && (node->Type == EFileType::Directory || node->Type == EFileType::Special) )
    {
        std::cout << "Hovered: " << node->Name << std::endl;
        auto it = std::find_if(NodeStates.begin(), NodeStates.end(), [&](auto&& nodeState)
        {
            return (nodeState.second.Flags & ENodeState_Detached);
        });
        if(it != NodeStates.end())
        {
            if(it->first->Parent == node)
            {
                it->second.Flags ^= ENodeState_Detached;
                NeedsRedrawing = true;
            }
            else if(MoveNode(it->first, node))
            {
                NodeUpdateList.push_back(std::tuple(it->first, node, EOperationType::Reparent));
                it->second.Flags ^= ENodeState_Detached;
                NeedsRedrawing = true;
            }
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
        DrawContextMenu(node, state);

    ImGui::PopStyleVar();
    ImGui::End(); 
}

void TreeController::DrawContextMenu(Node* node, NodeState& state)
{
    // We want to draw context menu only when node has been rightclicked and keep it open until user clicks somewhere else or selects one option
    // then perform correct operation on node reparenting it when done

    //TODO: enum or what?
    unsigned int choice = 0;
    if(ImGui::BeginPopupContextItem(FileTypeToString(node->Type)))
    {
        ImGui::Text("Size: %lu", node->Size);
        if(!(state.Flags & ENodeState_Collapsed))
        {
            if(ImGui::MenuItem("Collapse")) choice = 1;
        }
        else if(ImGui::MenuItem("Expand")) choice = 2;
        // Special nodes are non-movable
        if(node->Type != EFileType::Special) { if(ImGui::MenuItem("Move")) choice = 3; }
        if(ImGui::MenuItem("Remove")) choice = 4;
        if(node->Type == EFileType::Directory)
        {
            if(ImGui::MenuItem("Add new")) choice = 5;
        }

        ImGui::EndPopup();
    }
    switch(choice)
    {
        case 1:
            // Only root node is collapsed, rest is hidden because we cannot uncollapse just one of them (yet?)
            state.Flags |= ENodeState_Collapsed;
            ProcessForEachSubNode(node, 
            [this](Node* child){ NodeStates.at(child).Flags |= ENodeState_Hidden; 
            std::cout << "Collapsing Node: " << child->Name << std::endl;});
            break;
        case 2:
            state.Flags ^= ENodeState_Collapsed;
            ProcessForEachSubNode(node, 
            [this](Node* child){ NodeStates.at(child).Flags ^= ENodeState_Hidden; 
            std::cout << "Expanding Node: " << child->Name << std::endl;});
            break;
        case 3:
            // for now only parent is detached, children are hidden
            state.Flags |= ENodeState_Detached;
            break;
        case 4:
            ImGui::OpenPopup("Remove");
            break;
        case 5:
            ImGui::OpenPopup("Add new");
            break;
        default:
            break;
    }
    if(ImGui::BeginPopupModal("Add new", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        static char name[128] = ""; //FIXE magic number and static, could be done better (not cleared on exit)
        static char* currentItem = nullptr; //statics should be fixed, or reseted apropriately (it can be ommited by creating a state field for each node which is maintained only on-per controller basis)
        static EFileType fType = EFileType::Invalid;
        ImGui::Text("Add new file");
        ImGui::InputText("Name", name,  IM_ARRAYSIZE(name));
        if(ImGui::BeginCombo("File Type", currentItem))
        {
            for(auto fileType : util::IterateEnum<EFileType>())
            {
                bool isSelected = (currentItem == FileTypeToString(fileType));
                if(ImGui::Selectable(FileTypeToString(fileType), isSelected))
                {
                    ImGui::ClearActiveID();
                    currentItem = const_cast<char*>(FileTypeToString(fileType));
                    fType = fileType;
                }
            }
            std::cout << "selected filetype: " << FileTypeToString(fType)<< std:: endl;
            ImGui::EndCombo();
        }
        if(ImGui::Button("Add"))
        {
            if(FileSystem.lock()->MakeFile(StringPathFrom(node) + '/' + name, fType) == 0) // handle error codes by signaling to user with another color
            {
                NodeUpdateList.push_back(std::tuple(new Node(name, GenerateID(), EConnectionType::Normal, fType), node, EOperationType::Add));
                NeedsRedrawing = true;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    if(ImGui::BeginPopupModal("Remove", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::Text("Remove current Node?");
        if(ImGui::Button("Remove"))
        {
            if(FileSystem.lock()->Remove(StringPathFrom(node), node->Type) == 0)
            {
                NodeUpdateList.push_back(std::tuple(node, nullptr, EOperationType::Remove));
                NeedsRedrawing = true;
            } //else inform about failing
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void TreeController::UpdateTreeNodeStates()
{
    for(auto& [node, parent, operation] : NodeUpdateList)
    {
        if(operation == EOperationType::Add)
        {
            CurrentTree->AddNode(node, parent);
            NodeStates.emplace(node, NodeState({0,0}));
        } else if(operation == EOperationType::Remove)
        {
            CurrentTree->RemoveNode(node->ID);
            NodeStates.erase(node);
        } else if(operation == EOperationType::Reparent)
        {
            CurrentTree->AddNode(node, parent);
        }
    }
}

//TODO: optimize, too many structures and probably too hackish
bool TreeController::MoveNode(Node* from, Node* to)
{
    auto lockedFS = FileSystem.lock();
    std::unordered_map<Node*, std::string> newPaths;
    std::queue<Node*> directories;
    std::vector<Node*> otherNodes;
    std::vector<Node*> toRemove;
    newPaths.emplace(from, StringPathFrom(to) + '/' + from->Name);
    if(from->Type == EFileType::Directory)
    {
        directories.push(from);
        toRemove.push_back(from);
        while(!directories.empty())
        {
            Node* current = directories.front();
            directories.pop();
            auto currentPath = newPaths.at(current);
            if(lockedFS->CloneDirectory(StringPathFrom(current), currentPath) != 0)
                return false; // perform some cleanup
            for(const auto& child : current->Children)
            {
                if(child->Type == EFileType::Directory)
                {
                    directories.push(child);
                    toRemove.push_back(child);
                    newPaths.emplace(child, currentPath + '/' + child->Name);
                    std::cout<< "Creating directory: " << currentPath + '/' + child->Name << std::endl;
                }
                else
                    otherNodes.push_back(child);
            }
        }
        for(auto& current : otherNodes)
        {
            std::cout<< "Moving file: " << StringPathFrom(current) << " to " << newPaths.at(current->Parent) + '/' + current->Name << std::endl;
            if(lockedFS->Move(StringPathFrom(current), newPaths.at(current->Parent) + '/' + current->Name) != 0)
                return false;
        }
    }
    else //special case for just one file
    {
        std::cout<< "Moving file: " << StringPathFrom(from) << " to " << newPaths.at(from) << std::endl;
        if(lockedFS->Move(StringPathFrom(from), newPaths.at(from)) != 0)
            return false;
    }

    for(const auto& current : util::Reverse(toRemove))
    {
        std::cout<< "Removing directory: " << StringPathFrom(current) << std::endl;
        if(lockedFS->Remove(StringPathFrom(current), EFileType::Directory) != 0)
            return false;
    }
    // cleanup after old nodes
    return true;
}

std::string TreeController::StringPathFrom(Node* from) const
{
    std::string path = RootPath + '/';
    std::stack<Node*> nodePath = CurrentTree->GetPathToRootFrom(from);
    while(!nodePath.empty())
    {
        Node* current = nodePath.top();
        nodePath.pop();
        path += current->Name + '/';
    }
    path.pop_back();
    std::cout << "Root path to node" << path << std::endl; //TODO: add remapping to absolute path by using CWD
    return path;
}