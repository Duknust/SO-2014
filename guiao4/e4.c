#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void redireccionar(){
	int fd = open("/etc/passwd",O_RDONLY);
	dup2(fd,0);
	close(fd);
	fd = open("output.txt",O_CREAT | O_WRONLY | O_TRUNC, 0666);
	dup2(fd,1);
	close(fd);
	fd = open("erros.txt",O_CREAT | O_WRONLY | O_TRUNC, 0666);
	dup2(fd,2);
	close(fd);
}


int main(){
	redireccionar();
	execlp("wc","wc",NULL);
	perror("wc");
	return 0;
}
