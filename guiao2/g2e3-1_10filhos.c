#include <unistd.h>
#include <sys/wait.h>

//ex1
int main(){
int i=0;
for (i=0;i<10;i++){
pid_t pid = fork();
if(pid==0){
	printf("sou o filho %d ---:pid=%d, ppid=%d\n",i,getpid(),getppid());
	return 0;}

else{
	printf("pai:pid=%d, ppdid=%d, pidf=%d\n",getpid(),getppid(),pid);
}}
return 0;
}
