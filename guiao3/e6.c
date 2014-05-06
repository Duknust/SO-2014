#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

//implemente um COMANDO

int main(int argc, char** argv){
	char **argumentos=(char**)malloc(sizeof(char*)*(argc+1-1));
	int i=0;
	for(i=1; i<argc; i++){
        argumentos[i-1]=argv[i];
    }

	return systemMeu(argc,argumentos);
}

int systemMeu(int argc, char** argumentos){
	int res;
	int status=0;
	pid_t filho=0;
	
	filho = fork();
	if(filho==0){ //é o filho
		execvp(argumentos[0], argumentos);
		//_exit(res)
	}
	else { //é o pai
		waitpid(filho,&status,0);	
	}
	return 1;
}