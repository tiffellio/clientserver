/* Project 2
Name: Tiffany Elliott & Ben Lao
Date: Nov 2019
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ftp_server_response.hpp"
#include "ftp_server_command.hpp"
#include "ftp_server_session.hpp"
#include "ftp_server_connection.hpp"

void startClientFTPSession(int& controlSockDescriptor){
//Starts a client FTP session against a client control connection represented by 'controlSockDescriptor'.
//Keeps track of the states of the session using various local variables including 'controlSockDescriptor'.
//Other local variables that a session needs to maintain are as follows:
      //isClientConnected = true
      //isUser = false
      //isLoggedIn = false
      //dataSocketDescriptor = -1
      //dataListenerSockDescriptor = -1
      //rootDir
	bool isClientConnected = true;
    bool isUser = false;
    bool isLoggedIn = false;
    int dataSocketDescriptor = -1;
    int dataListenerSockDescriptor = -1;
	char rootDir[128];
	char buffer[256];
	//Determines the root directory of the FTP server and sets it to the local variable 'rootDir'
	//Passes the other references of these local variables in various function calls, the called functions then sets
	//the appropriate value to these variables using the references.
	//Waits for client's FTP request for FTP_CLIENT_SESSION_TIMEOUT_SEC + 0.000001xFTP_CLIENT_SESSION_TIMEOUT_USEC time
	//calling 'isConnectionReadyToRead()' function from 'ftp_server_connection.hpp'.
	bool timeout = false;
	bool error = false;
	bool success = isConnectionReadyToRead(controlSockDescriptor, FTP_CLIENT_SESSION_TIMEOUT_SEC, FTP_CLIENT_SESSION_TIMEOUT_USEC, error, timeout);
	while (isClientConnected){
		//If a request comes before the timeout, reads the request.
		if(success){
			memset(command, '\0', FTP_RESPONSE_MAX_LENGTH);
			receiveFromRemote(controlSockDescriptor, command, FTP_RESPONSE_MAX_LENGTH);
			printf("Command received: %s\n", command);
			interpreteCommand(command, controlSockDescriptor, dataListenerSockDescriptor, isClientConnected, isUser, isLoggedIn, const_cast<char*>(rootDir));
		} else {
			//If timeout or error happens before a FTP request comes, sends an appropriate response to the client
			if(isTimedOut) {
				sendToRemote(controlSockDescriptor, PASSIVE_ERROR_TIMEOUT_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
			} else if(isError){
				sendToRemote(controlSockDescriptor, INTERNAL_ERROR_RESPONSE, FTP_RESPONSE_MAX_LENGTH);	//right msg?			
			}
			//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send response to the client.
			sendToRemote(controlSockDescriptor, CONNECTION_RESET_BY_PEER, FTP_RESPONSE_MAX_LENGTH);
			closeAllConnections(controlSockDescriptor, dataListenerSockDescriptor, dataSocketDescriptor, isClientConnected);
		}
	}
}


/*
	while (isClientConnected) {
		//If a request comes before the timeout, reads the request.
		if (success) {
			//Interprets the request, takes appropriate action, and sends appropriate response to the client by calling
			//'interpreteCommand()' function from 'ftp_server.command.hpp'.
			bzero(buffer, sizeof(buffer));
			receiveFromRemote(controlSockDescriptor, buffer, sizeof(buffer));
			interpreteCommand(buffer, controlSockDescriptor, dataListenerSockDescriptor, dataSocketDescriptor, isClientConnected, isUser, isLoggedIn, rootDir);
		} else {
			//If timeout or error happens before a FTP request comes, sends an appropriate response to the client
			if (error) {
				printf("Error: something happened before FTP request.\n");
				closeAllConnections(controlSockDescriptor, dataListenerSockDescriptor, dataSocketDescriptor, isClientConnected);
				exit(0);
			} else if (timeout) {
				//and closes all connections calling 'closeAllConnections()' function from 'ftp_server_connection.hpp'.
				printf("Session expired: client timed out.\n");
				closeAllConnections(controlSockDescriptor, dataListenerSockDescriptor, dataSocketDescriptor, isClientConnected);
				exit(0);
			}
		}

    }
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send response to the client.
	getcwd(rootDir, sizeof(rootDir));
	sendToRemote(controlSockDescriptor, "Connecton sucessful...\n", 10);
}
*/
