// Arquivo.h
#ifndef ARQ_H      
#define ARQ_H 

// Diferenciar OS
#ifdef __unix__ 

    #define OS_WINDOWS 0
    #include <netdb.h>
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>

#elif defined(_WIN32) || defined(WIN32)

    #define OS_WINDOWS 1
    #include <windows.h>
    #include <winsock2.h>

#endif // Fim OS


// Include 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define
#define BUFFER_SIZE_ (10 * 1024 * 1024)

// Funções 
int SendFileForSocket(int sock , const char *path , const char *NameOfFile); // Manda arquivo
int GetFileFromSocket(int sock); // Recebe arquivo

#endif // Fim