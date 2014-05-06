#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	int i;
	char **args = (char**) malloc (sizeof(char*)*(argc+1)); //alocar espaço para os argumentos 
	args[0] = "./coiso";
	for(i=1; i<argc; i++){
		args[i]=argv[i];
	}
	args[argc] = NULL;
	execv("./coiso",args); //como nao sei quantos argumentos tenho 
	return EXIT_SUCCESS;
}
