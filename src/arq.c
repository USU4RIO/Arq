// Biblioteca arq.h
#include "arq.h"

/*******************************************
*                                          * 
*   Copia o arquivo de src para dest.      *
*                                          *
*   src = arquivo de origem.               *
*   dest = arquivo de destino.             * 
*                                          *
*   Retorno: 0 para sucesso, -1 para erro  * 
*                                          *
*******************************************/
int _CopyFile_(char *src , char *dest)
{
    // Variaveis
    char *buf = malloc(BUFFER_SIZE_);
    long int bytes;

    // Ponteiros para os arquivos
    FILE *src_file , *dest_file;

    // Abre o arquivo de origem
    src_file = fopen(src , "rb");
    if(src_file == NULL) return -1;

    // Cria o novo arquivo
    dest_file = fopen(dest , "wb");
    if(dest_file == NULL) return -1;

    // Copia o arquivo
    while((bytes = fread(buf , 1 , BUFFER_SIZE_ , src_file)) > 0)
    {
        if(bytes < 0)
        {   
            fclose(src_file);
            fclose(dest_file);
            free(buf);
            return -1;
        }

        fwrite(buf, 1 , bytes , dest_file);
    }

    // Fim
    fclose(src_file);
    fclose(dest_file);
    free(buf);
    return 0;
}

// Retorna o nome do arquivo
char *GetFileName(char *path)
{
    // Ponteiro para o nome do arquivo
    char *FileName = path;

    // Encontra o nome do arquivo
    if(OS_WINDOWS) // Se for windows
    {
        for(;*path != '\0';path++)
        {
            if(*path == '\\') FileName = path + 1;
        }
    }
    else // Se for linux
    {
        for(;*path != '\0';path++)
        {
            if(*path == '/') FileName = path + 1;
        }
    }

    return FileName;
}

// Retorna o tamanho do arquivo. Em caso de erros retorna -1
long int _GetFileSize_(FILE *arq)
{
    // Verifica se o argumento passado é nulo
    if(arq == NULL) return -1;

    /* Aponta o fluxo do arquivo para o fim
    dessa forma conseguindo o tamanho do arquivo com ftell */
    fseek(arq , 0 , SEEK_END);

    // Recebe o tamanho do arquivo
    long int FileSize = ftell(arq);

    /* Aponta o fluxo do arquivo para o inicio
    dessa forma voltando ao padrão */
    fseek(arq , 0 , SEEK_SET);

    return FileSize;

}

/*******************************************
*   Envia um arquivo para o socket passado *
*   no primeiro parâmetro.                 *
*                                          *
*   No segundo parâmetro deve ser passado  *
*   o caminho completo ate o arquivo.      *
*                                          *
*   Retorna 0 se tiver sucesso ou retorna  *
*   um numero menor que 0 caso ocorra erro *                                *
*                                          *
*   Retorno: -1 erro socket , -2 erro ao   *
*   abri arquivo , -3 erro ao enviar ,     *
*   0 sucesso.                             *
*******************************************/
int SendFileForSocket(int sock ,char *FileName)
{   
    // Variaveis
    FILE *arquivo;
    char *buffer = malloc(BUFFER_SIZE_) , info[10];
    long int BytesLidos = 0; int status = 0;
    char *NameFile = GetFileName(FileName);

    // Verifica erro no socket.
    if(sock == -1)
    {   
        free(buffer);
        return -1;
    }    

    // Abre o arquivo
    arquivo = fopen(FileName , "rb");
    if(arquivo == NULL)
    {    
        free(buffer);
        return -2;
    }

    // Envia o nome do arquivo
    status = send(sock , NameFile , strlen(NameFile) , 0);
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
    long int BytesRecebidos = 0; int status = 0;

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
