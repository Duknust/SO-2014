#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char** argv){
	int i=0;
	int res;
	char **argumentos=(char**)malloc(sizeof(char*)*(argc+1-1));
	int status=0;
	pid_t filho=0;
	for(i=1; i<argc; i++){
                argumentos[i-1]=argv[i];
        }
	filho = fork();
	if(filho==0){ //é o filho
		execvp(argv[1], argumentos);
	}
	else { //é o pai
		waitpid(filho,&status,0);	
	}
	return 0;
}
