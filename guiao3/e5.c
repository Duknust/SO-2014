#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
	int i=0;
	int status=0;
	pid_t filho=0;
	for(i=1;i<argc;i++){
		filho = fork();
		if(filho==0){ //é o filho
			execlp(argv[i],"");
		}
		else { //é o pai
			waitpid(filho,&status,0);	
		}
	}
}
