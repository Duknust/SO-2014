#include <unistd.h>

int main(){
	char c;
	while (read(0,(void *)&c,1)==1)
		write(1,&c,1);
	return 0;
}

