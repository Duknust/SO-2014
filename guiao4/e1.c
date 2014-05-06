#include <unistd.h> 
#include <stdio.h>
#include <string.h>

int main(){
	//char buffer[10000];
	char* texto = (char*)malloc(sizeof(char)*10000);


	while(fgets(texto,10000,stdin))
		printf("%s",texto);
 	perror("\n\no bo tem mel\n\n");
	return 0;
}
