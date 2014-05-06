#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

int main(){
	pid_t pid;
	int pd[2], n;
	char buffer[1024];
	pipe(pd);
	pid = fork();
	switch (pid){	
		case 0:
			dup2(pd[0],0);
			close(pd[0]); //ja o dupliquei para cima do 0	
			close(pd[1]);
			execlp("wc","wc",NULL);
			perror("wc");
			_exit(1);
		case -1:
			perror("fork");
			_exit(1);
		default:
			close(pd[0]);
			while((n=read(0,buffer,sizeof(buffer)))>0)
				write(pd[1],buffer,n);
			close(pd[1]);
			wait(NULL);
	}
	return 0;
}
