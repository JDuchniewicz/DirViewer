#include "Util.hpp"
#include <string>
#include <vector>

namespace dv
{
    // each node has only one connection to itself
    // hence storing node connection type in it
    struct Node
    {
        std::string name; // name of current file
        unsigned int id;
        //ImVec2 pos; // TODO: maybe store additional mapping in controller? Redraw only when adding and removing/expanding
        EConnectionType connnectionType; 
        Node* parent;
        std::vector<Node> children;
    };

    class Tree
    {
        public:
            Tree();
            ~Tree();
            bool AddNode(Node* to);
            bool RemoveNode(Node* from); //add removing discriminator
        private:
            std::vector<Node> nodes;
            Node* root;
            
    };
}
