#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <sstream>
#include "Directory.h"
using namespace std;

Directory::Directory(){
    //Sets the tree structure for a node starting from root node
    root = new DirNode("/", false);
    root->sibling = NULL;
    root->parent = NULL;
    root->child = NULL;
}

//Library helper function to split the string according to a character
vector<string> split(const string& str, char del){
    vector<string> tokenCollector;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, del)) {
        tokenCollector.push_back(token);    }
    return tokenCollector;
}

//function definition to insert a given file by its pathname in the directory structure
bool Directory::insert(const string &path, const bool isFile){

    //Sets the parent of s node to null and checks if the file path specified exists in the tree structure. IF not, returns false.
    DirNode *parent = NULL;
    bool isFound = search(path, &parent);
    if(isFound) return false;

    //Splits the file path specified into different folders as in directory
    vector<string> path_folders = split(path, '/');

    //Creates folders in the tree structures as specified
    DirNode *newNode = new DirNode(path, isFile);
    newNode->parent = parent;
    DirNode* son = parent->child;
    if(!son)
        parent->child = newNode;
    else{
        while(son->sibling)
            son = son->sibling;
        son->sibling = newNode;
    }
    while(son) son = son->sibling;
    son = newNode;
    return true;
}

bool Directory::search(const string &path, DirNode **last_node)const{
    //Splits the file path specified into different folders as in directory
    vector<string> path_folders = split(path, '/');

    //Search whether file specified in the filepath is present in the tree structure.
    DirNode *node = root->child;
    *last_node = root;
    for(const string &name: path_folders){
        while(node && node->value != name)
            node = node->sibling;
        if(!node)
            return false;
        *last_node = node;
        node = node->child;
    }
    return true && node->isFilePresent;
}


//Lists the directories from a given node
void Directory::list(DirNode *node, map<string,pair<int, int> >& meta){
    static int chunkSize = 1 * 1024 * 1024;
    if(node){
        cout << node->value<< "\t" << meta[node->value].first << "\t" << (int)ceil(1.0 * meta[node->value].second/chunkSize) <<endl;
        list(node->child, meta);
        list(node->sibling, meta);
    }
}

//Lists the directories from the root node
void Directory::list(map<string, pair<int, int> >& meta){
    list(root, meta);
}
