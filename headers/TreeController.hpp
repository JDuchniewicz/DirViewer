#include <map>
#include <memory>
#include "imgui.h"

namespace dv
{
    class Tree;
    
    class TreeController
    {
        public:
            TreeController();
            ~TreeController();

        private:
            std::unique_ptr<Tree> CurrentTree;
            std::map<unsigned int, ImVec2> NodeLocations;
    };
}
