// Arquivo.h
#ifndef ARQ_H      
#define ARQ_H 

// Diferenciar OS
#ifdef __unix__ 

    #define OS_WINDOWS 0
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
int SendFileForSocket(int sock ,char *FileName); // Manda arquivo
int GetFileFromSocket(int sock); // Recebe arquivo
char *GetFileName(char *path); // Retorna o nome do arquivo
long int _GetFileSize_(FILE *arq); // Retorna o tamanho do arquivo
int _CopyFile_(char *src , char *dest); // Copia o arquivo de src para dest

#endif // Fim