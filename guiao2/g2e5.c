#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int runInChild() {
    int j=0;
    
    while(j++<1000000000);
    return 0;
}

int main(){
    pid_t children[10],pid=0;
    int status[10];
    int i=0, level=0;
    printf("goiging to fork\n");
    
    while(level++<10){
        
        pid=fork();
        if(pid==0) {
            printf("eu: %d, pai: %d\n",getpid(),getppid());
            //_exit(0);
        } else {
            waitpid(pid,&status[level],0);
            _exit(0);
        }
    }
    
}