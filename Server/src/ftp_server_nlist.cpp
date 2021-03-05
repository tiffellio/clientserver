//#define DEFAULT_DIRECTORY_PATH "./"
#include <dirent.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "ftp_server_nlist.hpp"
#include "ftp_server_response.hpp"

int listDirEntries (int dataSockDescriptor) {
//Determines and sends the list of current directory entries to the client using 
//the data connection, represented by 'dataSockDescriptor'.
	struct dirent *cde;
	DIR *dd = opendir(DEFAULT_DIRECTORY_PATH);

	//technique from: https://pubs.opengroup.org/onlinepubs/009695399/basedefs/dirent.h.html
	char buff[FTP_RESPONSE_MAX_LENGTH];
	int count = 0;
	while((cde = readdir(dd)) != NULL){
		strcat(buff, cde -> d_name);
		strcat(buff, " ");
		count ++;
	}
	
	//Returns the count of entries to the caller.
	send(dataSockDescriptor, &buff, sizeof(buff), 0);
	return count;
}