//
//  cliente.c
//  bananaFluServer
//
//  Created by Duarte Nuno Ferreira Duarte on 08/05/14.
//  Copyright (c) 2014 Duarte Nuno Ferreira Duarte. All rights reserved.
//

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#include "erros.h"

#define _DEBUG 0

#define _INCREMENTAR 1000
#define _AGREGAR 1001
#define _SIZEBUFFER_INTERACAO 256
#define _SIZEBUFFER_BUFFER 512
#define _SIZEBUFFER_MENSAGEM 512
#define _SIZE_PIPE_GENERATOR 5 //numero de caracteres aleatorios para o pipe com nome de resposta
#define _SIZE_TAM 5
#define _SERVIDOR "./toServidor" //nome do pipe com nome para comunicacao com o servidor

int serverVivo=0;

/**
 * Devolve o numero de virgulas contidas na String
 * @param str String colocada como input.
 */
int numVirgulas(char* str){
    int res=0;
    for(int i=0;i<strlen(str);i++)
        if(str[i]==',')
            res++;
    return res;
}

/**
 * Atribui uma nova funcao ao sinal que recebe como argumento
 * @param sinal é o valor do sinal que sera alterado
 */
void alteraSinais(int sinal){
	char* buffy=(char*)malloc(1);
    switch (sinal) {
        case SIGINT: //atribui ao SIGINT a funcao de perguntar ao utilizador se realmente quer sair
            write(1,"(B121) Pretende sair?[y/N] ",strlen("(B121) Pretende sair?[y/N] "));
            int lidos = read(0,buffy,1);
            if(strcmp(buffy,"y")==0)
                exit(0);
            else write(1,"O programa vai continuar\n",strlen("O programa vai continuar\n"));
                
            break;
        case SIGALRM: //atribui ao SIGALRM a funcao de um timeout
            if(serverVivo==0){
                mensagem_de_erro(E_NOSERVER);
                exit(0);
            }
        default:
            break;
    }
}

/**
 * Troca o caracter \n por por o caracter de fim de String o \0
 * @param str e a String que sera alterada
 * @param length e o tamanho da String
 */
void takeOutN(char* str, int length){
    int sair = 0;
    for(int i=0; i<length && !sair;i++)
        if(str[i]=='\n'){
            str[i]='\0';
            sair=1;
        }
}

/**
 * Devolve uma String aleatoria
 * @param length e o comprimento da String a ser retornada
 */
char *randstring(int length) {
    srand(time(NULL));
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *randomString;
    
    if (length) {
        randomString = malloc(length +1+5);
        
        if (randomString) {
            int l = (int) (strlen(charset) -1);
            int key;
            for (int n = 0;n < length;n++) {
                key = rand() % l;
                randomString[n] = charset[key];
            }
            randomString[length] = '\0';
            strcat(randomString,"_pipe");
        }
    }
    
    return randomString;
}

/**
 * Limpa o espaco de uma String colocando todos os caracteres a \0
 * @param buffer e a String que sera alterada
 * @param length e o tamanho da String
 */
void clearbuffer(char *buffer, int length){
    int i=0;
    while(i<length)
        buffer[i++]='\0';
}


int main(int argc, char* argv[]){
    if(argc==1) { //caso em que nao sao passados parametros e tem uma interacao com o utilizador do tipo menu
        //modificar os sinais SIGINT e SIGALARM para terem novas funcoes
        signal(SIGINT,alteraSinais);
        signal(SIGALRM,alteraSinais);
        
        char *buffer=(char*)malloc(512);
        char *mensagem=(char*)malloc(512);
        char *interacao=(char*)malloc(_SIZEBUFFER_INTERACAO);
        char* aEnviar=NULL;
        
        int fdLeitura;
        
        int opcaoEscolhida=-1;
        int nLidos=0, lidosInteracao=0;
        char *tam = (char*) malloc(_SIZE_TAM);
        
        //para o caso de incremento
        char *localizacao=NULL;
        int incremento=-1;
        
        //para o caso de agregacao
        char *path=NULL;
        int nivel=-1;
        int i=0;
        
        write(1," ______   __    _______  __ \n",strlen(" ______   __    _______  __ \n"));
        write(1,"(  ___ \\ /  \\  / ___   )/  \\\n",strlen("(  ___ \\ /  \\  / ___   )/  \\\n"));
        write(1,"| (   ) )\\/) ) \\/   )  |\\/) )\n",strlen("| (   ) )\\/) ) \\/   )  |\\/) )\n"));
        write(1,"| (__/ /   | |     /   )  | |\n",strlen("| (__/ /   | |     /   )  | |\n"));
        write(1,"|  __ (    | |   _/   /   | |\n",strlen("|  __ (    | |   _/   /   | |\n"));
        write(1,"| (  \\ \\   | |  /   _/    | |\n",strlen("| (  \\ \\   | |  /   _/    | |\n"));
        write(1,"| )___) )__) (_(   (__/\\__) (_\n",strlen("| )___) )__) (_(   (__/\\__) (_\n"));
        write(1,"(______/ \\____/\\_______/\\____/\n",strlen("(______/ \\____/\\_______/\\____/\n"));
        write(1,"\t\t BananaCorp®\n",strlen("\t\t BananaCorp®\n"));
        write(1,"\n",1);
        //menu
        write(1,"Opções\n",sizeof("Opções\n"));
        write(1,"  1 - Incrementar\n",strlen("  1 - Incrementar\n"));
        write(1,"  2 - Agrupar\n",strlen("  2 - Agrupar\n"));
        write(1,"> ",strlen("> "));
        lidosInteracao = read(0, interacao, _SIZEBUFFER_INTERACAO);
        opcaoEscolhida=atoi(interacao);
        switch (opcaoEscolhida) { // para saber qual a opcao selecionada
            case 1:
                opcaoEscolhida = _INCREMENTAR;
                break;
            case 2:
                opcaoEscolhida = _AGREGAR;
                break;
            default:
                mensagem_de_erro(E_OPESCOLHIDA);
                break;
        }
        
        switch (opcaoEscolhida) { //interacção para obter os campos necessarios
            case _INCREMENTAR:
                write(0, "--Incrementar--\n", strlen("--Incrementar--\n"));
                write(0, "Indique a localização separada por virgulas\n", strlen("Indique a localização separada por virgulas\n"));
                write(0, "Localização: ", strlen("Localização: "));
                lidosInteracao=read(1, interacao, _SIZEBUFFER_INTERACAO);
                takeOutN(interacao,_SIZEBUFFER_INTERACAO);
                if (numVirgulas(interacao)>=3) mensagem_de_erro(E_DETALHE);
                localizacao = strdup(interacao);
                if(_DEBUG) clearbuffer(interacao, _SIZEBUFFER_INTERACAO);
                write(0, "Incremento: ", strlen("Incremento: "));
                lidosInteracao=read(1, interacao, _SIZEBUFFER_INTERACAO);
                incremento = atoi(interacao);
                if(incremento<=0) mensagem_de_erro(E_INCREMENTO);
                if(_DEBUG) clearbuffer(interacao, _SIZEBUFFER_INTERACAO);
                break;
                
                
            case _AGREGAR:
                write(0, "--Agregar--\n", strlen("--Agregar--\n"));
                write(0,"Indique a localização base\n",strlen("Indique a localização base\n"));
                write(0, "Localização: ", strlen("Localização: "));
                lidosInteracao=read(1, interacao, _SIZEBUFFER_INTERACAO);
                takeOutN(interacao,_SIZEBUFFER_INTERACAO);
                localizacao = strdup(interacao);
                if(numVirgulas(interacao)>0) mensagem_de_erro(E_LOCALIZACAO);
                if(_DEBUG) clearbuffer(interacao, _SIZEBUFFER_INTERACAO);
                write(0, "Nivel: ", strlen("Nivel: "));
                lidosInteracao=read(1, interacao, _SIZEBUFFER_INTERACAO);
                nivel = atoi(interacao);
                if(nivel>3 || nivel<0) mensagem_de_erro(E_NIVEL);
                if(_DEBUG) clearbuffer(interacao, _SIZEBUFFER_INTERACAO);
                write(0, "Path: ",strlen("Path: "));
                lidosInteracao=read(1, interacao, _SIZEBUFFER_INTERACAO);
                takeOutN(interacao,_SIZEBUFFER_INTERACAO);
                path=strdup(interacao);
                if(_DEBUG) clearbuffer(interacao, _SIZEBUFFER_INTERACAO);
                break;
                
                
            default:
                mensagem_de_erro(E_OPESCOLHIDA);
                return -1; //nunca entra aqui (depois do modulo de erros)
                break;
        }
        
        //    TIPO|LOCALIZACAO|INCREMENTO|PIPE(aleatorio)
        //    TIPO|LOCALIZACAO|NIVEL|PATH|PIPE(aleatorio)
        
        clearbuffer(mensagem, _SIZEBUFFER_MENSAGEM);
        char* pipeName = randstring(_SIZE_PIPE_GENERATOR);
        
        sprintf(tam,"%d",opcaoEscolhida);
        
        strcat(mensagem,tam); // tipo
        strcat(mensagem,"|");
        strcat(mensagem, localizacao); //localizacao
        strcat(mensagem, "|");
        
        clearbuffer(tam, _SIZE_TAM);
        switch (opcaoEscolhida) {
            case _INCREMENTAR:
                sprintf(tam,"%d",incremento); //incremento
                strcat(mensagem, tam);
                break;
            case _AGREGAR:
                sprintf(tam,"%d",nivel); //nivel
                strcat(mensagem, tam);
                strcat(mensagem, "|"); //path
                strcat(mensagem, path);
                break;
            default:
                perror("Erro: Algo correu mal\n");
                break;
        }
        strcat(mensagem, "|");
        strcat(mensagem, pipeName);
        for(i=strlen(mensagem);i<256;i++)
            strcat(mensagem,".");
        
        aEnviar=(char *)malloc(sizeof(char));
        aEnviar=mensagem;
        //strcpy(aEnviar,mensagem);
        
        mkfifo(pipeName, 0777);
        
        int fdEscrita = open(_SERVIDOR,O_WRONLY);
        alarm(3); //timeout para receber a resposta do servidor

        write(fdEscrita,aEnviar,strlen(aEnviar));
        write(1,"(B121) Sent\n",strlen("(B121) Sent\n"));
        int fdfromServidor = open(pipeName,O_RDONLY);

        nLidos=read(fdfromServidor,buffer,256);
        serverVivo=1;
        int resp = atoi(buffer);
        if(resp>=2000)
            mensagem_de_erro(resp); //caso obtenha um valor superior a 2000 corresponde a mensagem de erro
        write(1,buffer,nLidos);
        
        close(fdEscrita);
        close(fdfromServidor);
    }
    else if(strcmp(argv[1],"-blank")==0){ //sao passados argumentos e nao tem formato meu
        char* buffer=(char*)malloc(256);
        char* mensagem=(char*)malloc(256);
        char* tmp=(char*)malloc(256);
        char* locais, *nOi, *ficheiro;
        int tamMsg=-1;
        int i,j;
        int tipo=-1;
        
        int fdEscrita = open(_SERVIDOR,O_WRONLY);

        while((tamMsg=read(0,buffer,256))>0){
            clearbuffer(mensagem,256);
            buffer[tamMsg-1]='\0';
            
            //inc loc1,loc2,loc3 inc
            //agr loc1 nivel ficheiro
            char* tmp=(char*)malloc(256);
            for(i=0,j=0;i<tamMsg && buffer[i]!=' ';i++,j++){
                tmp[j]=buffer[i]; //para saber qual o tipo
            }
            i++;j++;
            tmp[j]='\0';

            if(strcmp(tmp,"inc")==0){
                tipo=_INCREMENTAR;
            } else if(strcmp(tmp,"agr")==0)
                tipo=_AGREGAR;
            
            clearbuffer(tmp,256);
            for(j=0;i<tamMsg && buffer[i]!=' ';i++,j++){
                tmp[j]=buffer[i]; //para saber a localizacao
            }
            i++;j++;
            tmp[j]='\0';
            
            locais=strdup(tmp);

            clearbuffer(tmp,256);
            for(j=0;i<tamMsg && buffer[i]!=' ' && buffer[i]!='\n';i++,j++){
                tmp[j]=buffer[i]; //para saber qual o incremento ou o nivel na agregacao
            }
            if(tipo==_AGREGAR)
                i++;
            j++;
            tmp[j]='\0';
            if(strcmp(tmp,"")==0) nOi=strdup("0");
                else
                    nOi=strdup(tmp);
            
            
            if(tipo==_AGREGAR){
                clearbuffer(tmp,256);
                for(j=0;i<tamMsg && buffer[i]!=' ';i++,j++){
                    tmp[j]=buffer[i]; //para saber qual o ficheiro onde vai guardar a agregacao
                }
                j++;
                tmp[j]='\0';
                ficheiro=strdup(tmp);
            }
            
            //TIPO|LOCAIS|NOI|PIPE
            //TIPO|LOCAIS|NOI|FICHEIRO|PIPE
            
            strcat(mensagem,tipo==_INCREMENTAR?"1000":"1001");
            strcat(mensagem,"|");
            strcat(mensagem,locais);
            strcat(mensagem,"|");
            strcat(mensagem,nOi);
            strcat(mensagem,"|");
            if(tipo==_AGREGAR){
                strcat(mensagem,ficheiro);
                strcat(mensagem,"|");
            }
            strcat(mensagem,"null");
        
            for(i=strlen(mensagem);i<256;i++)
                strcat(mensagem,".");
            
            write(fdEscrita,mensagem,strlen(mensagem));
            clearbuffer(buffer,256);
            clearbuffer(tmp,256);
        }
        if(fdEscrita > 1)
            close(fdEscrita);
    }
    
	return 0;
}
