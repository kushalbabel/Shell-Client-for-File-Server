#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
using namespace std;

// main function
int main(int argc , char *argv[])
{
    // Check if the executable is run with valid number of arguments
    if(argc != 5)
    {
        // display appropriate error message
        cout << "Less/more arguments provided. Total 4 arguments are expected." << endl;
        return -1;
    }
    
    // extract information from arguments provided
    const string file_name = string(argv[1]);
    const char* server_ip = argv[2];
    const int server_port = atoi(argv[3]);
    const bool isDisplayON = (string(argv[4]) == "display") ? 1 : 0;

    // create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        // display appropriate error message
        cout << "Could not create socket. Run again." << endl;
        exit(1);
    }

    // prepare the sockaddr_in structure
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_port = htons(server_port);

    // connect to server
    if(connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        // display appropriate error message
        cout << "Connection with server failed" << endl;
    }

    // prepare the file request message
    char send_buffer[1025];
    bzero(send_buffer, 1025);
    string req = "get " + file_name;
    strcpy(send_buffer, req.c_str());

    // send the message to the server
    if(send(sock, send_buffer, strlen(send_buffer), 0) < 0)
    {
        // display appropriate error message
        cout << "Error sending message to the server" << endl;
    }
    
    // receive the file from the server
    char recv_buffer[1025];
    while(1)
    {
        bzero(recv_buffer,1025);
        int n = recv(sock, recv_buffer, 1024, 0);    // receive the message from server
        // display the message if client is in display mode
        if(isDisplayON)
            cout << recv_buffer;
        if (n <= 0) break;
    }

    // close the socket
    close(sock);
}