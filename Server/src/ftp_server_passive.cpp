/*
#define PASSIVE_DEFAULT_PORT "0"
#define DATA_CONNECTION_TIME_OUT_SEC 20
#define DATA_CONNECTION_TIME_OUT_USEC 0
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "ftp_server_connection_listener.hpp"
#include "ftp_server_connection.hpp"
#include "ftp_server_net_util.hpp"
#include "ftp_server_command.hpp"
#include "ftp_server_string_util.hpp"
#include "ftp_server_response.hpp"
#include "ftp_server_passive.hpp"
using namespace std;

void enteringIntoPassive(char* argument, const int controlSockDescriptor, int& passiveListenerSockDescriptor, int& dataSockDescriptor){
	//Starts a passive connection listener by calling 'startPassiveListener()' function.
	bool success = false;
    startPassiveListener(passiveListenerSockDescriptor, success);

	//Once successful on the above function call, sends an appropriate response to the client.
    if (!success) {
    	sendToRemote(controlSockDescriptor, PASSIVE_ERROR_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
    } else {
		//Waits for a specific time interval for the client to send a connection request on the listener by calling
		//isListenerSocketReady() from 'ftp_server_connection_listener.hpp'.
	    bool error = false;
	    bool timeout = false;
	    if (isListenerSocketReady(passiveListenerSockDescriptor, DATA_CONNECTION_TIME_OUT_SEC, DATA_CONNECTION_TIME_OUT_USEC, error, timeout)) {
			//Accepts client's connection request, if there is any, and opens a data connection with the client by calling
			//'acceptClientConnetion() function from 'ftp_server_connection_listener.hpp'.
	        acceptClientConnection(controlSockDescriptor, passiveListenerSockDescriptor);//Closes the connection listener after opening the data connection with the client by calling 'closeListenerSocket()'
			//function from 'ftp_server_connection_listener.hpp'.
	    	closeListenerSocket(passiveListenerSockDescriptor);
	    } else {
			//Sends appropriate response to the client using control connection.
			//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send response to client.
			if (timeout) {
		        sendToRemote(passiveListenerSockDescriptor, PASSIVE_ERROR_TIMEOUT_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
		    } else if (error) {
		        sendToRemote(passiveListenerSockDescriptor, PASSIVE_ERROR_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
		    } else {
		        char response[FTP_RESPONSE_MAX_LENGTH];
		        createPassiveSuccessResponse(response, passiveListenerSockDescriptor);
		        sendToRemote(passiveListenerSockDescriptor, response, FTP_RESPONSE_MAX_LENGTH);
		    }
		}
	}

}

void startPassiveListener(int& listenerSockDescriptor, bool& succeded) {
	//Starts a passive listener socket that can listen connection requests from the remote computer.
	//by calling 'startListenerSocket()' function from 'ftp_server_connection_listener.hpp'.
	startListenerSocket(PASSIVE_DEFAULT_PORT, listenerSockDescriptor, succeded);
}

void createPassiveSuccessResponse(char* response, const int passiveListenerSockDescriptor) {
	//Creates a Passive success response.
	//Determines the passive listener port number from 'passiveListenerSockDescriptor' by calling
	//'getPortFromSocketDescriptor()' function from 'ftp_server_net_util.hpp'.
    int passiveListenerPort = getPortFromSocketDescriptor(passiveListenerSockDescriptor);
 	int p1 = passiveListenerPort >> 8;
	int p2 = passiveListenerPort - (p1 << 8);
	//Determines the local IP address by calling 'getHostIPAddress()' function from 'ftp_server_net_util.hpp'.
	char* localAddress = getHostIPAddress();
	replaceCharWith(localAddress, '.', ',');
	printf("FTP server with IP address: %s is waiting at port: %d for connection.........\n", localAddress, passiveListenerPort);
	//Includes both the IP address and the port number into passive success response according to RFC.
	sprintf(response, PASSIVE_SUCCESS_RESPONSE, localAddress, p1, p2);
}
