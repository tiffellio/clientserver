#include <string>
#include <string.h>
#include <ctype.h>
#include "ftp_server_string_util.hpp"

void replaceCharWith(char* str, char find, char replace) {
//Replaces all the occurrences of 'find' character in 'str' with 'replace' character.
    int index = 0;
    while(str[index]!='\0'){
        if(str[index] == find){
            str[index] = replace;
        }
        index++;
    }
}

bool startsWith(const char* prefix, const char* str) {
//Returns true if 'str' starts with 'prefix'.
    int index = 0;
    while (str[index] != '\0') {
        if (str[index] != prefix[index]) {
            return false;
        }
        index++;
    }
    return true;
}

void toUpper(char* str) {
    //Changes all characters of 'str' to upper case.
    int index = 0;
    while(str[index]!='\0'){
        str[index]=toupper(str[index]);
        index++;
    }
}

char* stripSpaceAtStart(char* str) {
    //Removes all the spaces, if there is any, from the beginning of 'str'.
    int i = 0;
    int j = 0;
    while(str[i]!=' ')
        i++;


    std::string line(str);
    for(j =0; i< (int)line.length(); i++, j++)
        str[j]=str[i];

    str[j]='\0';

    return str;
}

void stripNewlineAtEnd(char* str) {
    //Removes new line character ('\n'), if there is any, from the end of 'str'.
    if(str[strlen(str) - 1] == '\n')
        str[strlen(str) - 1] = '\0';
}
