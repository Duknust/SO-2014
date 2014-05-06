#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(){
	const char* toServidor="/Users/duarteduarte/Desktop/SO/pipes_semNome/toServidor";
	const char* fromServidor="/Users/duarteduarte/Desktop/SO/pipes_semNome/fromServidor";
	
	char buffer[512];
	//char envia[10]="fixe!";
	printf("abre ou nao!?!\n");
	int fdEscrita = open(toServidor,O_WRONLY);
	printf("open 1!!\n");
	int fdLeitura = open(fromServidor,O_RDONLY);
	printf("open 2!!\n");
	int nLidos=0;

	//write(0,envia,10);
	printf("devia ler aqui!!\n");
	while((nLidos=read(0,buffer,512))>0){
		write(fdEscrita,buffer,nLidos);
		printf("estou aqui!\n");
		nLidos=read(fdLeitura,buffer,512);
		write(1,buffer,nLidos);
	}

	return 0;
}