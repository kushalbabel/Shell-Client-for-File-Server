#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
using namespace std;


// This function will set up the socket and binding
int make_server_ready(char *port)
{
    int server_port = atoi(port);

    // create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        // display appropriate error message
        cout << "Could not create socket. Run again." << endl;
        exit(1);
    }

    // set server_socket to allow multiple connections.
    int opt = 1;
    if((setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(int)) == -1 )||
        (setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, (char*) &opt, sizeof(int)) == -1 ))
    {
        // display appropriate error message
        cout << "Could not allow multiple connections at the moment. Run again." << endl;
        exit(1);
    }

    // prepare the sockaddr_in structure
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(server_port);
     
    // bind the server_socket
    if(bind(server_socket, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        // display appropriate error message
        cout << "Bind failed. Try different port." << endl;
        exit(1);
    }
     
    // start listening to the socket
    if(listen(server_socket, SOMAXCONN) < 0){
        // display appropriate error message
        cout << "Unable to listen. Try again." << endl;
        exit(1);
    };

    return server_socket;
}

// This function will serve the client. Receive its message and serve the file
void serve_client(int client_socket)
{
    char client_buffer[1024];
    char send_buffer[1024];

    if(recv(client_socket, client_buffer, 1024, 0) <= 0)
    {
        // display appropriate error message
        cout << "Receive of message from client failed. Try again." << endl;
        exit(1);
    }
   
    string file_name = string(client_buffer).substr(4);
    ifstream infile(file_name.c_str());

    while(!infile.eof())
    {
        // read from file
        infile.read(send_buffer, 1024);
        if(send(client_socket, send_buffer, infile.gcount(), 0) < 0)
        {
            // display appropriate error message
            cout << "Sending message to user failed. Try again" << endl;
            exit(1);
        }
        usleep(1000); // sleep for 1 sec (slow server)
    }

    infile.close();
}

// This will be used in reaper thread. Its job is to reap the zombie childs periodically
void *reaper(void* args)
{
   while(1){
        waitpid(-1, 0, WNOHANG);
        usleep(500);
   }
}

// main function
int main(int argc, char *argv[])
{
    // Check if the executable is run with valid number of arguments
    if(argc != 2)
    {
        // display appropriate error message
        cout << "Less/more arguments provided. 1 argument(port number) is expected." << endl;
        exit(1);
    }
    
    int server_socket = make_server_ready(argv[1]);
    bool is_reaper_active = false;
    pthread_t reaper_thread;

    while(1)
    {
        // there is a new incoming connection
        struct sockaddr_in new_address;
        int client_socket = accept(server_socket, (struct sockaddr *) &new_address, (socklen_t*) &new_address);
        if(client_socket < 0)
        {
            // display appropriate error message
            cout << "Error in acceptance. Try again." << endl;
            exit(1);
        }

        pid_t pid = fork();

        if(pid == 0){
            // child process
            serve_client(client_socket);
            close(client_socket);
            exit(0);
        }
        else if(pid > 0){
            // parent process
            close(client_socket);
            if(!is_reaper_active){
                pthread_create(&reaper_thread, NULL, reaper, NULL);
                is_reaper_active = true;
            }
        }   
    }
}