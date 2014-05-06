#include <unistd.h>
#include <sys/wait.h>

//ex1
int main(){
printf("ola\n");
pid_t pidDAD = getppid();
pid_t pid = fork();
printf("sou o actual pai:%d\n",pidDAD);
if(pid==0){
	printf("filho:%d\n",pid);
} else {
	printf("pai:%d\n",pid);
}
printf("mundo\n");
return 0;
}
