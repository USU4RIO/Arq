// Biblioteca arq.h
#include "arq.h"

/*******************************************
*   Envia um arquivo para o socket passado *
*   no primeiro parâmetro.                 *
*                                          *
*   No segundo parâmetro deve ser passado  *
*   o caminho completo ate o arquivo.      *
*                                          *
*   Retorna 0 se tiver sucesso ou retorna  *
*   um numero menor que 0 caso ocorra erro *
*                                          *
*   O terceiro parâmetro deve receber o    *
*   nome do arquivo completo e com o tipo. *
*   exemplo: Nomedoarquivo.exe , .jpg ,    *
*   .mp4...                                *
*                                          *
*   Retorno: -1 erro socket , -2 erro ao   *
*   abri arquivo , -3 erro ao enviar ,     *
*   0 sucesso.                             *
*******************************************/
int SendFileForSocket(int sock , const char *path , const char *NameOfFile)
{   
    // Variaveis
    FILE *arquivo;
    char *buffer = malloc(BUFFER_SIZE_) , info[10];
    int BytesLidos = 0 , status = 0;

    // Verifica erro no socket.
    if(sock == -1)
    {   
        free(buffer);
        return -1;
    }    

    // Abre o arquivo
    arquivo = fopen(path , "rb");
    if(arquivo == NULL)
    {    
        free(buffer);
        return -2;
    }

    // Envia o nome do arquivo
    status = send(sock , NameOfFile , strlen(NameOfFile) , 0);
    if(status == -1)
    {   
        fclose(arquivo);
        free(buffer);    
        return -1;
    }

    // Aguarda resposta
    status = recv(sock , info , sizeof info , 0);
    if(status == -1)
    {
        fclose(arquivo);
        free(buffer);
        return -1;
    }

    // Envia o arquivo
    while( (BytesLidos = fread(buffer , 1 , BUFFER_SIZE_ , arquivo)) > 0 )
    {
        if(BytesLidos < 0)
        {   
            fclose(arquivo);
            free(buffer);
            return -3;
        }

        status = send(sock , buffer , BytesLidos , 0);
        if(status == -1)
        {
            fclose(arquivo);
            free(buffer);
            return -1;
        }

    }

    // Fim - Sucesso
    fclose(arquivo);
    free(buffer);
    return 0;
}

/*******************************************
*   Recebe um arquivo do socket passado    *
*   como argumento .                       *
*                                          *
*   Retorno: 0 sucesso , -1 erro socket ,  *
*   -2 erro ao criar arquivo , -3 erro ao  *
*   receber arquivo.                       *
*******************************************/
int GetFileFromSocket(int sock)
{
    // Variaveis
    FILE *arquivo;
    char *buffer = malloc(BUFFER_SIZE_) , nome_arquivo[256];
    int BytesRecebidos = 0 , status = 0;

    // Verifica erro no socket
    if(sock == -1)
    {
        free(buffer);
        return -1;
    }

    // Recebe o nome do arquivo
    status = recv(sock , nome_arquivo , sizeof nome_arquivo , 0);
    if(status == -1)
    {
        free(buffer);
        return -1;
    }
    nome_arquivo[status] = '\0';

    // Informa que recebeu o nome do arquivo
    status = send(sock , "OK" , 2  , 0);
    if(status == -1)
    {
        free(buffer);
        return -1;
    }

    // Cria o arquivo
    arquivo = fopen(nome_arquivo , "w+b");
    if(arquivo == NULL)
    {
        free(buffer);
        return -2;
    }

    // Recebe o arquivo
    while( (BytesRecebidos = recv(sock , buffer , BUFFER_SIZE_ , 0)) > 0 )
    {
        if(BytesRecebidos < 0)
        {
            fclose(arquivo);
            free(buffer);
            return -3;
        }

        fwrite(buffer , 1 , BytesRecebidos , arquivo);

    }

    // Fim - Sucesso
    fclose(arquivo);
    free(buffer);
    return 0;
}