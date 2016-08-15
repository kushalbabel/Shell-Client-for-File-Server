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
				continue;
			case 0 :
				commandError();
				continue;
			case 1:
				cdCmd(tokens);
				break;
			case 2:
				serverCmd(tokens);
				break;
			default :
				commandError();
		}
	}
	
	return 0;
}