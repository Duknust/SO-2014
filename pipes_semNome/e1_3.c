#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	//mkfifo(const char* filename, mode_t mode)
	char* fifo = "/Users/duarteduarte/Desktop/SO/pipes_semNome/fifo";
	char buf[1024];
	printf("Eu sou escritor do terminal a abrir\n");
	int fd = open(fifo,O_RDONLY);
	printf("Eu sou escritor do terminal a fechar\n");
	int n=0;
	while((n=read(fd,buf,1024))>0){
		write(1,buf,n);
	}
	return 0;
}