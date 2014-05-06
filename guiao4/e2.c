#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void redireccionar(){
	int fd = open("/etc/passwd",O_RDONLY);
	dup2(fd,0);
	close(fd);
	fd = open("output.txt",O_CREAT | O_WRONLY | O_TRUNC, 0666);
	dup2(fd,1);
	close(fd);
	fd = open("erros.txt",O_CREAT | O_WRONLY | O_TRUNC, 0666);
	dup2(fd,2);
	close(fd);
}


int main(int argc, char* argv[]){
	FILE *fp = NULL;
	char buffer[1024];
	redireccionar();
	pid_t filho;		
	filho = fork();
	if(filho == 0){
		switch(argc){
		case 1: 
			fp = stdin; 
			break;
		case 2:
			fp = fopen(argv[1],"r");
			if (fp==NULL){
				perror(argv[2]);
				return 1;
			}
			break;
		default:
			fprintf(stderr,"utilizacao: %s [ficheiro]\n",argv[0]);
			return 1;
		}
	
	while (fgets(buffer,sizeof(buffer),fp) != NULL)
		printf("%s",buffer);
	if(argc==2)
		fclose(fp);
	}
	if(filho!=0){
		printf("\n\n o pai ja foi! \n\n");
	}
	return 0;
}
