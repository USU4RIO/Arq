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

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE_ (10 * 1024 * 1024)
#define ERR_OPEN_FILE "[INFO]: Erro ao abrir arquivo"
#define ERR_CREATE_FILE "[INFO]: Erro ao criar arquivo"
#define ERR_READ_FILE "[INFO]: Erro ao ler arquivo"
#define ERR_WRITE_FILE "[INFO]: Erro ao gravar arquivo"
#define ERR_SOCKFD "[INFO]: Descritor invalido"
#define ERR_CONECTION "[INFO]: Erro na conexão"
#define ERR_SEND_FILE "[INFO]: Erro ao enviar o arquivo"

int sendfile_arq(int sock ,char *nome_arquivo , char *err_msg); // Manda arquivo
int getfile_arq(int sock , char *err_msg); // Recebe arquivo
char *getfilename_arq(char *path); // Retorna o nome do arquivo
long int getfilesize_arq(FILE *arq); // Retorna o tamanho do arquivo
int cpyfile_arq(char *src , char *dest, char *err_msg); // Copia o arquivo de src para dest

#endif