#include "UnixFileSystem.hpp"
#include "Node.hpp"
#include "Tree.hpp"
#include <queue>
#include <iostream> // for debug
#include <string>
#include "dirent.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include <cerrno>

using namespace dv;

UnixFileSystem::UnixFileSystem()
{

}

UnixFileSystem::~UnixFileSystem()
{

}

//handle ERRNO properly, consider passing int codes above to signal >error
int UnixFileSystem::GetDataStartingFrom(const std::string& srcPath, std::unique_ptr<Tree>& outTree, unsigned int callerIndex)
{
    std::string path = srcPath; //TODO: revise if this can be simplified to avoid costly operations on strings 
    Node* root = new Node(srcPath.substr(srcPath.find_last_of('/') + 1), GenerateID(), EConnectionType::Normal, EFileType::Special); // root node is special for now to prevent unwanted gui behaviour
    std::unique_ptr<Tree> createdTree = std::make_unique<Tree>(root);
    std::queue<std::pair<Node*, const std::string>> directoryQueue;
    DIR* directory = opendir(srcPath.c_str());
    if(!directory)
        return errno;
    // handle root directory
    while(struct dirent* dirEntry = readdir(directory))
    {
        if(strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
            continue;
        std::cout << "Read Node with name: " << dirEntry->d_name << std::endl;
        Node* newNode = new Node(dirEntry->d_name, GenerateID(), EConnectionType::Normal, ConvertFileType(dirEntry->d_type));// add size
        createdTree->AddNode(newNode, root);
        path = (srcPath + '/' + dirEntry->d_name);
        if(dirEntry->d_type == DT_DIR)
        {
            std::cout << "Added directory with path: " << path <<std::endl;
            directoryQueue.push({newNode, path});
        }
        else
        {
            struct stat st;
            stat(path.c_str(), &st);
            std::cout << "Stat'ted file of size: " << st.st_size << std::endl;
            newNode->Size = st.st_size;
        }
    }

    if(closedir(directory) == -1)
    {
        std::cout << "CHECK opening for root: " << directory << std::endl;
        return errno;
    }

    while(!directoryQueue.empty())
    {
        // DECISION
        // create Nodes on the fly and fill tree accordingly, this tree is later moved out to OUT parameter hence refilling currently existing tree
        // if boolean specifying we want only part of the tree is set, we need to unparse the tree outside (or fill it in some nice way on the fly)

        // add all children by exploring this directory and calling a while loop with readdir, for each child which is itself a directory add it to a tree
        // remember about adding nodes to queue and children both, moreover maybe remember about level order? so that we can easily remap it into tree structure
        // this way we introduce 1-1 mapping between data tree(which is assisted with controller itself -locations) and data from FS (maybe it should be fused into one?)

        auto currentDirectory = directoryQueue.front();
        Node* parent = currentDirectory.first;
        directoryQueue.pop();
        // open currently picked up node
        directory = opendir(currentDirectory.second.c_str());
        if(!directory)
        {
            std::cout << "CHECK opening for directory: " << currentDirectory.second<< std::endl;
            return errno;
        }
        while(struct dirent* dirEntry = readdir(directory)) //refactor this to separate private function 
        {
            if(strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
                continue;
            std::cout << "Read Directory with name: " << dirEntry->d_name << std::endl;
            Node* newNode = new Node(dirEntry->d_name, GenerateID(), EConnectionType::Normal, ConvertFileType(dirEntry->d_type));// add size
            createdTree->AddNode(newNode, parent);
            if(dirEntry->d_type == DT_DIR)
            {
                path = (currentDirectory.second + '/' + dirEntry->d_name);
                std::cout << "Added directory with path: " << path <<std::endl;
                directoryQueue.push({newNode, path});
            }
        }
        if(closedir(directory) == -1)
        {
            std::cout << "CHECK closing `for directory: " << directory << std::endl;
            return errno;
        }
    }
    outTree = std::move(createdTree); 
    
    return 0; //return status OK
}

int UnixFileSystem::MakeFile(const std::string& path, EFileType fType)
{
    int ret = 0;
    std::cout<< "PATH : " << path << std::endl;
    if(fType == EFileType::File)
    {
        ret = open(path.c_str(), O_RDWR | O_EXCL | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        if(ret < 0)
        {
            std::cout << "Creating file failed with error: " << errno << std::endl;
            return errno;
        }
        ret = close(ret);
        if(ret < 0)
        {
            std::cout << "Closing file failed with error: " << errno << std::endl;
            return errno;
        }
    } else if(fType == EFileType::Directory)
    {
        ret = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(ret < 0)
        {
            std::cout << "Creating failed with error: " << errno << std::endl;
            return errno;
        }
    } 
    else
    {
        std::cout << "FileType not implemented: " << FileTypeToString(fType) << std::endl;
        return -1; 
    }

    return 0;
}

int UnixFileSystem::CloneDirectory(const std::string& srcPath, const std::string& dstPath)
{
    struct stat st;
    if(stat(srcPath.c_str(), &st) < 0)
    {
        std::cout << "Cannot stat sourcefile, error: " << errno << std::endl;
        return errno;
    }
    if(mkdir(dstPath.c_str(), st.st_mode) < 0)
    {
        std::cout << "Cannot clone directory, error: " << errno << std::endl;
        return errno;
    }
    return 0;
}

int UnixFileSystem::Move(const std::string& srcPath, const std::string& dstPath)
{
    // for files on the same FS we can safely use rename Unix api call
    int ret = 0;
    std::cout << "Move from: " << srcPath << " to: " <<dstPath << std::endl;
    if(rename(srcPath.c_str(), dstPath.c_str()) < 0)
    {
        std::cout << "Moving failed with error: " << errno << std::endl;
        return errno;
    }
    return ret;
}

int UnixFileSystem::Remove(const std::string& path, EFileType fType)
{
    int ret = 0;
    std::cout<< "PATH : " << path << std::endl;
    if(fType == EFileType::Directory)
    {
        ret = rmdir(path.c_str());
    } else
    {
        ret = unlink(path.c_str());
    }
    if(ret < 0)
    {
        std::cout << "Removing failed with error: " << errno << std::endl;
        return errno;
    }
    return 0;
}


bool UnixFileSystem::RefreshState()
{

    return true;
}
bool UnixFileSystem::ObtainAdditionalData()
{
    return true;
}

EFileType UnixFileSystem::ConvertFileType(unsigned char fType) // TODO: expand it
{
    switch(fType)
    {
        case DT_DIR:
            return EFileType::Directory;
        case DT_REG:
            return EFileType::File;
        case DT_LNK:
            return EFileType::Symlink;
        case DT_UNKNOWN:
            return EFileType::Invalid;
        default:
            return EFileType::Other;
    }
}