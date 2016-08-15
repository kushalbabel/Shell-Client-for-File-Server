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

	// SIGINT handler funciton
void sigint_handler(int signo)
{
	cout << "\n\n";
	cout << "Received SIGINT; downloaded " << bytesReceived  << " bytes so far." << endl;
	exit(0);
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
	// main function
int main(int argc , char *argv[])
{	
	string command;
	vector<string> tokens;
	// register the sigint signal handler
	if(signal(SIGINT, sigint_handler) == SIG_ERR)
		cout << "Can't catch SIGINT" << endl;
	while(true){
		//prompt
		cout<<"Hello> ";
		//get command along with spaces
		getline(cin,command);
		//tokenize
		tokens = tokenize(command);
		printTokens(tokens);
	}
	
	return 0;
}