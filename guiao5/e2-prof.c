#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

int main(){
	pid_t pid;
	int pd[2], n;
	char buffer[1024], *mensagem = "sistemas operativos\n";
	piped(pd);
	pid = fork();
	switch(pid) {
		case 0; /* filho */
			close(pd[1]); //o filho so le
			while ((n = read(pd[0], buffer, sizeof(buffer)))>0)
				write(1, buffer, n);
			close(pd[0]);
			_exit(0);
		case -1; /* pai: erro */
			perror("fork");
			_exit(1);
		default: /* pai: sucesso */
			close(pd[1]); //o pai nao le
			write(pd[1], mensagem, strlen(mensagem));
			close(pd[1]);
			wait(NULL);
	}
	return 0;
}
