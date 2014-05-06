#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

void redireccionar(char* input, char* output){
	int fd;
	if(input!=NULL){
		fd = open(input,O_RDONLY);
		dup2(fd, 0);
		close(fd);
	}
	if(output!=NULL){
		fd = open(output,O_CREAT | O_WRONLY | O_TRUNC, 0666);
		dup2(fd, 1);
		close(fd);
	}
}

int main(int argc, char* argv[]){
	char *argInput=NULL, *argOutput=NULL;
	int conta=0,i,j;
	char ch;
	
	while ((ch = getopt(argc, argv, "-i:-o:")) != -1){
        switch (ch) {
            case 'i':
                argInput=optarg;
                conta++;
                break;
            case 'o':
                argOutput=optarg;
                conta++;
                break;
            default:
                break;
        }
    }
    redireccionar(argInput,argOutput);
    
	// 1+conta*2 é a posiçao no array de argumentos a seguir ao parametro da ultima opcao se existir
	char** paramsComando=(char **) malloc(sizeof(char*)*(argc-(1+conta*2))+1);
	for(j=0,i=1+conta*2;i<argc;i++,j++){
		paramsComando[j]=strdup(argv[i]);
	}
    paramsComando[j]= NULL;
    
	execvp(paramsComando[0],paramsComando);
	return -1;
}