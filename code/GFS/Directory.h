#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include <string>
#include <map>
using namespace std;


struct DirNode{
    string value;      //
    bool isFilePresent;  //Checks whther file is present or not.
    DirNode *parent;    //To store the parent directory
    DirNode *child ;    //To store the child of the node
    DirNode *sibling;   //To store other file present on the same level

    //Constructor
    DirNode(const std::string &_value, const bool &_isFile): value(_value), isFilePresent(_isFile){};
};

class Directory{
private:
    DirNode *root;  //To store root for the tree.
public:
    Directory();    //Constructor

    //Inserts a file and returns true if successful
    bool insert(const string &path, const bool isFile);

    //Search a file and returns true if successful.
    bool search(const string &path, DirNode **parent)const;

    //Lists the files in current directories
    void list(DirNode *node, map<string, pair<int, int> > & meta);
    void list(map<string, pair<int, int> > & meta);
};

#endif