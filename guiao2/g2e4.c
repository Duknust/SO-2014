#include <unistd.h>
#include <sys/wait.h>

int runInChild() {
	int j=0;	

	while(j++<1000000000);
	return 0;
}

int main(){
	pid_t children[10],pid=0;
	int status[10];
	int i=0;
	
	for (i=0;i<10;i++){
		pid=fork();
		if(pid!=0){
			children[i]=pid;
		} else {
			printf("eu sou o filho: %d\n",pid);
	//		runInChild();
			_exit(0);
		}
	}
	for(i=0;i<10;i++){
		waitpid(children[i],&status[i],0);
	}
	printf("eu sou o pai: %d\n",getpid());

}

