#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	const char* toServidor="/Users/duarteduarte/Desktop/SO/pipes_semNome/toServidor";
	const char* fromServidor="/Users/duarteduarte/Desktop/SO/pipes_semNome/fromServidor";
	const char* logi="/Users/duarteduarte/Desktop/SO/pipes_semNome/log.txt";
	time_t tempoServidor;

	mkfifo(toServidor, 0777);
	mkfifo(fromServidor, 0777);

	printf("abre ou nao!?!\n");
	int fdLeitura = open(toServidor,O_RDONLY);
	printf("open 1!!\n");
	int fdEscrita = open(fromServidor,O_WRONLY);
	printf("open 2!!\n");
	int fdLogi = open(logi,O_CREAT | O_WRONLY, 0666);
	printf("open logi!!\n");

	int nLidos=0;
	int desliga=0;
	char buffer[512];
	char* controlo;
	printf("estou a espera do cliente!\n");
	while((nLidos=read(fdLeitura,buffer,512))>0 && !desliga){
		printf("ja li!\n");
		tempoServidor = time(NULL);
		controlo=(char*)malloc(nLidos+1);
		strncpy(controlo,buffer,nLidos+1);
		printf("contr_ '%s'\n",controlo);
		if(strcmp(controlo,"shutdownServer\n")==0)
			desliga=1;
		else{
			printf("'%s'\n",buffer);
			write(fdEscrita,ctime(&tempoServidor),strlen(ctime(&tempoServidor)));
			//write(fdEscrita,buffer,nLidos);
			write(fdLogi,buffer,nLidos);
		}
	}
	close(fdLeitura);
	close(fdEscrita);
	close(fdLogi);
	return 0;
}
