#include <stdlib.h> 
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <stdio.h>
#include <string.h>
#include "../include/ftp_server_connection_listener.hpp"

#include "ftp_server_connection_listener.hpp"
#include "ftp_server_net_util.hpp"

int streamSocket = -1;

void startListenerSocket(char* port, int& listenerSockDescriptor, bool& succeded){
	//Starts a listener socket that can listen connection requests from the remote computer.
	struct sockaddr_in address;
	int set = 1;

	if((listenerSockDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Socket creation failed\n");
		exit(0);
	}

	if(setsockopt(listenerSockDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &set, sizeof(int)) < 0){
		printf("setsockopt\n");
		exit(0);
	} 
	//Creates a stream socket and binds it to a local IP address and to the given port number.
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(port));
	address.sin_addr.s_addr = INADDR_ANY;

	//Once the binding is successful listens for connection request from the remote computers.
	if(bind(listenerSockDescriptor, (sockaddr *)&address, sizeof(address)) != 0){
		printf("socket binding failure\n");
		exit(0);
	}

	if(listen(listenerSockDescriptor, MAX_PENDING_CONNECTIONS) != 0){
		printf("listening failed on server\n");
		exit(0);
	}

	succeded = true;
}

bool isListenerSocketReady(int sockDescriptor, int timeoutSec, int timeoutUSec, bool& isError, bool&isTimedout){
	//Returns true if there is any remote connection request on the listener socket represented by 'sockDescriptor'.
	//Waits for a connection request until 'timeoutSec' + 'timeoutUsec' time.
	//If no connection request has been received before the wait time times out, sets 'isTimedout' value to 'true'.
	//If any error occurs, sets 'isError' value to 'true'.
	//Calls 'isSocketReadyToRead()' function from 'ftp_server_net_util.hpp' to do all of the above.
	return isSocketReadyToRead(sockDescriptor, timeoutSec, timeoutUSec, isError, isTimedout);
}

void acceptClientConnection(int listenerSockDescriptor, int& clientSockDescriptor){
	//Accepts a connection request on the listener socket represented by 'listenerSockDescriptor'.
	//Opens the connection to the remote computer and represents the connection by 'clientSockDescriptor'.
	struct sockaddr_in newAddress;
	socklen_t length = sizeof(newAddress);
	if((clientSockDescriptor = accept(listenerSockDescriptor, (sockaddr*)&newAddress, &length)) < 0){
		printf("Accepting failed by the server\n");
		exit(0);
	}
}

void closeListenerSocket(int& listenerSockDescriptor){
	//Closes the listener socket represented by 'listenerSockDescriptor'.
	closeSocket(listenerSockDescriptor);
}

