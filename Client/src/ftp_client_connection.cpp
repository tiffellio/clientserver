
 /*
File: ftp_client_connection.cpp
Project: CSCI460 Project 1
Author: Tiffany Elliott & Qijie (Ben) Lao
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> /*socket definitions*/
#include <sys/socket.h>
#include <netinet/in.h> /*store address information*/
#include <unistd.h>
#include <arpa/inet.h> 
#include <iostream>
using namespace std;
#include "../include/ftp_client_connection.hpp"

// Sends a connection request on severPort of a sever whose IP address is equal to serverIP.
// If a connection has been established as a result of the request sets the connection descriptor value 
// to reference 'socketDescriptor' and sets reference 'isConnected' to true.
void connectToServer(int& socketDescriptor, bool& isConnected, const char* serverIP, int serverPort){

    struct sockaddr_in server; //declare structure for the address
    server.sin_family = AF_INET; //sets the family/type of address
    server.sin_port = htons(serverPort); //specify the port were going to connect to
	inet_pton(AF_INET, serverIP, &server.sin_addr);

   //call the connect function to connect to the remote address
    int connection_status = connect(socketDescriptor, (struct sockaddr *) &server, sizeof(server)); //casts to the right strcutre type

    //check everything is ok with the connection
    if(connection_status == -1){
		isConnected = 0;
    } else {
		isConnected = 1;
	}
}

// Closes network connection represented by reference 'socketDescriptor' and
// sets reference 'isConnected' to false.
void disconnectFromServer(int& socketDescriptor, bool& isConnected) {
    if (isConnected) {
        //close the socket once done
        close(socketDescriptor);
	    isConnected = 0;
    } else {
        exit(0);
    }
}

// Sends 'message' of length 'messageLength' bytes to the server 
// on the network connection represented by 'sockDescriptor'.
int sendToRemote(int sockDescriptor, const char* message, int messageLength){
    return send(sockDescriptor, message, messageLength, 0); 
}

// Receives 'message' of length 'messageLength' bytes from the server 
// on the network connection represented by 'sockDescriptor'.
int receiveFromRemote(int sockDescriptor, char* message, int messageLength){

    ///we must be able to send AND receive data
    return recv(sockDescriptor, message, messageLength, 0); 
}


