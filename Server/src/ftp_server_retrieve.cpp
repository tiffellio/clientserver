#include <string.h>
#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include "ftp_server_retrieve.hpp"
#include "ftp_server_connection.hpp"
#include "ftp_server_response.hpp"
using namespace std;

// #define FILE_OPEN_MODE "r"
// #define DATA_SOCKET_SEND_BUFFER_SIZE 65536 //64K

int sendFile(char* filename, int& dataSockDescriptor) {
    //Returns the total number of bytes sent to the client.
    char *line;

    ifstream ifs;
    //Checks whether the file with the 'filename' is accessible or not.
    ifs.open(filename, std::ifstream::in);

    if (ifs.fail()) {
        return 0;
    }

    int size = ifs.tellg();
    ifs.seekg(0, ios::beg);
    line = new char[size];
    bzero(line, size);
    ifs.read(line, size);
    ifs.close();

    send(dataSockDescriptor, line, DATA_SOCKET_SEND_BUFFER_SIZE, 0);
    return size;
/*
    int bytesSent =0;
    //DATA_SOCKET_SEND_BUFFER_SIZE is used as the chunk size.
    char buffer[DATA_SOCKET_SEND_BUFFER_SIZE];
    //If the file is accessible, opens the file and reads and sends the file content chunk by chunk to the client.
    while (!ifs.eof()){
        ifs.get(buffer, DATA_SOCKET_SEND_BUFFER_SIZE);
        //Sends file chunks using data connection represented by 'dataSockDescriptor'.
        bytesSent += send(dataSockDescriptor, buffer, DATA_SOCKET_SEND_BUFFER_SIZE, 0 );
    }
    return bytesSent;
*/


}

