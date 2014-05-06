#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	const char* toServidor="./toServidor";
	const char* logi="./log.txt";
	time_t tempoServidor;

	mkfifo(toServidor, 0777);

	printf("SERVER ACTIVO\n");
	int fdLeitura;
	printf("\n");
	int fdLogi = open(logi,O_CREAT | O_WRONLY, 0666);
	printf("(log.txt pronto)\n");

	int nLidos=0;
	int i=0,j=0,tamanhoMensagem=0;
	int fd_Cpipe=-1;
	char buffer[512], tamMensagemStr[3];
	char pipeResposta[100];
	char* impressao=(char*)malloc(512);
	while(1){
			for(i=0;i<512;i++){
				buffer[i]='\0';
			}
			printf("À espera de conexão...\n");
			fdLeitura = open(toServidor,O_RDONLY);
			nLidos=read(fdLeitura,buffer,512);
			i=0; j=0;
			tamanhoMensagem=0;
			tempoServidor = time(NULL);
			for(i=0; buffer[i]!='\0' && buffer[i]!='|';i++) 
				tamMensagemStr[i]=buffer[i];
			tamanhoMensagem=atoi(tamMensagemStr);
			for(j=0,i=i+1;buffer[i]!='\0' && buffer[i]!='|';i++,j++)
				pipeResposta[j]=buffer[i];
			fd_Cpipe=open(pipeResposta,O_WRONLY);
			write(fd_Cpipe,ctime(&tempoServidor),strlen(ctime(&tempoServidor)));
			//printf("Aqui?!1\n");
			//printf("%s\n",&(buffer[i+1]));
			strcat(impressao, &(pipeResposta));
			strcat(impressao, "\t");
			strcat(impressao, &(buffer[i+1]));
			//printf("%s\n",impressao);
			//printf("Aqui?!2\n");
			write(fdLogi,impressao,tamanhoMensagem);
			write(fdLogi,"\n",1);
			//printf("Aqui?!3\n");
			close(fd_Cpipe);
			close(fdLeitura);

	}
	close(fdLeitura);
	close(fdLogi);
	return 0;
}
