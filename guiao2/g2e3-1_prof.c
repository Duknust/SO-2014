#include <unistd.h>
#include <sys/wait.h>

//ex1
int main(){
    int status;
pid_t pid = fork();
if(pid==0){
	printf("filho:pid=%d, ppid=%d\n",getpid(),getppid());
	return 0;
} else {
	waitpid(pid,&status,0);
    printf("pai:pid=%d, ppdid=%d, pidf=%d\n",getpid(),getppid(),pid);
    
}
return 0;
}
