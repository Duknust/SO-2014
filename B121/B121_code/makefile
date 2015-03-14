objects=servidor.o cliente.o erros.o

all: servidor cliente

servidor : erros.o servidorDiv.o
	gcc -Wall -o servidor servidorDiv.o erros.o

cliente : erros.o cliente.o
	gcc -Wall -o cliente cliente.o erros.o

cliente.o : cliente.c
	gcc -c cliente.c -g

servidorDiv.o : servidorDiv.c servidorDiv.h
	gcc -c servidorDiv.c -g

erros.o : erros.c erros.h
	gcc -c erros.c -g

hyperclean:
	rm -f *.o toServidor *_pipe servidor cliente aqui.txt 

clean:
	rm -f *.o toServidor *_pipe servidor cliente
