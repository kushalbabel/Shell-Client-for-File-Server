#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h> //for waitpid
#include <sys/wait.h> //for waitpid
using namespace std;

int bytesReceived;
string server_ip = "";
string server_port  = "";
	// SIGINT handler funciton
void sigint_handler(int signo)
{
	cout << "\n\n";
	cout << "Received SIGINT; downloaded " << bytesReceived  << " bytes so far." << endl;
	return;
}
vector<string> tokenize(string command){
	command+=" ";
	vector<string> tokens;
	char readChar;
	int i, tokenIndex = 0;
	string currStr = "";

	for(i =0; i < command.size(); i++){
		readChar = command[i];
		if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			if (tokenIndex != 0){
				tokens.push_back(currStr);
				tokenIndex = 0;
				currStr = "";
			}
		} 
		else {
			currStr+=readChar;
			tokenIndex++;
		}
	}
	return tokens;
}
void printTokens(vector<string> tokens){
	cout<<"The tokens are :"<<endl;
	for (int i=0;i<tokens.size();i++){
		cout<<tokens[i]<<" ";
	}
	cout<<endl;
}
void commandError(){
	cout<<"Wrong command format"<<endl;
	return;
}
int classifyCmd(vector<string> tokens){
	if (tokens.size()==0) return -1;
	if(tokens[0] == "cd"){
		if(tokens.size() != 2){
			return 0;
		}
		else return 1;
	}
	if(tokens[0] == "server"){
		if(tokens.size()!= 3){
			return 0;
		}
		else return 2;
	}
	if(tokens[0] == "ls"){
		if(tokens.size()!= 1){
			return 0;
		}
		else return 3;
	}
	if(tokens[0] == "getf1"){
		if(tokens.size()<=1) commandError();
		else{
			if(tokens.size()==2){
				return 4;
			}
		}
	}
	return 0;
}
void cdCmd(vector<string> tokens){
	//implement cd
	int status = chdir((tokens[1]).c_str());
	if (status == 0){
		//successful
		cout<< "Directory changed to "<<tokens[1]<<endl;
	}
	else{
		//error
		if (status == -1){
			cout<<"No such directory"<<endl;
		}
		else{
			cout<<"error "<<status << " enccountered"<<endl;	
		}		
	}
	return;
}
void serverCmd(vector <string> tokens){
	server_ip = tokens[1];
	server_port = tokens[2];
	return;
}
void lsCmd(){
	pid_t pid = fork();
	pid_t terminated;
	if(pid < 0){
		cout<<"Error forking"<<endl;
		return;
	}
	if(pid == 0){
		//child process
		string path = "/bin/ls";
		//create char* for arguement passing
		char * cpath = new char [path.length()+1];
  		strcpy (cpath, path.c_str());
  		//create an array of pointers for arguement passing
		char* argArray[2];
		//populate arguemtn array
		argArray[0] = cpath;
		argArray[1] = NULL;
		//call ls
		execvp(path.c_str(),argArray);
		//one more exit for safety
		exit(0);
	}
	else{
		//parent process
		//wait for ls to finish
		int status;
		//wait for the exact child process
		terminated = waitpid(pid,&status,0);
		if(terminated!=pid){
			cout<<"Oops, some other process terminated!"<<endl;
		}
		else {
			return;
		}
	}
}
void simpleDwnld(vector<string> tokens){
	// pid_t pid = fork();
	// if (pid == )
	return;
}
// main function
int main(int argc , char *argv[])
{	
	string command;
	vector<string> tokens;
	// register the sigint signal handler
	// if(signal(SIGINT, sigint_handler) == SIG_ERR)
	// 	cout << "Can't catch SIGINT" << endl;
	while(true){
		//prompt
		cout<<"Hello> ";
		//get command along with spaces
		getline(cin,command);
		//tokenize
		tokens = tokenize(command);
		// printTokens(tokens);
		int type = classifyCmd(tokens);
		switch(type){
			case -1:
				//no command
				continue;
			case 0:
				//erroneous command
				commandError();
				continue;
			case 1:
				//cd
				cdCmd(tokens);
				break;
			case 2:
				//server
				serverCmd(tokens);
				break;
			case 3:
				//ls
				lsCmd();
			case 4:
				simpleDwnld(tokens);
				break;
			default:
				commandError();
		}
	}
	
	return 0;
}