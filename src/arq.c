// Biblioteca arq.h
#include "arq.h"

// Copia o arquivo de src para dest
int cpyfile_arq(char *src , char *dest, char *err_msg)
{
    char *buf = malloc(BUFFER_SIZE_);
    int bytes , error = 0;
    FILE *src_file , *dest_file;

    // Abre o arquivo de origem
    src_file = fopen(src , "rb");
    if(src_file == NULL) 
    {
        strcpy(err_msg , ERR_OPEN_FILE);
        return -1;
    }

    // Cria o novo arquivo
    dest_file = fopen(dest , "wb");
    if(dest_file == NULL)
    {
        strcpy(err_msg , ERR_CREATE_FILE);
        return -1;
    }

    // Copia o arquivo
    while((bytes = fread(buf , 1 , BUFFER_SIZE_ , src_file)) > 0)
    {
        if(bytes < 0)
        {   
            error = 1;
            strcpy(err_msg , ERR_READ_FILE);
            break;
        }
        fwrite(buf, 1 , bytes , dest_file);
    }

    fclose(src_file);
    fclose(dest_file);
    free(buf);
    return (error == 1 ? -1 : 0);
}

// Retorna o nome do arquivo
char *getfilename_arq(char *path)
{
    char *nome_arquivo = path;

    // Encontra o nome do arquivo
    if(OS_WINDOWS) // Se for windows
    {
        for(;*path != '\0';path++)
        {
            if(*path == '\\') nome_arquivo = path + 1;
        }
    }
    else // Se for linux
    {
        for(;*path != '\0';path++)
        {
            if(*path == '/') nome_arquivo = path + 1;
        }
    }

    return nome_arquivo;
}

// Retorna o tamanho do arquivo. Em caso de erros retorna -1
long int getfilesize_arq(FILE *arq)
{
    // Verifica se o argumento passado é nulo
    if(arq == NULL) return -1;

    /* Aponta o fluxo do arquivo para o fim
    dessa forma conseguindo o tamanho do arquivo com ftell */
    fseek(arq , 0 , SEEK_END);

    // Recebe o tamanho do arquivo
    long int tamanho_arquivo = ftell(arq);

    // Aponta o fluxo do arquivo para o inicio
    fseek(arq , 0 , SEEK_SET);

    return tamanho_arquivo;
}

// Envia o arquivo
int sendfile_arq(int sock ,char *nome_arquivo , char *err_msg)
{   
    FILE *arquivo;
    char *buffer = malloc(BUFFER_SIZE_) , confirm[3] , info_arquivo[1024];
    long int BytesLidos = 0 , tamanho_arquivo; int status = 0;

    // Obtendo o nome do arquivo
    char *filename = getfilename_arq(nome_arquivo);

    // Verifica erro no socket.
    if(sock == -1)
    {   
        strcpy(err_msg , ERR_SOCKFD);
        free(buffer);
        return -1;
    }    

    // Abre o arquivo
    arquivo = fopen(nome_arquivo , "rb");
    if(arquivo == NULL)
    {    
        strcpy(err_msg , ERR_OPEN_FILE);
        free(buffer);
        return -1;
    }

    // Obtendo o tamanho do arquivo
    tamanho_arquivo = getfilesize_arq(arquivo);

    // Tamanho e nome do arquivo
    sprintf(info_arquivo , "%s:%li", filename , tamanho_arquivo);

    // Envia o nome e tamanho do arquivo
    status = send(sock , info_arquivo , strlen(info_arquivo) , 0);
    if(status == -1)
    {   
        strcpy(err_msg , ERR_CONECTION);
        fclose(arquivo);
        free(buffer);    
        return -1;
    }

    // Aguarda resposta
    status = recv(sock , confirm , sizeof (confirm) , 0);
    if(status == -1)
    {
        strcpy(err_msg , ERR_CONECTION);
        fclose(arquivo);
        free(buffer);
        return -1;
    }

    // Envia o arquivo
    while( (BytesLidos = fread(buffer , 1 , BUFFER_SIZE_ , arquivo)) > 0 )
    {
        if(BytesLidos < 0)
        {   
            strcpy(err_msg , ERR_READ_FILE);
            status = 1;
            break;
        }

        status = send(sock , buffer , BytesLidos , 0);
        if(status == -1)
        {
            status = 1;
            strcpy(err_msg , ERR_SEND_FILE);
            break;
        }

    }

    fclose(arquivo);
    free(buffer);
    return (status == 1 ? -1 : 0);
}

// Recebe o arquivo
int getfile_arq(int sock , char *err_msg)
{
    FILE *arquivo;
    char *buffer = malloc(BUFFER_SIZE_) , nome_arquivo[256] , info_arquivo[1024] , *temp;
    long int BytesRecebidos = 0 , tamanho_arquivo , total = 0; int status = 0;

    // Verifica erro no socket
    if(sock == -1)
    {
        strcpy(err_msg , ERR_SOCKFD);
        free(buffer);
        return -1;
    }

    // Recebe o nome e tamanho do arquivo
    status = recv(sock , info_arquivo , sizeof info_arquivo , 0);
    if(status == -1)
    {
        strcpy(err_msg , ERR_CONECTION);
        free(buffer);
        return -1;
    }

    // Obtendo as informações do arquivo
    int i;
    for(i = 0; info_arquivo[i] != ':'; ++i) nome_arquivo[i] = info_arquivo[i];
    nome_arquivo[i] = '\0';
    temp = (strstr(info_arquivo , ":") + 1);
    tamanho_arquivo = atoi(temp);

    // Informa que recebeu as informações do arquivo
    status = send(sock , "OK" , 2  , 0);
    if(status == -1)
    {
        strcpy(err_msg , ERR_CONECTION);
        free(buffer);
        return -1;
    }

    // Cria o arquivo
    arquivo = fopen(nome_arquivo , "w+b");
    if(arquivo == NULL)
    {
        strcpy(err_msg , ERR_CREATE_FILE);
        free(buffer);
        return -1;
    }

    // Recebe o arquivo
    while(total < tamanho_arquivo)
    {
        BytesRecebidos = recv(sock , buffer , BUFFER_SIZE_ , 0);
        if(BytesRecebidos < 0)
        {
            strcpy(err_msg , ERR_CONECTION);
            status = 1;
            break;
        }
        fwrite(buffer , 1 , BytesRecebidos , arquivo);
        total += BytesRecebidos;
    }

    fclose(arquivo);
    free(buffer);
    return (status == 1 ? -1 : 0);
}
