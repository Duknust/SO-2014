#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(){
	//mkfifo(const char* filename, mode_t mode)
	char* fifo;
	char buf[1024];
	mkfifo(fifo, 0666);
	int fd = open(fifo,O_WRONLY);
	while((n=read(0,buf,1024))>0){
		write(fd,buf,n);
	}
	return 0;
}