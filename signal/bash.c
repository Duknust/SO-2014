#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h> 
#include <readline/history.h>
#include <string.h>

#define ARRAY_MAX_SIZE 100

// linkar com lreadline

void f(int);

void limpaArray(char *array[]){
	int i;
	for (i=0;i<ARRAY_MAX_SIZE;i++)
		array[i]=NULL;
}

int main(int argc, char** argv){
	signal(SIGINT,f);


	pid_t filho;
	int i=0, status, terminator=0;
	char* array[ARRAY_MAX_SIZE];
	char* catraz = (char*) malloc (sizeof(char)*100);
	char* result;
	int pos=0;
	char* temp = NULL;
	printf("minhabash> ");
	while(fgets(catraz, 100, stdin) != NULL && strcmp(catraz,"exit\n")!=0){
		catraz[strlen(catraz)-1] = '\0';
		terminator=0;
		


		result = strtok(catraz," ");
		while( result != NULL ) {
			array[pos]=result;
			pos++;
			result = strtok (NULL," ");
		}
		if(strcmp(array[pos-1], "&") == 0){
				terminator=1;
				array[pos-1]=NULL;
		}

		filho = fork();
		if(filho==0){
			execvp(array[0], array);
		} else {
			if(!terminator){
				waitpid(filho,&status,0);
			}
		}
		
		printf("\nminhabash> ");
		
		pos=0;
		limpaArray(array);
	}
	free(result);
	free(catraz);
	catraz = (char*) malloc (sizeof(char)*100);
}

void f(int sinal){
	printf("O SINAL!\n");
}


//readline
//history
//wordexp

//bbbbb
