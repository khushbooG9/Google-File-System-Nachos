#ifndef _CHUNKSERVER_H_
#define _CHUNKSERVER_H_
#include <string>
using namespace std;

extern int chunkSize;   //To specify the chunk size

class Chunkserver{
private:
    string name;    //To identify the chunk server
    double size;    //To determine the number of chunks for the file
    void put();     //To upload a file
    void read();    //To read an existing file from the chunk server
    void locate();  //To look up a file in the chunk server
    void fetch();   //If file present, retrieves the file from chunk server

public:
    string command; //To parse command got from the client
    int fid, bufsize, offset; // File id, Buffer size, offset required for positioning in the file
    char* buf;     //Buffer for reading/writing a file
    bool finish;    //To check if the command execution have completed

    //Constructor
    Chunkserver(const string &name);

    void operator()();
    double getsize() {return size;}     //To get the number of chunks for the file
    string get_name()const;
};

#endif