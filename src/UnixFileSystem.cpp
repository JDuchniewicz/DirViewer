#include "UnixFileSystem.hpp"
#include <queue>
#include <iostream> // for debug`
#include "dirent.h"

using namespace dv;

UnixFileSystem::UnixFileSystem()
{

}

UnixFileSystem::~UnixFileSystem()
{

}

bool UnixFileSystem::GetDataStartingFrom(const std::string& rootdirectory, std::vector<Entry>& entries, unsigned int callerIndex)
{
    std::vector<Entry> entryTree;
    DIR* directory = opendir(rootdirectory.c_str());
    if(!directory)
        return false;
    //finally we have our main directory opened we can proceed with nascent directories in 1by1 manner
    std::queue<Entry> entryQueue;
    Entry root{rootdirectory, EFileType::Directory}; 
    entryQueue.push(root);
    while(!entryQueue.empty())
    {
        Entry current = entryQueue.front();
        entryQueue.pop();
        struct dirent* dirEntry = readdir(directory);
        if(!dirEntry)
           continue; 

    }
     
//    std::cout << "Read Directory with name: " << dirEntry->d_name << std::endl;
    
    return true;
}

bool UnixFileSystem::Write(unsigned int callerIndex)
{

    return true;
}

bool UnixFileSystem::RefreshState()
{

    return true;
}
bool UnixFileSystem::ObtainAdditionalData()
{
    return true;
}