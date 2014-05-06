#include <unistd.h>

int main(){
	int pd[2];
	char* params[3];
	int i=0;
	pipe(pd);
	pid_t filho;

	for(i=0;i<3;i++)
		params[i]=NULL;

	filho=fork();


	if (filho!=0){
		dup2(pd[0],0);
		close(pd[0]);
		close(pd[1]);
		waitpid(filho);
		
		params[0]="wc";
		params[1]="-l";
	  	params[2]=NULL;

		execvp("wc",params);
		}
	else {
		dup2(pd[1],1);
		close(pd[1]);
		close(pd[0]);
		execlp("ls","ls","/etc",NULL);
		}
} 
