 /*
File: ftp_client_ui.cpp
Project: CSCI460 Project 1
Author: Tiffany Elliott & Qijie (Ben) Lao
*/
#include "ftp_client_ui.hpp"
#include "ftp_client_command.hpp"
#include "ftp_server_response.hpp"
#include <iostream>
#include <string.h>
#include <sstream>
using namespace std;

#define MAX_LENGTH 80
#define FTP_CLIENT_PROMT "csci460Ftp>>"


void getUserCommand() {
	// Displays a command line prompt as follows:
	//      csci460Ftp>>
	cout << FTP_CLIENT_PROMT;
	string command;
	//cin >> command;
    getline(cin, command);
	// Lets user to type command and reads the user command from the prompt.
	// Interprets user command by calling function interpreteUserCommand().
	interpreteUserCommand(command);
}

void interpreteUserCommand(std::string command) {
    // Stream class to operate on strings.
    stringstream cmd; 
    // a copy of the command contents in the stream buffer.
    cmd.str (command);
    cmd >> command; // command
    string args;
    cmd >> args; // argument after command
    // Interprets user command and call appropriate function from 'ftp_client_command.hpp'.
    if (command == FTP_CLIENT_USER_COMMAND_HELP) { // "help" command
		handleCommandHelp();
	} else if (command == FTP_CLIENT_USER_COMMAND_USER) { // "user" command
		if (args.length() != 0) {
            handleCommandUser(args);
        }else{
            handleCommandHelp();
        }
	} else if (command == FTP_CLIENT_USER_COMMAND_PASSWORD) { // "pass" command
		if (args.length() != 0) {
            handleCommandPassword(args);
        }else{
            handleCommandHelp();
        }
	} else if (command == FTP_CLIENT_USER_COMMAND_DIRECTORY) { // "dir" command
		handleCommandDirectory();
	} else if(command == FTP_CLIENT_USER_COMMAND_PRINT_DIRECTORY){ // "pwd" command
		handleCommandPrintDirectory();
	} else if (command == FTP_CLIENT_USER_COMMAND_CHANGE_DIRECTORY) { // "cwd" command
		if (args.length() != 0) {
            handleCommandChangeDirectory(args);
        }else{
            handleCommandHelp();
        }
	} else if (command == FTP_CLIENT_USER_COMMAND_CHANGE_DIRECTORY_UP) { // "cdup" command
		handleCommandChangeDirectoryUp();
	} else if (command == FTP_CLIENT_USER_COMMAND_GET) { // "get" command
		if (args.length() != 0) {
            handleCommandGetFile(args);
        }else{
            handleCommandHelp();
        }
	} else if (command == FTP_CLIENT_USER_COMMAND_QUIT) { // "quit" command
		handleCommandQuit();
	} else {
		cout << FTP_CLIENT_PROMT << "Invalid command!" <<endl;
        handleCommandHelp();
	}
}

void showFtpResponse(std::string response) {
	// Displays server response to the user on the prompt.
	// For example, success response of user command
	//      csci460Ftp>>user csci460
	// is displays as 
	//      csci460Ftp>>331 Username okay, need password.
	cout << "\t"<<response << endl;
}

void showFtpDirResponse(std::string response) { // Displays server response to the user against user command
    //      csci460Ftp>>dir
    cout << response <<endl;
    // as 
    //      csci460Ftp>>226 Directory listing (11) entries sent OK . Closing data connection..
    //
    //      D       .
    //      D       ..
    //      D       bin
    //      F       makefile                2631
    //      D       include
    //      F       README                  0
    //      D       build
    //      D       resource
    //      D       src
}

