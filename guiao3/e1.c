#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	int i=0;
	for(i=0;i<argc;i++){
		printf("argc[%d] é -- %s --\n", i, argv[i]);
	}
}
