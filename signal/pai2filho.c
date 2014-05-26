#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h> 
#include <readline/history.h>
#include <string.h>
#include <signal.h>

void altera(int);

int main(){
	pid_t filho=fork();
	if(filho==0){
		while(1){
			printf("O igor tem fome!\n");
		}
	}
	else {
		wait(1);
		printf("O IGOR JA COMEU HOJE!\n");
		kill(filho, SIGUSR1);
	}
}
