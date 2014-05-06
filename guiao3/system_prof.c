#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int sistema(const char *comando){
	int argc, status;
	char *copia = strdup(comando), *aux = NULL;
	char **argv = NULL;
	pid_t pid;
	for(argc=0, aux = strtok(copia," "); aux != NULL; aux = strtok(NULL, " "), argc++); /* NAO FAZ NADA XD */
	argv = (char **)malloc((argc+1) * sizeof(char*));
	strcpy(copia, comando);
	for(argc=0, aux = strtok(copia," "); aux != NULL; aux = strtok(NULL, " "), argc++){
		argv[argc] = aux;
	}
	argv[argc] = NULL;
	for (argc = 0; argv[argc]!=NULL; argc++){
		printf("argv[%d] = \"%s \"\n",argc, argv[argc]);
	}
	pid = fork();
	if (pid==0){
		execvp(argv[0],argv);
		perror(comando);
		_exit(EXIT_FAILURE);
	}
	waitpid(pid,&status,0);
	free(copia);
	free(argv);
	return WIFEXITED(status)? WEXITSTATUS(status):-1;
}


int main(){
	sistema("ls -l -a");
	puts("sucesso");
	return 0;
}

int system_a_serio(const char *comando){
	pid_t pid;
	if ((pid=fork())==0){
		execlp("sh","sh","-c",comando,NULL);
		perror(comando);
		_exit(EXIT_FAILURE);
	}
	waitpid(pid,&status,0);
	return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}