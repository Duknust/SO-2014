#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define N 1024*1024*1024
#define M 4

char vector[N];

int main(){
	int i, j, parcial=0;
	pid_t pid[M]; //filhos que cria
	vector[N/8-1] = vector [N/7] = vector[N/6] = vector[N/6+1] =
		vector[N/8+1] = 1;
	for(j=0; j!=M; j++){
		pid[j]=fork();
		if(pid[j] == 0){	
			for(i=0;i!=N/M;i++)
				if (vector[j*(N/M)+i]) parcial++;
			_exit(parcial);
		}
	}
	for(j=0; j!=M;j++){
		int status;
		pid_t filho;
		filho=wait(&status);
		for(i=0;pid[i]!=filho;i++);
		if(WIFEXITED(status)) {
			printf("filho #%d = %d\n", i, WEXITSTATUS(status));
			/*
			char buffer[1024];
			sprintf(buffer, "filho #%d = %d\n", i, WEXITSTATUS(status));
			write(1, buffer, strlen(buffer));
			*/
		}
	}
	return 0;
}
