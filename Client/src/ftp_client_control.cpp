 /*
File: ftp_client_control.cpp
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
#include "../include/ftp_client_connection.hpp"
#include "../include/ftp_client_control.hpp"
#include "../include/ftp_client_ui.hpp"
#include <iostream>
using namespace std;

int controlSocket = -1;
int dataSocket = -1;
bool isControlConnected = false; 
bool isDataConnected = false;

void connectToServerControl(const char* serverIP, int serverPort){
	// Sends a control connection request on severPort of a sever whose IP address is equal to serverIP.
    controlSocket = socket(AF_INET, SOCK_STREAM, 0);
    connectToServer(controlSocket, isControlConnected, serverIP, serverPort);
}

void connectToServerData(const char* serverIP, int serverPort){
	// Sends a data connection request on severPort of a sever whose IP address is equal to serverIP.
	// If the data connection is successful sets its value to dataSocket and 
	// sets isDataConnected to true.
    dataSocket = socket(AF_INET, SOCK_STREAM, 0);
    connectToServer(dataSocket, isDataConnected, serverIP, serverPort);
}

int sendOnControl(const char* buffer, int length) {
	// Sends the info in the buffer of length equal to 'length' on control connection.
	// Returns the actual number of bytes sent
	return sendToRemote(controlSocket, buffer, length);
}

int sendOnData(const char* buffer, int length) {
	// Sends the info in the buffer of length equal to 'length' on data connection.
	// Returns the actual number of bytes sent.
	return sendToRemote(dataSocket, buffer, length);
}

int receiveOnControl(char* buffer, int length) {
	// Receives the info in the buffer of length equal to 'length' on control connection.
	// Returns the actual number of bytes received.
	return receiveFromRemote(controlSocket, buffer, length);
}

int receiveOnData(char* buffer, int length) {
	// Receives the info in the buffer of length equal to 'length' on data connection.
	// Returns the actual number of bytes received.
	return receiveFromRemote(dataSocket, buffer, length);
}

void disconnectControlConnection() {
	// Closes the opened control connection and sets isControlConnected to false;
	disconnectFromServer(controlSocket, isControlConnected);
}

void disconnectDataConnection() {
	// Closes the opened data connection and sets isDataConnected to false;
	if (isDataConnected) {
		close(dataSocket); 
    	isDataConnected = 0;
	} else {
		return;
	}

	disconnectFromServer(dataSocket, isDataConnected);
}

bool isControlConnectionAvailable() {
	// Returns the value of isControlConnected.
	return isControlConnected;
}

bool isDataConnectionAvailable() {
	// Returns the value of isDataConnected.
	return isDataConnected;
}
