//
//  servidorDiv.c
//  bananaFluServer
//
//  Created by Duarte Nuno Ferreira Duarte on 28/05/14.
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

#include "erros.h"
#include "servidorDiv.h"


int pipeReg[2];
int pidFilhoRegisto=0;
int status_filhoReg=-1;

int fdReg=-1;

int distritosAreiniciar=0; //numero de distritos que se encontram a reinciar

/**
 * Coloca na queue de um determinado distrito a mensagem que lhe vai ser entrega mais tarde
 * @param mensagem e a String que sera colocada na queue de mensagens em espera
 * @param distrito e o distrito a qual pertence a mensagem a ser entregue
 */
void enqueue(char* mensagem, DistritoFluRecord *distrito){
    Pendente *tmp = (Pendente*)malloc(sizeof(Pendente));
    tmp->mensagem=strdup(mensagem);
    tmp->next=distrito->pendentes;
    distrito->pendentes=tmp;
}

/**
 * Retira da queue de um determinado distrito a mensagem que lhe devia ter sido entregue
 * @param distrito e o distrito a qual pertence a mensagem a ser entregue
 */
char* dequeue(DistritoFluRecord *distrito){
    Pendente *tmp=NULL;
    char* res=NULL;
    if(distrito->pendentes!=NULL){
        tmp=distrito->pendentes;
        distrito->pendentes=tmp->next;
        res=strdup(tmp->mensagem);
        //free(tmp);
    }
    return res;
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

/**
 * Devolve (caso exista) o distrito com o nome associado
 * @param nome e o nome do distrito a ser encontrado
 */
DistritoFluRecord *getByNameDist(char* nome){
    int i;
    for(i=0; i<MAX_DISTRITOS;i++){
        if((distritos[i] != NULL) && (strcmp(distritos[i]->nome, nome)==0))
            return distritos[i];
    }
    return NULL;
}

/**
 * Devolve (caso exista) o concelho com o nome associado
 * @param nome e o nome do concelho a ser encontrado
 * @param distrito e a estrutura do distrito onde deve ser encontrado o concelho
 */
ConcelhoFluRecord *getByNameConc(char* nome, DistritoFluRecord *distrito){
    ConcelhoFluRecord *res=NULL;
    for(res=distrito->concelhos;res!=NULL;res=res->next)
        if(strcmp(res->nome, nome)==0)
            return res;
    return NULL;
}

/**
 * Devolve (caso exista) a freguesia com o nome associado
 * @param nome e o nome do freguesia a ser encontrada
 * @param concelho e a estrutura do concelho onde deve ser encontrada a freguesia
 */
FreguesiaFluRecord *getByNameFreg(char* nome, ConcelhoFluRecord *concelho){
    FreguesiaFluRecord *res=NULL;
    for(res=concelho->freguesias;res!=NULL;res=res->next)
        if(strcmp(res->nome, nome)==0)
            return res;
    return NULL;
}

/**
 * Cria (caso ainda nao esteja cheio o array de distritos) um distrito com o nome inserido
 * @param nome e o nome do distrito a ser criado
 */
DistritoFluRecord *criarDistrito(char* nome){
    DistritoFluRecord *tmp = (DistritoFluRecord*)malloc(sizeof(DistritoFluRecord));
    tmp->nome=strdup(nome);
    tmp->aggregate=0;
    tmp->pipe[0]=-1;
    tmp->pipe[1]=-1;
    tmp->pidFilho=0;
    tmp->estado=ALIVE;
    tmp->concelhos=NULL;
    tmp->pendentes=NULL;
    
    distritos[numeroDistritos] = tmp;
    numeroDistritos++;
    
    return tmp;
}

/**
 * Cria um concelho com o nome inserido e insere-o no distrito dado como argumento
 * @param nome e o nome do concelho a ser criado
 * @param distrito e a estrutura do distrito onde deve ser colocado o concelho
 */
ConcelhoFluRecord *criarConcelho(char* nome, DistritoFluRecord *distrito){
    ConcelhoFluRecord *tmp = (ConcelhoFluRecord*)malloc(sizeof(ConcelhoFluRecord));
    tmp->nome=strdup(nome);
    tmp->aggregate=0;
    tmp->freguesias=NULL;
    tmp->next=distrito->concelhos;
    distrito->concelhos=tmp;
    
    return tmp;
}

/**
 * Cria uma freguesia com o nome inserido e insere-a no concelho dado como argumento
 * @param nome e o nome da freguesia a ser criada
 * @param concelho e a estrutura do concelho onde deve ser colocada a freguesia
 */
FreguesiaFluRecord *criarFreguesia(char* nome, ConcelhoFluRecord *concelho){
    FreguesiaFluRecord *tmp = (FreguesiaFluRecord*)malloc(sizeof(FreguesiaFluRecord));
    tmp->nome=strdup(nome);
    tmp->aggregate=0;
    tmp->next=concelho->freguesias;
    concelho->freguesias=tmp;
    
    return tmp;
}

/**
 * Inicia o array de distritos com todas as posicoes do array a NULL
 */
void initDistritos(){
    //printf("clearing\n");
    for(int i=0;i<MAX_DISTRITOS;i++)
        distritos[i]=NULL;
}

/**
 * Atribui uma nova funcao ao sinal que recebe como argumento
 * @param sinal é o valor do sinal que sera alterado
 */
void alteraSinais(int sinal){
    int i=0, pidFilho=0, status=0;
    int para=0, posicao=-1;
    char* tmp=(char*)malloc(256);
    char* queueMsg=NULL;
    int posFilhoMorreu = -1;
    
    clearbuffer(tmp, 256);
    switch (sinal) {
        case SIGCHLD: //o SIGCHLD passa a ser responsavel por reiniciar o processo que morreu
            if((pidFilho=waitpid(-1, &status, WNOHANG)) != -1){ //descobrir o pid do filho que morreu
                for(i=0;i<numeroDistritos && !para;i++)
                    if(kill(distritos[i]->pidFilho,0)==-1){
                        posFilhoMorreu=i;
                        para=1;
                    }
                
                distritosAreiniciar+=1; //incrementar o numero de processos a reiniciar
                distritos[posFilhoMorreu]->estado=DEAD; //alterar o estado do processo que enviou o sinal
                pidFilho=fork();
                if(pidFilho==0){
                    workerFilho(distritos[posFilhoMorreu]); //iniciar o processo que tera a mesma funcao que o que morreu
                }
                else{
                    distritos[posFilhoMorreu]->pidFilho=pidFilho; //actualizar a informacao do pid do processo
                    strcat(tmp, "(B121) O filho de ");
                    strcat(tmp, distritos[posFilhoMorreu]->nome);
                    strcat(tmp, " esta a ser reiniciado\n");
                    write(1,tmp,strlen(tmp));
                }
            }
            break;
        case SIGUSR1: //o SIGUSR1 e o sinal enviado pelo filho que foi iniciado ou reiniciado para o pai avisar que esta activo
            for(i=0;i<numeroDistritos && distritosAreiniciar>0;i++){ //so entra caso nao seja um processo a iniciar
                if(distritos[i]->estado==DEAD){
                    distritosAreiniciar-=1;
                    distritos[i]->estado=ALIVE; //actualizar o estado do processo
                    while((queueMsg=dequeue(distritos[i]))!=NULL)
                        write(distritos[i]->pipe[1],queueMsg,strlen(queueMsg));
                    strcat(tmp, "(B121) O filho de ");
                    strcat(tmp, distritos[i]->nome);
                    strcat(tmp, " esta novamente activo\n");
                    write(1,tmp,strlen(tmp));
                }
            }
            break;
        default:
            break;
    }
}

/**
 * Funcao que faz a divisao de uma mensagem por um dado separador e insere os tres primeiros numa estrutura
 * @param mensagem e a String que vai sofrer a divisao
 * @param sep e o token onde a String sera partida
 */
Localidades *split(char* mensagem, char* sep){
    Localidades *res = (Localidades*)malloc(sizeof(Localidades));
    res->localidades[0]=res->localidades[1]=res->localidades[2]=NULL;
    int i=0;
    
    char *p = strtok(mensagem, sep);
    while(p != NULL) {
        res->localidades[i++]=p;
        p = strtok(NULL, sep);
    }
    
    return res;
}

/**
 * Incrementa as variavel associadas a localizacao passada como argumento
 * @param localidade e a localizacao a ser incrementada
 * @param numCasos e o valor do incremento a ser efectuado
 */
int incrementar(char* localidade[], unsigned numCasos){
    DistritoFluRecord *distrito = getByNameDist(localidade[0]);
    ConcelhoFluRecord *concelho = NULL;
    FreguesiaFluRecord *freguesia = NULL;
    
    if(distrito==NULL)
        distrito = criarDistrito(localidade[0]);
    distrito->aggregate+=numCasos;
    if(localidade[1]!=NULL){
        concelho = getByNameConc(localidade[1], distrito);
        if(concelho==NULL)
            concelho = criarConcelho(localidade[1], distrito);
        concelho->aggregate+=numCasos;
        if(localidade[2]!=NULL){
            freguesia = getByNameFreg(localidade[2], concelho);
            if(freguesia==NULL)
                freguesia = criarFreguesia(localidade[2], concelho);
            freguesia->aggregate+=numCasos;
        }
    }
    
    return 0;
}

/**
 * Devolve o numero de digitos que ocupa o inteiro passado como argumento
 * @param n e o inteiro a ser avaliado
 */
unsigned numDigits(const unsigned n) {
    if (n < 10) return 1;
    return 1 + numDigits(n / 10);
}

/**
 * Agrega todos os valores a partir de uma localizacao num ficheiro
 * @param prefixo e a localizacao a partir do qual vao ser agregados os valores
 * @param nivel e o nivel de detalhe da agregacao
 * @param path e o nome do ficheiro onde deve ser colocada essa agregacao
 */
int agregar(char* prefixo[], int nivel, char* path){ // TEM DE BLOQUEAR ENQUANTO ESCREVE
    
    unlink(path);
    
    DistritoFluRecord *distrito = getByNameDist(prefixo[0]);
    ConcelhoFluRecord *tmpC = NULL;
    FreguesiaFluRecord *tmpF = NULL;
    char *tamanho=(char*)malloc(10);
    if(distrito == NULL){
        return E_AGRIMPOSSIVEL;} //retorna que a agregacao é impossivel
    int fdEscrita = open(path, O_CREAT| O_TRUNC | O_WRONLY, 0666);
    if(fdEscrita<0) return E_FICHEIRO;//mensagem_de_erro(E_FICHEIRO);
    switch (nivel) {
        case 0:
            write(fdEscrita, distrito->nome,strlen(distrito->nome));
            write(fdEscrita, ":", 1);
            clearbuffer(tamanho, 10);
            sprintf(tamanho,"%d",(distrito->aggregate));
            write(fdEscrita, tamanho, sizeof(tamanho)-1);
            write(fdEscrita, "\n", 1);
            break;
        case 1:
            for(tmpC = distrito->concelhos; tmpC!=NULL; tmpC=tmpC->next){
                write(fdEscrita, distrito->nome,strlen(distrito->nome));
                write(fdEscrita, ":", 1);
                write(fdEscrita, tmpC->nome,strlen(tmpC->nome));
                write(fdEscrita, ":", 1);
                clearbuffer(tamanho, 10);
                sprintf(tamanho,"%d",(tmpC->aggregate));
                write(fdEscrita, tamanho, numDigits(tmpC->aggregate));
                write(fdEscrita, "\n", 1);
            }
            break;
        case 2:
            for(tmpC = distrito->concelhos; tmpC!=NULL; tmpC=tmpC->next)
                for(tmpF = tmpC->freguesias; tmpF!=NULL; tmpF=tmpF->next){
                    write(fdEscrita, distrito->nome,strlen(distrito->nome));
                    write(fdEscrita, ":", 1);
                    write(fdEscrita, tmpC->nome,strlen(tmpC->nome));
                    write(fdEscrita, ":", 1);
                    write(fdEscrita, tmpF->nome, strlen(tmpF->nome));
                    write(fdEscrita, ":", 1);
                    clearbuffer(tamanho, 10);
                    sprintf(tamanho,"%d",(tmpF->aggregate));
                    write(fdEscrita, tamanho, numDigits(tmpF->aggregate));
                    write(fdEscrita, "\n", 1);
                }
        default:
            break;
    }
    if(fdEscrita>=0)
        close(fdEscrita);
    
    return 0;
}

/**
 * Funcao que faz parsing da mensagem
 * @param mensagem e a String que vai sofrer o parsing
 * @param localidade e uma passagem por referencia que retornara a localidade na mensagem
 * @param path e uma passagem por referencia que retornara o path na mensagem
 * @param nOi e uma passagem por referencia que retornara o nivel ou incremento na mensagem
 * @param pipeResposta e uma passagem por referencia que retornara o pipe na mensagem
 */
int parsingMensagem(char* mensagem, char **localidade, char **path, int *nOi, char **pipeResposta){
    int i=0, j=0, para=0;
    int tipo=-1, nivelOrIncr=-1;
    char *tmp=(char*)malloc(256);
    char *temp;
    
    //    TIPO|LOCALIZACAO|INCREMENTO|PIPE(aleatorio)
    //    TIPO|LOCALIZACAO|NIVEL|PATH|PIPE(aleatorio)
    
    for(i=0,j=0; i<strlen(mensagem) && !para; i++,j++){
        if(mensagem[i] != '|')
            tmp[j]=mensagem[i];
        else para=1;
    }
    tmp[j]='\0';
    tipo = atoi(tmp); //tipo
    clearbuffer(tmp, 256);
    
    for(j=0,para=0; i<strlen(mensagem) && !para; i++,j++){
        if(mensagem[i] != '|')
            tmp[j]=mensagem[i];
        else para=1;
    }
    tmp[j]='\0';
    temp=strdup(tmp); //localizacao
    (*localidade)=temp;
    clearbuffer(tmp, 256);
    
    for(j=0,para=0; i<strlen(mensagem) && !para; i++,j++){
        if(mensagem[i] != '|')
            tmp[j]=mensagem[i];
        else para=1;
    }
    tmp[j]='\0';
    
    nivelOrIncr = atoi(tmp);
    (*nOi)=nivelOrIncr;
    clearbuffer(tmp, 256);
    
    if(tipo==_AGREGAR){
        para=0;
        for(j=0; i<strlen(mensagem) && !para; i++,j++){
            if(mensagem[i] != '|')
                tmp[j]=mensagem[i];
            else para=1;
        }
        tmp[j]='\0';
        temp=strdup(tmp); //path
        (*path)=temp;
        clearbuffer(tmp, 256);
    }
    
    for(j=0,para=0; i<strlen(mensagem) && !para; i++,j++){
        if(mensagem[i] != '|' && mensagem[i] != '.')
            tmp[j]=mensagem[i];
        else para=1;
    }
    tmp[j]='\0';
    temp=strdup(tmp); //pipe de resposta
    (*pipeResposta)=temp;
    free(tmp);
    return tipo;
}

/**
 * Funcao que copia uma String para uma String resultado ate encontrar um determinado caracter
 * @param mensagem e a String que vai sofrer o parsing
 * @param delimitador e o caracter que sera o terminador da String
 * @param maxSize e o tamanho maximo a ser copiado
 * @param mensagemARetornar e a String que sera retornada
 */
int copiaStringAte(char* mensagem, char delimitador, int maxSize, char** mensagemARetornar){
    char* tmp=(char*)malloc(maxSize);
    int i=0;
    for(i=0;i<strlen(mensagem) && mensagem[i]!=delimitador;i++);
    
    tmp=strncpy(tmp, mensagem, i);
    *mensagemARetornar = strdup(tmp);
    
    return i+1;
}

/**
 * Funcao que le de um ficheiro toda a informacao de um determinado distrito
 * @param ficheiroEscrita e o nome do ficheiro que contem a informacao
 */
int reloadFilho(char* ficheiroEscrita){
    int fdRead = open(ficheiroEscrita,O_RDONLY);
    if(fdRead>0){
        int tamLido = -1;
        char *buffer=(char*)malloc(256);
        char *lixo=(char*)malloc(5);
        char *tmp=(char*)malloc(512);
        char *localidade=(char*)malloc(1);
        char *path=(char*)malloc(1);
        char *pipeResposta=(char*)malloc(1);

        int nOi=-1;
        int tipo=-1;
        int numeroRetorno=0;
        
        Localidades *localidades;
        
        
        while((tamLido = read(fdRead, buffer, 256))>0){
            tipo = parsingMensagem(buffer, &localidade, &path, &nOi, &pipeResposta);
            
            if(tipo==_INCREMENTAR){ //so sao necessarias as mensagens que continham informacao relativa a incrementos
                localidades = split(localidade, ",");
                incrementar(localidades->localidades, nOi);
            }
            read(fdRead, lixo, 1);
        }
    }
    return 0;
}


int reloadLocalization(char* file){
    int fdRead = open(file,O_RDONLY);
    int getmore=0;
    char *buffer=(char*)malloc(256);
    char *tmp=(char*)malloc(512);
    char *localidade=(char*)malloc(1);
    char *path=(char*)malloc(1);
    char *pipeResposta=(char*)malloc(1);
    
    int nOi=-1;
    int tamLido=-1;
    int tipo=-1;
    int numeroRetorno=0;
    char* toParse=(char*)malloc(1);
    char *posnl;
    
    Localidades *localidades;
    
    
    clearbuffer(buffer, 256);
    while((tamLido = read(fdRead, buffer, 256))>0){
        getmore=0;
        
        strcat(tmp,buffer);
        
        while(numeroRetorno<tamLido && !getmore){
            //printf("--nr: %d\n'%s'--\n",numeroRetorno,tmp);
            if((posnl = strstr(tmp,"\n"))!=NULL){
                numeroRetorno+=copiaStringAte(tmp,'\n',512,&toParse);
                tmp = (posnl+1);
                
                tipo = parsingMensagem(toParse, &localidade, &path, &nOi, &pipeResposta);
                
                if(tipo==_INCREMENTAR){
                    localidades = split(localidade, ",");
                    incrementar(localidades->localidades, nOi);
                }
            }
            else getmore=1;
            clearbuffer(toParse, 256);
        }
        clearbuffer(buffer,256);
    }
    
    return 0;
}

/**
 * Funcao que aglomera todo o trabalho de um processo responsavel por um distrito
 * @param distrito e a estrutura com toda as bases de um distrito
 */
int workerFilho(DistritoFluRecord *distrito){
    char* msgBuffer = (char*)malloc(256),
    *ficheiroEscrita = (char*)malloc(128),
    *localidade, *path, *pipeResposta;
    int nOi=0;
    int tamMsg=-1;
    int fdReg=-1;
    int tipo=-1;
    int fdResp=-1;
    int resp=-1;
    
    Localidades *localidades;
    
    clearbuffer(ficheiroEscrita,128);
    strcat(ficheiroEscrita, distrito->nome);
    strcat(ficheiroEscrita, ".txt");
    
    reloadFilho(ficheiroEscrita); //reler toda a informacao que ja existia no servidor relativamente a este distrito
    kill(getppid(),SIGUSR1); //enviar ao processo pai um sinal para que ele saiba que este processo esta preparado para receber qualquer mensagem
    
    close(distrito->pipe[1]);
    clearbuffer(msgBuffer, 256);
    while((tamMsg=read(distrito->pipe[0],msgBuffer,256))>0){
        if(fdReg<0){
            fdReg = open(ficheiroEscrita, O_CREAT | O_APPEND | O_WRONLY, 0666);
        }
        write(fdReg, msgBuffer, tamMsg);
        write(fdReg, "\n", 1);

        localidade=(char*)malloc(sizeof(char));
        path=(char*)malloc(sizeof(char));
        pipeResposta=(char*)malloc(sizeof(char));

        tipo = parsingMensagem(msgBuffer, &localidade, &path, &nOi, &pipeResposta);
        localidades = split(localidade,",");
        switch (tipo) {
            case _INCREMENTAR:
                incrementar(localidades->localidades, nOi);
                fdResp = open(pipeResposta,O_WRONLY);
                time_t tempoServidor = time(NULL);
                write(fdResp,ctime(&tempoServidor),strlen(ctime(&tempoServidor)));
                break;
                
            case _AGREGAR:
                    resp = agregar(localidades->localidades, nOi, path);
                    if (resp>=2000 && !(strcmp(pipeResposta, "null")==0)){ //significa que houve um erro
                        fdResp = open(pipeResposta,O_WRONLY);
                        char *tamanho=(char*)malloc(10);
                        clearbuffer(tamanho, 10);
                        sprintf(tamanho,"%d",resp);
                        write(fdResp,tamanho,strlen(tamanho));
                        free(tamanho);
                    } else if (!(strcmp(pipeResposta, "null")==0)){
                        int fdResp = open(pipeResposta,O_WRONLY);
                        time_t tempoServidor = time(NULL);
                        write(fdResp,ctime(&tempoServidor),strlen(ctime(&tempoServidor)));
                    }
                break;
            default:
                break;
        }
        if(fdResp>0)
            close(fdResp);
        clearbuffer(msgBuffer, 256);
    }
    
    return 0;
}

/**
 * Funcao que faz que regista em ficheiro uma String
 * @param regBuffer e a String que vai ser guardada em ficheiro
 * @param tamRegMsg e o comprimento da mensagem
 */
void logToFile(char* regBuffer, long tamRegMsg){
    if(fdReg<0){
        fdReg = open(REG_FILE, O_CREAT | O_APPEND | O_WRONLY, 0666);
    }
    write(fdReg, regBuffer, tamRegMsg);
    write(fdReg, "\n", 1);
}

/**
 * Funcao que trata de ler de um pipe uma mensagem e registar num ficheiro
 */
int spawnRegWriter(){
    char *regBuffer=(char*)malloc(256);
    int tamRegMsg=0;
    int pid_filhoRegisto = fork();
    
    if(pid_filhoRegisto==0){
        while((tamRegMsg=read(pipeReg[0],regBuffer,256))>0)
            logToFile(regBuffer,tamRegMsg);
    }
    
    return pid_filhoRegisto;
}



int main(int argc, const char * argv[])
{
    //DistritoFluRecord *distritos[MAX_DISTRITOS];
    const char* toServidor="./toServidor";
    
    signal(SIGCHLD, alteraSinais); //atribuir nova funcao ao SIGCHLD
    signal(SIGUSR1, alteraSinais); //atribuir nova funcao ao SIGUSR1
    
    initDistritos(); //inicializacao da estrutura que contera os distritos

    char *msgBuffer=(char*)malloc(256);
    
    int serv2Reg = pipe(pipeReg); //criar um pipe que servira de comunicacao com o processo que regista todas as mensagens
    long tamMsg=0;
    int fdListen=-1, fdResposta=-1;
    
    char* tamanho=(char*)malloc(10);
    
    unlink(toServidor); //para remover o pipe toServidor
    
    if(mkfifo(toServidor, 0777) == 0) //criar um pipe com nome para receber mensagens dos clientes
        write(1,"(B121) Server started\n",strlen("(B121) Server started\n"));
    
    char* localidade=NULL, *path=NULL, *pipeResposta=NULL;
    int nOi, tipo;
    Localidades *localidades;
    fdListen = open(toServidor,O_RDONLY); //abrir o pipe com nome que sera responsavel pela comunicacao cliente-servidor
    
    //inicia o filho que trata do registo de todas as mensagens recebidas
    pidFilhoRegisto = spawnRegWriter();
    
    write(1, "(B121) Waiting for connection\n", strlen("(B121) Waiting for connection\n"));
    while(1){
        clearbuffer(msgBuffer, 256);
        fdListen = open(toServidor,O_RDONLY);
        while((tamMsg=read(fdListen,msgBuffer,256))>0){
            write(1,"(B121) Message received\n",strlen("(B121) Message received\n"));
            write(pipeReg[1], msgBuffer, tamMsg); //enviar mensagem recebida para o pipe responsavel pelo registo
            localidade=(char*)malloc(sizeof(char));
            path=(char*)malloc(sizeof(char));
            pipeResposta=(char*)malloc(sizeof(char));
            tipo = parsingMensagem(msgBuffer, &localidade, &path, &nOi, &pipeResposta);
            if(numeroDistritos>=MAX_DISTRITOS && !(strcmp(pipeResposta, "null")==0)){ //caso nao possa ser adicionado novo distrito
                fdResposta = open(pipeResposta,O_WRONLY);
                clearbuffer(tamanho, 10);
                sprintf(tamanho,"%d",E_CAPACIDADE);
                write(fdResposta,tamanho,strlen(tamanho));
                close(fdResposta);
            } else {
                localidades = split(localidade,",");
                DistritoFluRecord *worker = getByNameDist(localidades->localidades[0]);
                if(worker!=NULL){ //se o processo relativo a este distrito existe...
                    if(worker->estado==ALIVE){
                        write(worker->pipe[1],msgBuffer,tamMsg); //se o processo esta vivo envia lhe a mensagem
                    }
                    else
                        enqueue(msgBuffer, worker); //se o processo esta a reiniciar guarda a mensagem na queue
                }
                else { //se o prococesso relativo a este distrito nao existe...
                    DistritoFluRecord *novo = criarDistrito(localidades->localidades[0]);
                    pipe(novo->pipe);
                    int pid_novoFilho=fork();
                    if(pid_novoFilho==0){ //atribuir o distrito a um novo processo
                        workerFilho(novo);
                    }
                    else{ //registar a informacao do novo processo
                        novo->pidFilho=pid_novoFilho;
                        write(novo->pipe[1],msgBuffer,strlen(msgBuffer));
                    }
                }
            }
        }
    }
    
    write(1,"(B121) Server will shutdown\n",strlen("(B121) Server will shutdown\n"));
    return 0;
}