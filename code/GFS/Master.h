#ifndef MASTER_H
#define MASTER_H
#include <string>
#include "../lib/sysdep.h"
#include <map>
#include <vector>
#include "Chunkserver.h"
#include "Directory.h"
using namespace std;

class Master{
public:
	//Constructor
    explicit Master(int numReplicate);
	//Destructor
    ~Master();
	map<string, pair<int, int> > metadata;  //To store the information related to the file
	void add(Chunkserver *server);      //For adding the chunk server to the master server
	void operator()();
    vector<string> parse_commands();    ///To keep on parsing commands entered by the user
    void sendMessage(int receiver, char *data);

private:
    vector<Chunkserver *> chservers;    //To store the chunkservers present
    int replicationNumber;      //To define the replication factor.
    int fileID;  //To identify the file chunk created
    Directory directory; //To create an instance of a directory

};

 #endif //_MASTER_H