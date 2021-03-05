/*
#define COMMAND_BUFFER_SIZE 512
#define COMMAND_CODE_CHARACTER_COUNT 5
#define COMMAND_ARG_MAX_CHARACTER_COUNT 256
#define COMMAND_DELIMITER " "

#define DEFAULT_USERNAME "csci460"
#define DEFAULT_USERNAME_LENGTH 7
#define DEFAULT_PASSWORD "460pass"
#define DEFAULT_PASSWORD_LENGTH 7


#define COMMAND_USER "USER"
#define COMMAND_PASSWORD "PASS"
#define COMMAND_PWD "PWD"
#define COMMAND_CWD "CWD"
#define COMMAND_CDUP "CDUP"
#define COMMAND_PASV "PASV"
#define COMMAND_NLST "NLST"
#define COMMAND_RETR "RETR"
#define COMMAND_QUIT "QUIT"



#define PATH_NOT_ALLOWED_CURERENT_DIRECTORY "./"
#define PATH_NOT_ALLOWED_PARENT_DIRECTORY "../"
*/
#include <sstream>
#include <unistd.h>
#include <string>
#include <string.h>

#include "ftp_server_nlist.hpp"
#include "ftp_server_retrieve.hpp"
#include "ftp_server_response.hpp"
#include "ftp_server_command.hpp"
#include "ftp_server_connection.hpp"
#include "ftp_server_passive.hpp"

using namespace std;

void interpreteCommand(char* command, int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor, 
                       bool& isClientConnected, bool& isUser, bool& isLoggedIn, const char* rootDir){
	//Separates the command and its argument calling 'parseCommandLine()' function.
	//Determines, which valid command (FTP request) has been sent, only the commands defined in this header file are valid for this FTP server.
	//Calls appropriate 'handleCommandXXXX()' function to handle a valid command.
	//Calls 'handleCommandUnSupported()' if an invalid command has been received.
    char str[COMMAND_CODE_CHARACTER_COUNT];
    char cmp[COMMAND_ARG_MAX_CHARACTER_COUNT];
    parseCommandLine(command, str, cmp);
    if(strcmp(str,COMMAND_USER)) {
        handleCommandUSER(cmp, controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected, isUser);
    } else if (strcmp(str,COMMAND_PASSWORD)) {
        handleCommandPASS(cmp, controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected, isUser, isLoggedIn);
    } else if (strcmp(str,COMMAND_PWD)) {
        handleCommandPWD(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor,isClientConnected, isLoggedIn);
    } else if (strcmp(str,COMMAND_CWD)) {
        handleCommandCWD(cmp, controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected, isLoggedIn);
    } else if (strcmp(str,COMMAND_CDUP)) {
        handleCommandCDUP(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected, isLoggedIn, rootDir);
    } else if (strcmp(str,COMMAND_PASV)) {
        handleCommandPASV(cmp, controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected, isLoggedIn);
    } else if (strcmp(str,COMMAND_NLST)) {
        handleCommandNLST(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected, isLoggedIn);
    } else if (strcmp(str,COMMAND_RETR)) {
        handleCommandRETR(cmp, controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected, isLoggedIn);
    } else if (strcmp(str,COMMAND_QUIT)) {
        handleCommandQUIT(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
    } else {
        handleCommandUnSupported(controlSockDescriptor);
    }
}

void parseCommandLine(char* commandLine, char* command, char* argument) {
	//Breaks the command line string that has been received through the first argument 'commandLine' into its parts 'command' and 'argument'.
	//Assigns command part to reference parameter 'command' and argument part to reference parameter 'argument'.
	//Caller retrieves the parts through the references ('command' and 'argument').
	string args(commandLine);
	stringstream cmd; 
    // a copy of the command contents in the stream buffer.
    cmd.str (args);
    cmd >> command; // command
    cmd >> argument; // argument after command
}

void handleCommandUSER(char* username, int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor, bool& isClientConnected, bool& isUser){
	//Handles USER command by comparing 'username' with the DEFAULT_USERNAME.
	//If the 'username' matches, sets 'true' to 'isUser' and sends appropriate response to the client.
	//If the 'username' does not match, sends appropriate response to the client and closes all connections 
		//by calling 'closeAllConnections()' function from 'ftp_server.connection.hpp'.
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
	if (strcmp(username, DEFAULT_USERNAME)) {																	//==0?
        isUser = 1;
        sendToRemote(dataListenerSockDescriptor, USERNAME_OK_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
    } else {
        sendToRemote(dataListenerSockDescriptor, INVALID_USERNAME_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
        closeAllConnections(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
    }
}

void handleCommandPASS(char* password, int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor, bool& isClientConnected, bool& isUser, bool& isLoggedIn){
	//Handles PASS command by comparing 'password' with the DEFAULT_PASSWORD.
	//It also checks whether USER command was successful or not before this PASS command by checking the value in 'isUser'.
	//A 'false' value in 'isUser' means USER command has not been successful before this PASS command.
	//If there was no successful USER command, sends an appropriate response to the client and closes all connections by calling 'handleNotLoggedIn()' function.
	//If the password does not match, sends appropriate response to the client and closes all connections by calling ''handleNotLoggedIn()' function.
	//If the password matches, sets 'true' to 'isLoogedIn' and sends appropriate response to the client.
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
	if (isUser==0) {
		handleNotLoggedIn(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
		return;
	} 

	if (strcmp(password, DEFAULT_PASSWORD) != 0) { //!==?
		handleNotLoggedIn(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
	} else {
        isLoggedIn = 1;
        sendToRemote(dataListenerSockDescriptor, LOGIN_RRESPONSE, FTP_RESPONSE_MAX_LENGTH);
	}
}

void handleCommandPWD(int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor, bool& isClientConnected, bool& isLoggedIn){
	//Handles PWD command.
	//First, checks whether the client is logged in or not by checking the value in 'isLoggedIn'.
	//If the client is not logged in, sends an appropriate response to the client and closes all connections by calling 'handleNotLoggedIn()' function.
	//If the client is logged in, determines the current working directory sends it to the client in an appropriate response.
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
	char *buff;
	if (isLoggedIn == 0) {
        handleNotLoggedIn(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
	} else {
		buff = get_current_dir_name();
		sendToRemote(dataListenerSockDescriptor, buff, sizeof(buff));
	}
}

void handleCommandCWD(char* directory, int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor, bool& isClientConnected, bool& isLoggedIn){
	//Handles CWD command.
	//First, checks whether the client is logged in or not by checking the value in 'isLoggedIn'.
	//If the client is not logged in, sends an appropriate response to the client and closes all connections by calling 'handleNotLoggedIn()' function.
	//If the client is logged in, determines whether the requested 'directory' is valid or not.
	//A requested directory is valid only if it is a sub directory of the current working directory.
	//If the requested directory is not a valid directory, sends an appropriate response to the client.
	//If the requested directory is valid, changes the current directory to the requested directory and sends an appropriate response to the client.
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
	if (isLoggedIn == 0) {
        handleNotLoggedIn(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
    } else {

        if (chdir(directory) != 0) {
            sendToRemote(dataListenerSockDescriptor, INVALID_PATH_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
        } else {
            char cwd[COMMAND_ARG_MAX_CHARACTER_COUNT];
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                sendToRemote(dataListenerSockDescriptor, CWD_FAIL_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
            } else {
                sendToRemote(dataListenerSockDescriptor, CHANGE_DIRECTORY_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
            }
        }
    }
}

void handleCommandCDUP(int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor, bool& isClientConnected, bool& isLoggedIn, const char* rootDir){
	//Handles CDUP command.
	//First, checks whether the client is logged in or not by checking the value in 'isLoggedIn'.
	//If the client is not logged in, sends an appropriate response to the client and closes all connections by calling 'handleNotLoggedIn()' function.
	//If the client is logged in, determines whether moving up to the parent directory is not falling beyond the project's root directory.
	//Project's root directory is passed in the last parameter 'rootDir'.
	//If the move does not go beyond project's root directory, changes the current working directory to the parent directory and sends an appropriate response to the client.
	//If the the move goes beyond project's root directory, sends an appropriate response to the client.
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
	if (isLoggedIn == 0) {
		handleNotLoggedIn(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
	} else {
		char *buff = get_current_dir_name();
		if (strcmp(buff, rootDir) != 0) {
            chdir(PATH_NOT_ALLOWED_PARENT_DIRECTORY);
            sendToRemote(dataListenerSockDescriptor, CHANGE_TO_PARENT_DIRECTORY_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
        } else {
            sendToRemote(dataListenerSockDescriptor, CDUP_FAIL_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
        }
	}

}
void handleCommandPASV(char* argument, int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor, bool& isClientConnected, bool& isLoggedIn){
	//Handles PASV command.
	//First, checks whether the client is logged in or not by checking the value in 'isLoggedIn'.
	//If the client is not logged in, sends an appropriate response to the client and closes all connections by calling 'handleNotLoggedIn()' function.
	//If the client is logged in, enters into passive mode by calling 'enteringIntoPassive()' function from 'ftp_server_passive.hpp'
	//'enteringIntoPassive()' function starts a passive connection listener, sends an appropriate response to the client.
	//'enteringIntoPassive()' function waits for a specific time interval for the client to send a connection request on the listener.
	//'enteringIntoPassive()' function accepts client's connection request, if there is any, and opens a data connection with the client.
	//'enteringIntoPassive()' function closes the connection listener after opening the data connection with the client.
	//'enteringIntoPassive()' function also sends appropriate response to the client using control connection.
	if (isLoggedIn == 0){
        handleNotLoggedIn(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
    }else{
        enteringIntoPassive(argument, controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor);
    }
}

void handleCommandNLST(int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor, bool& isClientConnected, bool& isLoggedIn){
	//Handles NLST command.
	//First, checks whether the client is logged in or not by checking the value in 'isLoggedIn'.
	//If the client is not logged in, sends an appropriate response to the client and closes all connections by calling 'handleNotLoggedIn()' function.
	//If the client is logged in, it checks whether the data connection has already been established by a PASV command before this NLST command.
	//If a data connection has already been established 'dataSockDescriptor' parameter should have a non-negative value.
	//If a data connection has not been established, sends an appropriate response to the user.
	//If a data connection has already been established, prepares and sends the list of current directory entries by calling 'listDirEntries()' function
	//from 'ftp_server_nlist.hpp'. 'listDirEntries()' function sends the list of current directory entries using the data connection
	//and returns the count of entries.
	//Sends the count of entries in an appropriate response message to the client using the control connection.
	//Closes the data connection.  
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
	if (isLoggedIn == 0) {
		handleNotLoggedIn(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
	} else {
		if (dataSockDescriptor < 1) {
            sendToRemote(dataListenerSockDescriptor, DATA_OPEN_CONNECTION_ERROR_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
        } else {
            int count = listDirEntries(dataSockDescriptor);  //wat
            char response[FTP_RESPONSE_MAX_LENGTH];
            sprintf(response, NLST_CONNECTION_CLOSE_RESPONSE, count);
            sendToRemote(dataListenerSockDescriptor, response, FTP_RESPONSE_MAX_LENGTH); 
            closeConnection(dataSockDescriptor);
        }
	}
}

void handleCommandRETR(char* argument, int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor, bool& isClientConnected, bool& isLoggedIn){
	//Handles RETR command.
	//First, checks whether the client is logged in or not by checking the value in 'isLoggedIn'.
	//If the client is not logged in, sends an appropriate response to the client and closes all connections by calling 'handleNotLoggedIn()' function.
	//If the client is logged in, it checks whether the data connection has already been established by a PASV command before this RETR command.
	//If a data connection has already been established 'dataSockDescriptor' parameter should have a non-negative value.
	//If a data connection has not been established, sends an appropriate response to the user.
	//If a data connection has already been established, reads from the file and sends the content of the file by calling 'sendFile()' function
	//from 'ftp_server_retrieve.hpp'. 'sendFile()' function sends the content of the file using the data connection
	//and returns the count of the bytes sent.
	//Sends the count of the bytes sent in an appropriate response message to the client using the control connection.
	//Closes the data connection.  
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
	if (isLoggedIn == 0) {
		handleNotLoggedIn(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
	} else {
		if (dataSockDescriptor < 1) {
            sendToRemote(dataListenerSockDescriptor, DATA_OPEN_CONNECTION_ERROR_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
        } else {
        	char *buff;
            sendFile(buff, dataSockDescriptor);  //wat
            sendToRemote(dataListenerSockDescriptor, RETR_CONNECTION_CLOSE_RESPONSE, FTP_RESPONSE_MAX_LENGTH); 
            closeConnection(dataSockDescriptor);
        }
	}
}

void handleCommandQUIT(int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor,bool& isClientConnected){
	//Handles QUIT command by sending appropriate response to the client.
	//Closes all connections by calling 'closeAllConnections()' function from 'ftp_server.connection.hpp'.
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
    sendToRemote(dataListenerSockDescriptor, QUIT_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
    closeAllConnections(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
}

void handleCommandUnSupported(const int controlSockDescriptor) {
	//Sends an appropriate response saying the requested command is not supported by this FTP server.
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
    sendToRemote(controlSockDescriptor, UNSUPPORTED_COMMAND_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
}

void handleNotLoggedIn(int& controlSockDescriptor, int& dataListenerSockDescriptor, int& dataSockDescriptor, bool& isClientConnected){
	//Sends an appropriate response saying the user is not logged in.
	//Closes all connections by calling 'closeAllConnections()' function from 'ftp_server.connection.hpp'.
	//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
    sendToRemote(dataListenerSockDescriptor, NOT_LOGGED_IN_RESPONSE, FTP_RESPONSE_MAX_LENGTH);
    closeAllConnections(controlSockDescriptor, dataListenerSockDescriptor, dataSockDescriptor, isClientConnected);
}
