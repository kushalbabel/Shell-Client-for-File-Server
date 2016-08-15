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
#include <fcntl.h>    /* For O_WRONLY */
#include <sys/types.h> //for waitpid
#include <sys/wait.h> //for waitpid
using namespace std;

int bytesReceived;
string server_ip = "127.0.0.1";
string server_port  = "5000";
// SIGINT handler funciton
void sigint_handler(int signo)
{
	cout << "\n\n";
	cout << "Received SIGINT; downloaded " << bytesReceived  << " bytes so far." << endl;
	return;
}
char* strToChar(string path){
	char * cpath = new char [path.length()+1];
  	strcpy (cpath, path.c_str());
	return cpath;
}
vector<string> tokenize(string command){
	//append space to get the last token
	command+=" ";
	//initialize vector which holds final tokens
	vector<string> tokens;
	char readChar;
	int i, tokenIndex = 0;
	string currStr = "";
	//loop until all chars are read
	for(i =0; i < command.size(); i++){
		readChar = command[i];
		//space or newline
		if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			if (tokenIndex != 0){
				//one token finished
				tokens.push_back(currStr);
				tokenIndex = 0;
				currStr = "";
			}
		} 
		else {
			//continue reading token
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
//assigns integer value to each commmand and also checks for validity of command
int classifyCmd(vector<string> tokens){
	//-1 if emty commmand, 0 if invalid command
	if (tokens.size()==0) return -1;
	//1 if cd
	if(tokens[0] == "cd"){
		if(tokens.size() != 2){
			return 0;
		}
		else return 1;
	}
	//2 if server
	if(tokens[0] == "server"){
		if(tokens.size()!= 3){
			return 0;
		}
		else return 2;
	}
	//3 if ls
	if(tokens[0] == "ls" || tokens[0] == "cat"){
		return 3;
	}
	//4 if simple download
	if(tokens[0] == "getfl"){
		if(tokens.size()<=1) commandError();
		else{
			if(tokens.size()==2){
				//simple download
				return 4;
			}
			else{
				if(tokens.size()==4 && tokens[2] == ">"){
					//redirected download
					return 5;
				}
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
void waitForDeath(pid_t pid){
	//wait for pid to finish
	int status;
	//wait for the exact child process
	pid_t terminated = waitpid(pid,&status,0);
	if(terminated!=pid){
		cout<<"Oops, some other process terminated!"<<endl;
	}
	else {
		return;
	}
	return;
}
void simpleCmd(vector<string> tokens){
	pid_t pid = fork();
	if(pid < 0){
		cout<<"Error forking"<<endl;
		return;
	}
	if(pid == 0){
		//child process
		string path = "/bin/";
		path = path+tokens[0];
		//create char* for arguement passing
		char * cpath = new char [path.length()+1];
  		strcpy (cpath, path.c_str());
  		//create an array of pointers for arguement passing
		char* argArray[tokens.size()+1];
		//populate arguemtn array
		argArray[0] = cpath;
		argArray[tokens.size()] = NULL;
		for (int i=1;i<tokens.size();i++){
			argArray[i] = strToChar(tokens[i]);
		}
		//call the executable
		execvp(path.c_str(),argArray);
		//one more exit for safety
		exit(0);
	}
	else{
		//parent process
		waitForDeath(pid);
	}
	return;
}
void download(string filename){
	string path = "./get-one-file-sig";
	//create char* for arguement passing
	char * cpath = new char [path.length()+1];
	strcpy (cpath, path.c_str());
	//create an array of pointers for arguement passing
	char* argArray[6];
	//populate arguemtn array
	argArray[0] = cpath;
	argArray[1] = strToChar(filename);
	argArray[2] = strToChar(server_ip);
	argArray[3] = strToChar(server_port);
	argArray[4] = strToChar("display");
	argArray[5] = NULL;
	execvp(path.c_str(),argArray);
	//one more exit for safety
	exit(0);
	return;
}
void simpleDwnld(vector<string> tokens){
	if (server_ip == ""|| server_port == ""){
		cout<<"First provide server details"<<endl;
		return;
	}
	pid_t pid = fork();
	if(pid < 0){
		cout<<"Error forking"<<endl;
		return;
	}
	if(pid == 0){
		//child process
		download(tokens[1]);
		
	}
	else{
		//parent process
		waitForDeath(pid);
	}
	return;
}
void redirectedDwnld(vector<string> tokens){
	if (server_ip == ""|| server_port == ""){
		cout<<"First provide server details"<<endl;
		return;
	}
	string output = tokens[3];
	pid_t pid = fork();
	if(pid < 0){
		cout<<"Error forking"<<endl;
		return;
	}
	if(pid == 0){
		//child process
		int fd1 ;
		if ((fd1 = open(output.c_str(), O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0644)) < 0){
			cout<<"could not open file "<<output<<endl;
			exit(0);
		}
		else{

			if (dup2(fd1,1) < 0){
				cout<<"cant dup"<<endl;
				exit(0);
			}
			close(fd1);
			download(tokens[1]);
			exit(0);
		}
		
	}
	else{
		//parent process
		waitForDeath(pid);
	}
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
				simpleCmd(tokens);
				break;
			case 4:
				simpleDwnld(tokens);
				break;
			case 5:
				redirectedDwnld(tokens);
				break;
			default:
				commandError();
		}
	}
	
	return 0;
}