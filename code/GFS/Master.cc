#include "Master.h"
#include <sstream>   //nachos
#include "post.h"
#include "numeric"
#include <stdlib.h>
#include "post.h"
using namespace std;

//Constructor for the mAtser server
Master::Master(int numReplicate):replicationNumber(numReplicate), fileID(0){}

//To add a chunk server to the list of chunk servers for the master server
void Master::add(Chunkserver *server){
	chservers.push_back(server);
}

void Master::sendMessage(int receiver, char *data){
int farHost = receiver;
PacketHeader outPktHdr;
MailHeader outMailHdr;

//Get the input from the user what command to execute
cin.getline(data, 300);
outPktHdr.to = farHost;
outMailHdr.to = 100;
outMailHdr.from = 0;
outMailHdr.length = strlen(data)+1;
kernel->postOfficeOut->Send(outPktHdr, outMailHdr, data);
}


//Keep on parsing incoming connections while a command in form of message arrives
vector<string> Master::parse_commands(){


    if (kernel->hostName != 100){
        cout<<"This is not the master server";
    }else {
        char *message = "Connected to the master server";
        //Sends a message to client
        sendMessage(0,message);
        while(TRUE) {
        //100 is the master's host number

        PacketHeader inPktHdr;
        MailHeader inMailHdr;
        char buffer[MaxMailSize];

        //Receive the message from client
        kernel->postOfficeIn->Receive(100, &inPktHdr, &inMailHdr, buffer);
        std::string tmp;
        std::vector<std::string> parameters;
        std::stringstream ss(buffer);
        while (ss >> tmp)
            parameters.push_back(tmp);

        std::cout << "In Master now. The argument passed is :  ";
        for (int i = 0; i < parameters.size(); ++i)
            cout<< parameters[i] << ' ';

        std::cout << endl;

        return parameters;
    }

}}

//To simulate the parsing of commands and executing the corresponding function to the command specified
void Master::operator()(){
	while(TRUE){
		vector<string> parameters = parse_commands();
		vector<int> id;
		int openfile;
		char *buf = NULL;

		//To check if no parameters specified
		if(parameters.empty()){
			cerr<<"Input is blank."<<endl;
			char *message = "Input is blank";
			//Sends a message to client
			sendMessage(0,message);
			continue;
		}
		//Exit if specified
		if(parameters[0]=="quit"){
            char *message = "The master server closed";
            //Sends a message to client
            sendMessage(0,message);
			exit(0);
		}
		//Listing the current files present on the chunk servers.
		else if(parameters[0]=="list"||parameters[0]=="ls"){
			if(parameters.size()!=1){
				cerr<<"Usage: list (lists all the files on the chunkserver)"<<endl;
			}
			else{
				cout<<"-------Files--------"<<endl;
                directory.list(metadata);
			}
			continue;
		}
		//Uploading a file on the chunk servers present
		else if(parameters[0]=="put"){
			if(parameters.size()!=3){
				cerr<<"Usage Put Source_File_Path Destination_File_Path"<<endl;
                char *message = "Usage Put Source_File_Path Destination_File_Path";
                //Sends a message to client
                sendMessage(0,message);
                continue; }
				std::string temp= parameters[1];
                char *file= &temp[0u];

            openfile = OpenForReadWrite(file, TRUE);
			if(openfile<0){
				cerr<<"Open file error:File"<<parameters[1]<<endl;
                char *message = "Open file error";
                //Sends a message to client
                sendMessage(0,message);
				continue;
			}
			else if(!directory.insert(parameters[2], TRUE)){
				cerr<<"Create file error. Maybe the file:"<<" "<<parameters[2]<<" "<<"already exists"<<endl;
                char *message = "Create file error. Maybe the file already exists";
                //Sends a message to client
                sendMessage(0,message);
				continue;
			}
			else{
				int totalsize = Tell(openfile);
				buf = new char[totalsize];
				Lseek(openfile, 0, 0);
				ReadPartial(openfile, buf, totalsize);
				vector<double> serverSize;
				for ( Chunkserver *server : chservers) {
                    serverSize.push_back(server->getsize());
                }
                ++fileID;
				for(int i=0; i < replicationNumber; i++){

                    metadata[parameters[2]]= make_pair(fileID, totalsize);
                   chservers[id[i]]->command = "put";
                   chservers[id[i]]->fid = fileID;
                   chservers[id[i]]->bufsize = totalsize;
                   chservers[id[i]]->buf = buf;
                   chservers[id[i]]->finish = FALSE;
				}
			}
		}
		/*Checks if the file specified is present in the ChunkServer.
        * If present, reads the file into the buffer and returns to the user.*/
		else if(parameters[0]=="read"||parameters[0]=="fetch"){
			if(parameters.size()!=3&&parameters.size()!=4){
				cerr<<"Usage: read Source_File_Path Destination_File_Path"<<endl;
				cerr<<"Usage: fetch FileId Offset Destination_File_Path"<<endl;
				continue;
			}
			else{
				if(parameters[0]=="read"&&metadata.find(parameters[1])==metadata.end()){
					cerr<<"Error : No such file exists"<<endl;
					continue;
				}
				for(int i =0; i<4;i++){
					chservers[i]->command=parameters[0];
					if(parameters[0]=="read"){
						pair<int, int> metadata1 = metadata[parameters[1]];
						chservers[i]->fid = metadata1.first;
						chservers[i]->bufsize = metadata1.second;
					}
					else{
						chservers[i]->fid = atoi(parameters[1].c_str());
						chservers[i]->offset = atoi(parameters[2].c_str());
					}
					chservers[i]->finish = FALSE;

				}
			}
		}
		//Looks up the location of the file specified in the chunk servers.
        else if(parameters[0]=="locate"){
			if(parameters.size()!=3){
				cerr<<"Usage: locate FileId Offset"<<endl;
				continue;
			}
			else{
				for(int i=0; i<4;i++){
					chservers[i]->command = "locate";
					chservers[i]->fid = atoi(parameters[1].c_str());
					chservers[i]->offset = atoi(parameters[2].c_str());
					chservers[i]->finish = FALSE;
				}
			}
		}
		else{
			cerr<<"Wrong command"<<endl;
            char *message = "Wrong command";
            //Sends a message to client
            sendMessage(0,message);

		}

		if(parameters[0]=="read"||parameters[0]=="fetch"){
			for(int i =0; i<4; i++){
				if(chservers[i]->bufsize){
					int openfiletowrite;
					if(parameters[0]=="read"){
                        std::string temp= parameters[2];
                        char *file= &temp[0u];
						openfiletowrite = OpenForReadWrite(file, TRUE);
					}
					else if(parameters[0]=="fetch"){
                        std::string temp= parameters[3];
                        char *file= &temp[0u];
						openfiletowrite = OpenForReadWrite(file, TRUE);
					}
					if(openfiletowrite<0){
						cerr<<"Create file failed. Wrong directory."<<endl;
					}
					else{
						WriteFile(openfiletowrite, chservers[i]->buf, chservers[i]->bufsize);
						Close(openfiletowrite);
					}
					delete []chservers[i]->buf;
				}
			}
		}
		else if(parameters[0]=="put"){
			cout<<"Upload successful with FileId:"<<fileID<<endl;
            char *message = "Upload successful";
            //Sends a message to client
            sendMessage(0,message);
		}
		else if(parameters[0]=="locate" || parameters[0]=="ls"){
			bool notfound = TRUE;
			for(int i=0; i<4;i++){
              if(chservers[i]->bufsize){
              	notfound = FALSE;
              	cout<<"Found FileId"<<parameters[1]<<"Offset"<<parameters[2]<<"at"<<chservers[i]->get_name()<<endl;
              }
			}
			if(notfound){
				cout<<"Not Found FileId"<<parameters[1]<<"Offset"<<parameters[2]<<endl;
			}
		}
		delete []buf;
		Close(openfile);
	}
}

