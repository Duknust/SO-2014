#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>

int main(int argc, char *argv[]){
        char *c;
	int size = atoi(argv[1]);
	c = (char*) malloc(size*sizeof(char));
        while (read(0,c,size)>0){
		write(1,c,size);
	}
	free(c);
        return 0;
}
