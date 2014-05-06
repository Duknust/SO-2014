#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	//mkfifo(const char* filename, mode_t mode)
	char* fifo="/Users/duarteduarte/Desktop/SO/pipes_semNome/fifo";

	mkfifo(fifo, 0666);
	return 0;
}