#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	const char* toServidor="./toServidor";
	char* fromServidor=NULL;
	
	char buffer[512], mensagem[512];
	char* tmp=(char*)malloc(1024);
	
	int fdLeitura;
	int nLidos=0;
	char tam[4];

	if(argc<3){
		printf("Erro!\nUsar: ./cliente [nome] [mensagem]\n");
		return -1;
	}

	fromServidor=(char*)malloc(sizeof(char)*strlen(argv[1])+5);
	//printf("JA PASSEI AQUI!\n");
	
	strcat(fromServidor,argv[1]);
	strcat(fromServidor,"_pipe");
	//printf("E AQUI!\n");
	
	int tams=strlen(argv[2])+strlen(argv[1]);
	sprintf(tam,"%d",tams);



	strcat(tmp,tam);
	strcat(tmp,"|");
	strcat(tmp,fromServidor);
	//strcpy(tmp,mensagem);
	strcat(tmp,"|");
	//strcpy( tmp,mensagem);
	strcat(tmp,argv[2]);
	//strcpy( tmp,mensagem);
	
	
	//itoa(tams,tam,10);
	strcpy(mensagem,tmp);

	//printf("%s\n",tmp);

	mkfifo(fromServidor, 0777);

	//open(fdEscrita,O_RDONLY);
	//

	int fdEscrita = open(toServidor,O_WRONLY);
	write(fdEscrita,mensagem,strlen(mensagem));
	
	//printf("%s\n",fromServidor);
	int fdfromServidor = open(fromServidor,O_RDONLY);
	while((nLidos=read(fdfromServidor,buffer,512))>0){
		write(1,buffer,nLidos);
	}

	close(fdEscrita);
	return 0;
}