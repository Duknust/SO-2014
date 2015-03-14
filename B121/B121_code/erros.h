//
//  erros.h
//  bananaFluServer
//
//  Created by Duarte Nuno Ferreira Duarte on 19/05/14.
//  Copyright (c) 2014 Duarte Nuno Ferreira Duarte. All rights reserved.
//

#ifndef bananaFlueServer_erros_h
#define bananaFlueServer_erros_h

#define E_NOSERVER			 2000 /**    Servidor inactivo ou desligado    **/
#define E_OPESCOLHIDA		 2001 /**            Opcao invalida            **/
#define E_DETALHE			 2002 /**   Demasiado detalhe na localizacao   **/
#define E_INCREMENTO	     2003 /**          Incremento invalido         **/
#define E_NIVEL			     2004 /**             Nivel invalido           **/
#define E_LOCALIZACAO		 2005 /**        Localizacao impossivel        **/
#define E_AGRIMPOSSIVEL      2006 /**         Agregacao impossivel         **/
#define E_FICHEIRO           2007 /**    Impossivel utilizar o ficheiro    **/
#define E_CAPACIDADE         2007 /**      Excedeu a capacidade total      **/

int mensagem_de_erro(int erro_num);

#endif
