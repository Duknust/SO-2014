//
//  erros.c
//  bananaFluServer
//
//  Created by Duarte Nuno Ferreira Duarte on 19/05/14.
//  Copyright (c) 2014 Duarte Nuno Ferreira Duarte. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "erros.h"

typedef struct erro
{
    char *err_code;
    char *mensagem;
} ERRO;

static ERRO erro[] =
{
    {"E_NOSERVER",     "(B121) **Erro: O servidor esta ocupado ou inactivo**\n"},
    {"E_OPESCOLHIDA",     "(B121) **Erro: Opcao invalida**\n"},
    {"E_DETALHE",      "(B121) **Erro: Demasiado detalhe na localizacao**\n"},
    {"E_INCREMENTO",      "(B121) **Erro: Incremento invalido**\n"},
    {"E_NIVEL",     "(B121) **Erro: Nivel invalido**\n"},
    {"E_LOCALIZACAO",     "(B121) **Erro: Localizacao impossivel**\n"},
    {"E_AGRIMPOSSIVEL",    "(B121) **Erro: Agregacao impossivel**\n"},
    {"E_FICHEIRO",    "(B121) **Erro: Impossivel utilizar o ficheiro**\n"},
    {"E_CAPACIDADE",    "(B121) **Erro: Impossivel adicionar nova localizacao**\n"}
};

/**
 * Reporta o erro escrevendo uma mensagem no standard error e terminando a execucao
 * @param erro_num O número do erro que vem da tabela de erros.
 */
int mensagem_de_erro(int erro_num)
{
    char* sms = erro[erro_num-2000].mensagem;
    write(2,sms,strlen(sms));
    exit(0);
    
    return erro_num;
}
