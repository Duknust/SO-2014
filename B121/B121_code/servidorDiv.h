//
//  servidor.h
//  bananaFluServer
//
//  Created by Duarte Nuno Ferreira Duarte on 08/05/14.
//  Copyright (c) 2014 Duarte Nuno Ferreira Duarte. All rights reserved.
//

#ifndef bananaFlueServer_servidor_fatherh
#define bananaFlueServer_servidor_fatherh

#define MAX_DISTRITOS 33 //numero maximo de distritos que o servidor pode conter
#define _INCREMENTAR 1000
#define _AGREGAR 1001
#define REG_FILE "registo.txt" //nome do ficheiro que guarda todas as mensagens que o servidor recebe
#define ALIVE 1
#define DEAD 0

//estrutura responsavel por guardar a informacao que ficou pendente
typedef struct sPendente{
    char* mensagem;
    struct sPendente *next;
}Pendente;

//estrutura responsavel pela informacao relativa as freguesias
typedef struct sFreguesiaFluRecord{
    char* nome;
    int aggregate;
    struct sFreguesiaFluRecord *next;
} FreguesiaFluRecord;

//estrutura responsavel pela informacao relativa aos concelhos
typedef struct sConcelhoFluRecord{
    char* nome;
    int aggregate;
    struct sFreguesiaFluRecord *freguesias;
    struct sConcelhoFluRecord *next;
} ConcelhoFluRecord;

//estrutura responsavel pela informacao relativa aos distritos
typedef struct sDistritoFluRecord{
    char* nome;
    int aggregate;
    int pipe[2];
    int pidFilho;
    int estado;
    struct sConcelhoFluRecord *concelhos;
    struct sPendente *pendentes;
} DistritoFluRecord;

//estrutura temporaria para transporte de uma localizacao
typedef struct sLocalidades{
    char* localidades[3];
} Localidades;

//estrutura responsavel por guardar todos os distritos
DistritoFluRecord *distritos[MAX_DISTRITOS];

//numero de distritos que se encontram no servidor
int numeroDistritos=0;


int workerFilho(DistritoFluRecord *distrito);
int spawnRegWriter();
DistritoFluRecord *getByNameDist(char* nome);
ConcelhoFluRecord *getByNameConc(char* nome, DistritoFluRecord *distrito);
FreguesiaFluRecord *getByNameFreg(char* nome, ConcelhoFluRecord *concelho);
DistritoFluRecord *criarDistrito(char* nome);
ConcelhoFluRecord *criarConcelho(char* nome, DistritoFluRecord *distrito);
FreguesiaFluRecord *criarFreguesia(char* nome, ConcelhoFluRecord *concelho);
int incrementar(char* localidade[], unsigned numCasos);
int agregar(char* prefixo[], int nivel, char* path);


#endif
