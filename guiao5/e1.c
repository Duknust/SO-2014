#include <unistd.h> 

int main(){
	int p[2];
	int pipe_criado = pipe(p);
	pid_t filho;
	char bufp[100] = "ola mundo\0";
	char buff[100];
	int n=0;

	filho = fork();
	if(filho==0){
		n = read(p[0],&buff,sizeof(buffer);
		_exit(0);
	} else {
		write(p[1],&bufp,n);
	}
}
