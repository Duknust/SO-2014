#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	//mkfifo(const char* filename, mode_t mode)
	char* fifo="/Users/duarteduarte/Desktop/SO/pipes_semNome/fifo";
	char buf[1024];
//	mkfifo(fifo, 0666);
	int n=0;
	printf("Eu sou leitor do terminal a abrir\n");
	int fd = open(fifo,O_WRONLY);
	printf("Eu sou leitor do terminal a fechar\n");
	while((n=read(0,buf,1024))>0){
		write(fd,buf,n);
	}
	return 0;
}