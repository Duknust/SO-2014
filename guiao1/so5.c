#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

size_t readlnZe(int fildes, char *buf, size_t nbyte){
	int i=0;char *c;
	while (read(fildes,buf,1)>0 && i<nbyte){
	if ((char)*buf+i == '\n'){
		*c=(*buf+i);
		c='\0';
			return i;}
		i++;
	}
	return i;
}

int main(){
	char* buf = (char*)malloc(sizeof(char)*100);
	int res = readlnZe((size_t)0,buf,(size_t)100);
	if (res!=-1)
		printf("%s",buf);
	return 0;
}

