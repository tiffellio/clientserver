 /*
File: ftp_client_command.cpp
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
#include "../include/ftp_client_control.hpp"
#include "../include/ftp_client_ui.hpp"
#include "../include/ftp_client_command.hpp"
#include "../include/ftp_server_response.hpp"
#include <iostream>
#include <string.h>
#include <fstream>
using namespace std;

void handleCommandHelp() {
// Displays following help information to the user terminal
// Usage: csci460Ftp>> [ help | user | pass | pwd | dir | cwd | cdup | get | quit ]
//          help                    Gives the list of FTP commands available and how to use them.
//          user    <username>      Sumbits the <username> to FTP server for authentication.
//          pass    <password>      Sumbits the <password> to FTP server for authentication.
//          pwd                     Requests FTP server to print current directory.
//          dir                     Requests FTP server to list the entries in the current directory.
//          cwd     <dirname>       Requests FTP server to change current working directory.
//          cdup                    Requests FTP server to change current directory to parent directory.
//          get     <filename>      Requests FTP server to send the file with <filename>.
//          quit                    Requests to end FTP session and quit.
	cout << "Usage: csci460Ftp>>  [ help | user | pass | pwd | dir | cwd | cdup | get | quit ]\n";
    cout << "          help                    Gives the list of FTP commands available and how to use them.\n";
	cout << "          user    <username>      Sumbits the <username> to FTP server for authentication.\n";
    cout << "          pass    <password>      Sumbits the <password> to FTP server for authentication.\n";
    cout << "          pwd                     Requests FTP server to print current directory.\n";
    cout << "          dir                     Requests FTP server to list the entries in the current directory.\n";
    cout << "          cwd     <dirname>       Requests FTP server to change current working directory.\n";
    cout << "          cdup                    Requests FTP server to change current directory to parent directory.\n";
    cout << "          get     <filename>      Requests FTP server to send the file with <filename>.\n";
    cout << "          quit                    Requests to end FTP session and quit.\n";
}

void handleCommandUser(std::string username) {
    	// Sends a 'USER <username>' request meesage to the FTP Server.
    	// Receives response from FTP Server.
    	// Reacts and informs the user about the response.
    	// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter true 
    	// to perform the activities mentioned above.
        handleSimpleCommandResponse("USER "+username, true);
}

void handleCommandPassword(std::string password) {
	// Sends a 'PASS <password>' request message to the FTP Server.
	// Receives response from FTP Server.
	// Reacts and informs the user about the response. 
	// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter true 
	// to perform the activities mentioned above.
    handleSimpleCommandResponse("PASS "+password, true);
}

void handleCommandDirectory() {
    // First sends a 'PASV' request message to the FTP server.
    // Receives the response against PASV request message from the server.
    // If the response is a successful one, retrives data-connection listener port number form the response.
    // Requests a data connection to the server on the listener port at the server.
    // Calls function handlePassive() to perform the activities mentioned above. 
    handlePassive(PASV_NLST);

    // If the data connection is successful, sends a 'NLST' request message to the server on the control connection.
    // Receives the response against NLST request from the server on the control connection.
    // If the response is successful, retrieves the list of entries in server's current directory on the data connection.
    // Displays the list of entries to the user.
    // Calls function handleNLIST() to perform the activities mentioned above. 
    handleNLIST();
}

void handleCommandPrintDirectory() {
	// Sends a 'PWD' request message to FTP Server.
	// Receives the response from the server.
	// If the response is successful, displays the name of the current working directory of the server.
	// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter false 
	// to perform the activities mentioned above.
    handleSimpleCommandResponse("PWD", false);
}

void handleCommandChangeDirectory(std::string path) {
	// Sends a 'CWD <path>' request message to the FTP Server.
	// Receives the response from the server and displays the response to the user.
	// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter false 
	// to perform the activities mentioned above.
    handleSimpleCommandResponse("CWD "+path, false);
}

void handleCommandChangeDirectoryUp() {
    // Sends a 'CDUP' request message to FTP Server.
    // Receives the response from the server and displays the response to the user.
    // Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter false 
    // to perform the activities mentioned above.
    handleSimpleCommandResponse("CDUP", false);
}

void handleCommandGetFile(std::string filename) {
    // First sends a 'PASV' request message to the FTP server.
    // Receives the response against PASV request message from the server.
    // If the response is a successful one, retrives data-connection listener port number form the response.
    // Requests a data connection to the server on the listener port at the server.
    // Calls function handlePassive() to perform the activities mentioned above.
    handlePassive(PASV_RETR); 

    // If the data connection is successful, sends a 'RETR <filename>' request message 
    // to the server on the control connection.
    // Receives the response against RETR request from the server on the control connection.
    // If the response is successful, retrieves the content of the file on the data connection.
    // Saves the file in the local directory at the client computer.
    // Calls function handleRETR() to perform the activities mentioned above. 
    handleRETR(filename);
}

void handleCommandQuit() {
	// Sends a 'QUIT' request message to FTP Server.
	// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter false 
	// to send 'QUIT' request message.
    handleSimpleCommandResponse("QUIT", false);

	// Quits from the application.
    exit(0);
}

void handlePassive(pasvNextCmd pasvNext) {
    // Sends a 'PASV' request message to the FTP server.
    sendOnControl("PASV", 4);

    // Receives the response against PASV request message from the server.
    char buffer[DATA_SOCKET_RECEIVE_BUFFER_SIZE] = {0};
    int ret = receiveOnControl(buffer, DATA_SOCKET_RECEIVE_BUFFER_SIZE);
    buffer[ret]='\0';
    showFtpResponse(buffer);

    // If the response is a successful one, retreives data-connection listener port number form the response.
    // Retreives data-connection listener port number form the response 
    if(atoi(buffer) < 400 && ret > 0){
        char hostIP[13];
        int hostPort;

    	// using function getHostIPAndPortFromPassiveSuccessResponse().
        getHostIPAndPortFromPassiveSuccessResponse(buffer, hostIP, hostPort);

        // Requests a data connection to the server on the listener port at the server.
        connectToServerData(hostIP, hostPort);
    } else {
        handleCommandQuit();
    }
}

void handleNLIST() {
	// Sends a 'NLST' request message to the server on the control connection.
    sendOnControl("NLST", 4);

	// Receives the response against NLST request from the server on the control connection.
    char buffer[DATA_SOCKET_RECEIVE_BUFFER_SIZE];
    int ret = receiveOnControl(buffer, DATA_SOCKET_RECEIVE_BUFFER_SIZE);
    buffer[ret]='\0';

    // If the response is successful, retrieves the list of entries in server's current directory 
    if (atoi(buffer) < 400 && ret > 0) {
        cout<< "\n";
        showFtpResponse(NLST_CONNECTION_OPEN_RESPONSE);
		ret = receiveOnControl(buffer, DATA_SOCKET_RECEIVE_BUFFER_SIZE);
		buffer[ret]='\0';
		showFtpResponse(buffer);
		ret=receiveOnData(buffer, DATA_SOCKET_RECEIVE_BUFFER_SIZE);
        buffer[ret]='\0';

		// on the data connection.
	    // Displays the list of entries to the user.
		showFtpDirResponse(buffer);
		cout<< "\n";
	}
}

void handleRETR(std::string filename) {
    // Sends a 'RETR <filename>' request message to the server on the control connection.
    string ptr = "RETR "+filename;
    sendOnControl(ptr.c_str(), ptr.length());

    // Receives the response against RETR request from the server on the control connection.
    char buffer[DATA_SOCKET_RECEIVE_BUFFER_SIZE] = {0};
    int ret = receiveOnControl(buffer, DATA_SOCKET_RECEIVE_BUFFER_SIZE);
    buffer[ret]='\0';

    // If the response is successful, retrieves the content of the file on the data connection.
    if(atoi(buffer) < 400 && ret > 0){
        showFtpResponse(NLST_CONNECTION_OPEN_RESPONSE);
        ret = receiveOnControl(buffer, DATA_SOCKET_RECEIVE_BUFFER_SIZE);
        buffer[ret]='\0';
        showFtpResponse(buffer);

        ret = receiveOnData(buffer, DATA_SOCKET_RECEIVE_BUFFER_SIZE);
        buffer[ret]='\0';
        ofstream fout;
        fout.open(filename);

        //check if file is open
        if (fout.fail()) {
            cout << "Unable to open file." << endl;
            cout << "Data cannnot save back to the file."<<endl;
            exit(1);
        }
        // out the data to the opened file
        fout << buffer << endl;
        //close file
        fout.close();
        cout << "\t" << ret << " bytes has been saved into file " << filename << ".\n";
    }
}

void handleSimpleCommandResponse(std::string ftpCommand, bool checkAuthentication) {
    // Sends 'ftpCommand' request message to FTP server on the control connection.
    sendOnControl(ftpCommand.c_str(), ftpCommand.length());

    // Receives the response from the server against the request.
    char buffer[DATA_SOCKET_RECEIVE_BUFFER_SIZE];
    int ret = receiveOnControl(buffer, DATA_SOCKET_RECEIVE_BUFFER_SIZE);
    buffer[ret]='\0';

    // Displays the response to the user.
    showFtpResponse(buffer);

    // If the response is unsuccessful and checkAuthentication parameter value is true, quits the application.
    if(atoi(buffer) >= 400 && checkAuthentication == true) {
        handleCommandQuit();
    }
}

void getHostIPAndPortFromPassiveSuccessResponse(char* response, char* hostIP, int& hostPort) {
    // Retrieves IP address of FTP Server from the response string into reference 'hostIP'.
    // Retrives data-connection listening port number of FTP server from the response string into reference 'hostPort'. 
    // The example of a successful response message is "227 Entering Passive Mode (192,168,1,65,202,143)."
    // From the above response message 'hostIP' will be 192.168.1.65 and 'hostPort' will be
    // (202x256 + 143) or 51855.


    //Tokenize the response so that we can pass the IP and the PORT separately
    char *token;
    token = strtok (response," ,.-");

    int i = 1;

    while (i<5){
        token = strtok (NULL, " ,.-()");
        i++;
    }

    string ip = token;

    while (i<8){
        token = strtok (NULL, " ,.-()");
        ip=ip+"."+token;
        i++;
    }

    token = strtok (NULL, " ,.-()");
    string p1 = token;

    token = strtok (NULL, " ,.-()");
    string p2 = token;

    strcpy(hostIP, ip.c_str());
    hostPort= stoi(p1)*256 + stoi(p2);
}
