#include "sysdep.h"
#include "Chunkserver.h"
#include <string>
#include <sstream>

//Specify chunksize to be 1 MB
int chunkSize = 1 * 1024 * 1024;

Chunkserver::Chunkserver(const std::string &name1):name(name1), buf(NULL), finish(true){}

//Checks what command has been specified by the user and runs corresponding functions
void Chunkserver::operator()(){
    while(true){
        if (command == "put"){
            size += bufsize / 1024.0 / 1024.0;
            put();
        }
        else if(command == "read")
            read();
        else if(command == "locate")
            locate();
        else if(command == "fetch")
            fetch();
        this->finish = true;
    }}

/*To upload a file.
 * Checks how many chunks are required based on the file size.
 * Creates the chunks.
 * Writes the file to the chunks.*/

void Chunkserver::put(){
    int start = 0;
    stringstream ss, sss;
    ss << fid;
    while(start < bufsize ){
        int offset = start / chunkSize;
        sss << offset;
        string filePath = name+ "/" + ss.str() + " " + sss.str();
        char *file= &filePath[0u];
        int openfile = OpenForWrite(file);
        if(openfile<0)
            cerr << "create file error in chunkserver: (file name) " << filePath << endl;
        WriteFile(openfile, buf, min(chunkSize, bufsize-start));
        start += chunkSize;
        Close(openfile);
    }
}


/*Checks if the file specified is present in the ChunkServer.
 * If present, reads the file into the buffer and returns to the user.*/
void Chunkserver::read(){
    int start = 0;
    stringstream ss, sss;
    ss << fid;
    buf = new char[bufsize];
    while(start < bufsize){
        int offset = start / chunkSize;
        sss << offset;
        string filePath = name + "/" + ss.str() + " " + sss.str();
        char *file= &filePath[0u];
        int openfile = OpenForReadWrite(file, true);
        // file found not in this server.
        if(openfile<0){
            delete []buf;
            bufsize = 0;
            break;
        }
        ReadPartial(openfile, buf, min(chunkSize, bufsize - start));
        start += chunkSize;
        Close(openfile);
    }
}

/*Checks if the file specified is present in the ChunkServer.
 * If present, opens the file content on the user end.*/
void Chunkserver::fetch(){
    int start=0;
    stringstream ss, sss;
    ss << fid;
    sss << offset;
    buf = new char[chunkSize];
    string filePath = name + "/" + ss.str() + " " + sss.str();
    char *file= &filePath[0u];
    int openfile = OpenForReadWrite(file, true);
    // file found not in this server.
    if(openfile<0){
        delete []buf;
        bufsize = 0;
    }
    else{
        ReadPartial(openfile, buf, min(chunkSize, bufsize - start));
        bufsize = Tell(openfile);
    }
    Close(openfile);
}

//Looks up the location of the file specified in the chunk servers.
void Chunkserver::locate(){
    stringstream ss, sss;
    ss << fid;
    sss << offset;
    string filePath = name + "/" + ss.str() + " " + sss.str();
    char *file= &filePath[0u];
    int openfile = OpenForWrite(file);
    if(openfile>=0) {
        bufsize = 1;
    }
    else {
        bufsize = 0;
    }
    Close(openfile);
}

string Chunkserver::get_name()const{
    return name;
}
