#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	char* c = (char*)malloc(sizeof(char)*120);
  	int i=0,j=0, fd;
        
	for(j=1;j<argc;j++){
		fd = open(argv[j], O_RDONLY, 0666);
        	if (fd == -1) {
                	perror(argv[1]);
                	return 1;
        	}
		while(read(fd,&c,1)==1){
			i++;
		}
	
        	close(fd);
	}
        while (i>0){
                write(1, &c, 1);
                i--;
        }
        
	return 0;	
}
