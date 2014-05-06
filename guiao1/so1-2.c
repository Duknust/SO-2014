#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	int i=0, fd;
	char c='a';
	fd = open(argv[1], O_CREAT|O_WRONLY, 0666);
	if (fd == -1) {
		perror(argv[1]);
		return 1;
	}
	while (i<1024*1024*10){
		write(fd, &c, sizeof(c));
		i++;
	}
	close(fd);
	return 0;
}
