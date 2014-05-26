#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h> 
#include <readline/history.h>
#include <string.h>
#include <signal.h>

void f(int sinal){
	puts("recebi");
}

int main(){
	signal(SIGINT,f);
	pid_t pid = fork();
	if(pid==0){
		pause();
	}
	else 
		kill(pid,SIGINT);
}
