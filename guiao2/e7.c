#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int procuraLinha(int linha[], int tamanho, int agulha){
	int res=0,i;
	for(i=0;i<tamanho;i++){
		if(agulha==linha[i])
			res=1;
	}
	return res;
}

int main(int argc, char* argv[]){
	int agulha=atoi(argv[1]), nfilhos=0;
	int status[10];
	int res;
	int i=0,j=0;
	pid_t filho[10];
	int matriz[10][1000];
	for (i=0;i<10;i++)
		for(j=0;j<1000;j++)
			matriz[i][j]=0;
	matriz[2][100] = 1;
	matriz[3][500] = 1;

	while(nfilhos<10){
		filho[nfilhos]=fork();
		if(filho[nfilhos]==0){
			res=procuraLinha(matriz[nfilhos],1000,agulha);
			_exit(res);
		}
		nfilhos++;
	}

	for(i=0;i<10;i++){
			waitpid(filho[i],&(status[i]),0);		
	}

	i=0;
	while(i<10){
		if(WEXITSTATUS(status[i])==1)
			printf("o filho %d encontrou\n", i);
		i++;
	}
	return 0;
}