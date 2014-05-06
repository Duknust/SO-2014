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
			n = read(pd[0], buffer, sizeof(buffer));
			write(1, buffer, n);
			_exit(0);
		case -1; /* pai: erro */
			perror("fork");
			_exit(1);
		default: /* pai: sucesso */
			write(pd[1], mensagem, strlen(mensagem));
			wait(NULL);
	}
	return 0;
}
