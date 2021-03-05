/* Team 2: Tiffany & Ben
Project 2: FTP Server
Date: November, 2019
*/
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <sys/select.h>

#include "ftp_server_net_util.hpp"

void closeSocket(int& sockDescriptor){
	//Closes the stream socket, represented by 'sockDescriptor'.
	close(sockDescriptor);
}

int getPortFromSocketDescriptor(int sockDescriptor) {
	//Determines and returns the associated port number from a given socket descriptor.
	struct sockaddr_in my_addr;
	getsockname (sockDescriptor,(struct sockaddr *) &my_addr,(socklen_t*) sizeof(my_addr)); // 
	return (int) ntohs(my_addr.sin_port); //converts the unsigned short integer hostshort from host byte order to network byte order
}

bool isSocketReadyToRead(int sockDescriptor, int timeoutSec, int timeoutUSec, bool& isError, bool& isTimedout) {
	//Returns true if there is any data sent by the remote computer on the stream socket represented by 'sockDescriptor'.
	//Waits for the data until 'timeoutSec' + 0.000001x'timeoutUsec' time.
	//If no data has been sent before the wait time times out, sets 'isTimedout' value to 'true'.
	//If any error occurs, sets 'isError' value to 'true'.

	fd_set set;
	char buffer[MAX_IP_ADDRESS_LENGTH];
	struct timeval t;

	t.tv_sec = timeoutSec;
	t.tv_usec = timeoutUSec;
	FD_ZERO(&set);
	FD_SET(sockDescriptor, &set);

	int ret = select(sockDescriptor + 1, &set, NULL, NULL, &t);

	if (ret == -1) {
		isError = true;
		printf("There was an error\n");
		return 0;
	} else if (ret == 0) {
		isTimedout = true;
		printf("Timeout\n");
		return 0;
	} else { // ?????
		bzero(buffer, sizeof(buffer));
		read(sockDescriptor, buffer, sizeof(buffer));
        //return true;
	}
	printf("Buffer: %s\n", buffer);
	return 1;
}

char* getHostIPAddress() {
	//Determines and returns an IP address of the local host.
	char* ip;
	char hBuff[MAX_IP_ADDRESS_LENGTH];
	struct hostent* host;

	gethostname(hBuff, MAX_IP_ADDRESS_LENGTH);
	host = gethostbyname(hBuff);
	char hostBuff[MAX_IP_ADDRESS_LENGTH];
	
	gethostname(hostBuff, MAX_IP_ADDRESS_LENGTH);
	host = gethostbyname(hostBuff);
	ip = inet_ntoa(*((struct in_addr*) host -> h_addr_list[0]));
	return ip;
}
