all: servidor cliente

servidor: servidor.c
	gcc -o servidor.exe servidor.c
cliente: cliente.c
	gcc -o cliente.exe cliente.c
	servidor.exe
