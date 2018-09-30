#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <windows.h>
#include <locale.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>

#define BYTE 16384
#define PORTA 5000
#define BACKLOG 10

void Ajuda(int connfd);
void Criar_DIR(int connfd, char *current_dir);
void Remover_DIR(int connfd, char *current_dir);
void Entrar_DIR(int connfd, char *current_dir);
void Sair_DIR(int connfd, char *current_dir);
void Mostrar_DIR(int connfd, char *current_dir);
void Criar_FILE(int connfd, char *current_dir);
void Remover_FILE(int connfd, char *current_dir);
void Escrever_FILE(int connfd, char *current_dir);
void Mostrar_FILE(int connfd, char *current_dir);
void CMD(int connfd);
void Invalido(int connfd);
void retENT(char *recvBuff);
void* pthreads_worker(void *connect);


char current_dir_raiz[BYTE];
pthread_mutex_t mutex;


int main(int argc, char *argv[]){
	
	setlocale(LC_ALL, "Portuguese");
	
	/*Listen File Descriptor (listenfd) and Conection File Descriptor (connfd)*/

	int listenfd = 0, connfd = 0, sktbind = 0, sktlisten = 0, *id_socket;
	struct sockaddr_in serv_addr; // por alocação automática

	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char current_dir_name[BYTE];
	int tamBuff=0;

	/* Zera a struct*/
	listenfd = socket(AF_INET, SOCK_STREAM,6); // AF_INET  Arpa Internet Protocols, SOCK_STREAM socket por stream, 0  Internet Protocol
	
	if (listenfd == -1) // verifica se ocorreu erro na criação do socket descritor
	{
		printf("Erro: Criar socket descritor.\n");
	}else
		printf("Criado socket descritor!\n");
	
	
	memset(&serv_addr, '0', sizeof(serv_addr)); // ou poderia usar o bzero
	memset(sendBuff, '0', sizeof(sendBuff)); // preenche área de memoria com 0
	memset(recvBuff, '0', sizeof(recvBuff)); // preenche área de memoria com 0

	/*Instancia os campos do Struct*/
	serv_addr.sin_family = AF_INET; // familia
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // endereço
	serv_addr.sin_port = htons(PORTA); // porta

	/* Associa um endereço ao descritor do socket */
	sktbind = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); // associa esse soquete a um porta na sua máquina local
	
	char mybuf[80];
	inet_ntop(AF_INET, &serv_addr.sin_addr, mybuf, 80);
	printf("Socket IP:PORTA = %d, %s, %d\n", serv_addr.sin_family, mybuf, ntohs(serv_addr.sin_port));
	
	
	if (sktbind == -1) // verifica se ocorreu erro na associção do socket a um endereço
	{
		printf("Erro: Bind socket.\n");
	}else
		printf("Bind socket!\n");	
	
	sktlisten = listen(listenfd, BACKLOG);	// fila para escutar os clientes da conexão socket
	
	if (sktlisten == -1) // verifica se ocorreu erro na fila
	{
		printf("Erro: Listen socket.\n");
	}else
		printf("Listen socket!\n");	

	getcwd(current_dir_raiz, BYTE); //obtém o nome do caminho do diretório raiz de trabalho 
	
	pthread_mutex_init(&mutex, NULL);
	
	while(1){
		
		printf("Aguardando conexao.\n");
		
		while(connfd = accept(listenfd, (struct sockaddr*)NULL, NULL))
		{
			// cria a pthread do cliente
			pthread_t pthread_c; 				
			pthread_create(&pthread_c, NULL, pthreads_worker, &connfd);
			sleep(1);
		}

	}
}


void* pthreads_worker(void *connect){
	
	
		int connfd = *(int*)connect;
		int *id_socket;
		char sendBuff[BYTE];
		char recvBuff[BYTE];
		char current_dir[BYTE]; // área onde será armazenado o nome do diretório de trabalho
		int tamBuff=0;
		
		//DIR *current_dir = NULL;
		
		id_socket = malloc(sizeof(BYTE)); // aloca memoria para o conteudo do endereço ID do socket.
		//printf("\n---SIZEOF %i --- \n", (int) sizeof(BYTE)); // return : 4
		*id_socket = connfd; // atribuir o endereço ID do socket, pois connfd carrega em seu conteúdo o endereço de memória do socket e não o ID.
		//printf("\n --- connfd: %x ---\n", &connfd);

		printf(">: Cliente %i conectado!\n", id_socket);
	
		strcpy(current_dir, current_dir_raiz);
		
		
		// Função getcwd() obtém o nome do caminho do diretório de trabalho
		//getcwd(current_dir_name, sizeof(current_dir_name));
		//current_dir = opendir(current_dir_name);	// abre um diretório
		printf("> Diretório atual do Servidor: %s\n\n",current_dir);		
		
		memset(sendBuff,0, sizeof(sendBuff));
		memset(recvBuff,0, sizeof(recvBuff));
		
		snprintf(sendBuff, sizeof(sendBuff), "Cliente %i conectado no Diretório %s!\n", id_socket, current_dir);
		send(connfd,sendBuff,strlen(sendBuff), 0);
	
		do
		{
			// A função recv() retorna o número de bytes recebidos
			tamBuff = recv(connfd,recvBuff,BYTE, 0);
			recvBuff[tamBuff] = 0x00;

			
			if (tamBuff < 0) // erro na recepção de mensagem
			{
				printf("Erro: Buffer de entrada.\n");
				snprintf(recvBuff, sizeof(recvBuff), "sair");
				tamBuff = strlen(recvBuff);

			}else
			
			if (strcmp(recvBuff,"cdir") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				Criar_DIR(connfd,current_dir);
				
			}else
			
			if (strcmp(recvBuff,"rdir") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				Remover_DIR(connfd,current_dir);
				
			}else
			
			if (strcmp(recvBuff,"edir") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				Entrar_DIR(connfd,current_dir);
			}else
				
			if (strcmp(recvBuff,"sdir") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				Sair_DIR(connfd,current_dir);
			}else

			if (strcmp(recvBuff,"mdir") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				Mostrar_DIR(connfd,current_dir);
			}else
			
			if (strcmp(recvBuff,"cfile") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				Criar_FILE(connfd,current_dir);
			}else
			
			if (strcmp(recvBuff,"rfile") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				Remover_FILE(connfd,current_dir);
			}else
			
			if (strcmp(recvBuff,"efile") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				Escrever_FILE(connfd,current_dir);
			}else

			if (strcmp(recvBuff,"mfile") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				Mostrar_FILE(connfd,current_dir);
			}else
			
			if (strcmp(recvBuff,"cmd") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				CMD(connfd);		
			}else
			
			if (strcmp(recvBuff,"-h") == 0)
			{
				printf("> %i: %s\n",id_socket,recvBuff);
				Ajuda(connfd);
			}else
				{
					printf("> Cliente %i digitou comando inválido.\n", id_socket);
					Invalido(connfd);				
				}
		}while(strcmp(recvBuff,"sair") != 0);
	
		printf(">: Cliente %i desconectado.\n", id_socket);
		close(connfd);
		free(id_socket);
		pthread_exit(0);
		
	
	
}

void Ajuda(int connfd)
{
	char sendBuff[BYTE];
	
	snprintf(sendBuff, sizeof(sendBuff), "\n\t\t   | CODE |             MENU        |  #  |\n\t\t   | cdir |  Criar      Diretório   |  #  |\n\t\t   | rdir |  Remover    Diretório   |  #  |\n\t\t   | edir |  Entrar     Diretório   |  #  |\n\t\t   | sdir |  Sair       Diretório   |  #  |\n\t\t   | mdir |  Mostrar    Diretório   |  #  |\n\t\t   | cfile|  Criar      Arquivo     |  #  |\n\t\t   | rfile|  Remover    Arquivo     |  #  |\n\t\t   | efile|  Escrever   Arquivo     |  #  |\n\t\t   | mfile|  Mostrar    Arquivo     |  #  |\n\t\t   | cmd  |  Prompt     Comando     |  #  |\n\t\t   | sair |  Encerrar   Programa    |  #  |\n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
}

void Criar_DIR(int connfd, char *current_dir)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;

	if(chdir(current_dir)==0){
		snprintf(sendBuff, sizeof(sendBuff), "Criar diretório, digite o nome: \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		// A função recv() retorna o número de bytes recebidos
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
	
		char comando[1024]  = "mkdir ";
		strcat(comando,recvBuff);
		
		if (system(comando) == 0)
		{
			snprintf(sendBuff, sizeof(sendBuff), "Diretório criado com sucesso. \n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else
			{			
			snprintf(sendBuff, sizeof(sendBuff), "Erro ao criar diretório. \n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
			}
	
	}else{
		
		stpcpy(current_dir,current_dir_raiz);
		snprintf(sendBuff, sizeof(sendBuff),"Erro ao acessar o diretório atual. Redirecionando ao diretório raiz.\n");
		send(connfd, sendBuff,strlen(sendBuff),0);
	}
}



void Remover_DIR(int connfd, char *current_dir)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;

	
	if(chdir(current_dir)==0){
		snprintf(sendBuff, sizeof(sendBuff), "Excluir diretório, digite o nome: \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		
		// A função recv() retorna o número de bytes recebidos
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		
		char comando[1024]  = "rmdir ";
		strcat(comando,recvBuff);
			
		if (system(comando) == 0)
		{
			snprintf(sendBuff, sizeof(sendBuff), "Diretório excluído com sucesso. \n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else
			{			
			snprintf(sendBuff, sizeof(sendBuff), "Erro ao excluir diretório. \n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
			}
	}else{
		
		stpcpy(current_dir,current_dir_raiz);
		snprintf(sendBuff, sizeof(sendBuff),"Erro ao acessar o diretório atual. Redirecionando ao diretório raiz.\n");
		send(connfd, sendBuff,strlen(sendBuff),0);
	}
	
}  



void Entrar_DIR(int connfd, char *current_dir)
{
	
	char sendBuff[BYTE]; // buffer de envio
	char recvBuff[BYTE]; // buffer de recebimento
	char pasta[BYTE]; // buffer que define as pastas do diretório de trabalho
	int tamBuff=0;

	
	if(chdir(current_dir)==0){
	
		snprintf(sendBuff, sizeof(sendBuff), "Entrar em diretório, digite o nome: \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		
		// A função recv() retorna o número de bytes recebidos
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		
		memset(pasta, 0, sizeof(pasta)); // zera buffer
		
		strcpy(pasta,current_dir); // copiar o nome do diretorio corrente p/ buffer pasta
		strcat(pasta,"/"); // concatena com uma barra
		strcat(pasta,recvBuff); // concatena com o buffer recebido
		
		if (chdir(pasta) == 0)	// altera p/ o diretório de trabalho atual definido pelo buffer pasta
		{
			getcwd(current_dir, BYTE); // obtém o nome do caminho do diretório de trabalho atual
			printf("> Entrou no diretório: %s \n",current_dir);
			
			snprintf(sendBuff, sizeof(sendBuff), "Diretório atual: %s\n",current_dir);
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else
			{			
			snprintf(sendBuff, sizeof(sendBuff), " Erro ao entrar em diretório. \n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
			}
	}else{
		
		stpcpy(current_dir,current_dir_raiz);
		snprintf(sendBuff, sizeof(sendBuff),"Erro ao acessar o diretório atual. Redirecionando ao diretório raiz.\n");
		send(connfd, sendBuff,strlen(sendBuff),0);	
	}
		
} 


void Sair_DIR(int connfd, char *current_dir)
{
	
	char sendBuff[BYTE]; // buffer de envio
	char pasta[BYTE]; // buffer que define as pastas do diretório de trabalho
	int tamBuff=0;

	if(chdir(current_dir) == 0){
	
		memset(pasta, 0, sizeof(pasta)); // zera buffer
		
		strcpy(pasta,current_dir); // copiar o nome do diretorio corrente p/ buffer pasta
		strcat(pasta,"/.."); // concatena com uma barra e dois pontos
		
		if (chdir(pasta) == 0)	// altera p/ o diretório de trabalho atual definido pelo buffer pasta
		{
			getcwd(current_dir, BYTE); // obtém o nome do caminho do diretório de trabalho atual
			printf("> Retornou para o diretório: %s \n",current_dir);
			
			snprintf(sendBuff, sizeof(sendBuff), "Diretório atual: %s\n",current_dir);
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else{			
			snprintf(sendBuff, sizeof(sendBuff), " Erro ao retornar em diretório. \n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}
	}else{
		stpcpy(current_dir,current_dir_raiz);
		snprintf(sendBuff, sizeof(sendBuff),"Erro ao acessar o diretório atual. Redirecionando ao diretório raiz.\n");
		send(connfd, sendBuff,strlen(sendBuff),0);
	}
}



void Mostrar_DIR(int connfd, char *current_dir)
{	
	// O tipo DIR representa um fluxo de diretório, isto é, uma seq. ordenada
	// de todas as entradas de um diretório até um diretório específico.
		
	char sendBuff[BYTE]; // buffer de envio
	DIR *dir_now = NULL;
	struct dirent *dir = NULL; // O dirent da estrutura descreve uma entrada de diretório. 
	
	
	if(chdir(current_dir) == 0){
		
		dir_now = opendir(current_dir);
		
		memset(sendBuff, 0, sizeof(sendBuff)); // preenche área de memoria com 0
		
		// concatena o nome do diretorio de trabalho ao buffer de envio  
		strcat(sendBuff,"Diretório > ");
		strcat(sendBuff,current_dir);
		strcat(sendBuff,"\n\n\t");
		
		// A função readdir () retorna um ponteiro para uma estrutura que representa
		// a entrada de diretório na posição atual no fluxo de diretório especificado
		// pelo argumento current_dir e posiciona o fluxo de diretório na próxima entrada. 
		// Ela retorna um ponteiro nulo ao atingir o final do fluxo do diretório. 
		
		while(dir = readdir(dir_now)){ // listagem do diretório
			strcat(sendBuff, dir->d_name);
			strcat(sendBuff, "\t\n\t");
		}
		strcat(sendBuff, "\n");
		// função redefine a posição de um fluxo de diretório para o início do diretório raiz
		rewinddir(dir_now);	
		send(connfd,sendBuff,strlen(sendBuff),0);	
		
	}else{
		
		stpcpy(current_dir,current_dir_raiz);
		snprintf(sendBuff, sizeof(sendBuff),"Erro ao acessar o diretório atual. Redirecionando ao diretório raiz.\n");
		send(connfd, sendBuff,strlen(sendBuff),0);
		
	}
	
} 



void Criar_FILE(int connfd, char *current_dir)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;

	if(chdir(current_dir)==0){
	
		snprintf(sendBuff, sizeof(sendBuff), "Criar arquivo, digite o nome: \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		
		// A função recv() retorna o número de bytes recebidos
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		
		char comando[1024]  = "touch ";
		strcat(comando,recvBuff);
			
		if (system(comando) == 0)
		{
			snprintf(sendBuff, sizeof(sendBuff), "Arquivo criado com sucesso. \n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
		}else
			{			
			snprintf(sendBuff, sizeof(sendBuff), "Erro ao criar arquivo. \n");
			send(connfd,sendBuff,strlen(sendBuff), 0);
			}
	}else{
		stpcpy(current_dir,current_dir_raiz);
		snprintf(sendBuff, sizeof(sendBuff),"Erro ao acessar o diretório atual. Redirecionando ao diretório raiz.\n");
		send(connfd, sendBuff,strlen(sendBuff),0);
		
	}
}



void Remover_FILE(int connfd, char *current_dir)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;

	
	
	if(chdir(current_dir) == 0){
		snprintf(sendBuff, sizeof(sendBuff), "Remover arquivo, digite o nome: \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		
		// A função recv() retorna o número de bytes recebidos
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		
		char comando[BYTE]  = "rm ";
		strcat(comando,recvBuff);
		
		// Mutex somente para remoção!
		pthread_mutex_lock(&mutex);
			if (system(comando) == 0)
			{
				snprintf(sendBuff, sizeof(sendBuff), "Arquivo removido com sucesso. \n");
				send(connfd,sendBuff,strlen(sendBuff), 0);
			}else
				{			
				snprintf(sendBuff, sizeof(sendBuff), "Erro ao remover arquivo. \n");
				send(connfd,sendBuff,strlen(sendBuff), 0);
				}
		pthread_mutex_unlock(&mutex);
	}else{
		stpcpy(current_dir,current_dir_raiz);
		snprintf(sendBuff, sizeof(sendBuff),"Erro ao acessar o diretório atual. Redirecionando ao diretório raiz.\n");
		send(connfd, sendBuff,strlen(sendBuff),0);
	}
		
}  



void Escrever_FILE(int connfd, char *current_dir)
{
	
	char sendBuff[BYTE];
	char recvBuff[BYTE];

	int tamBuff=0;

	
	
	if(chdir(current_dir) == 0){
		
		snprintf(sendBuff, sizeof(sendBuff), "Escrever no arquivo, digite o nome: \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		
		// A função recv() retorna o número de bytes recebidos
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		strcat(sendBuff, "\n");
		// Mutex somente para escrita!
		pthread_mutex_lock(&mutex);
			FILE *arquivo; 
			arquivo = fopen(recvBuff,"a+");
			
			if(arquivo == NULL){
				strcpy(sendBuff, "Falha ao abrir o arquivo!\n");
				send(connfd, sendBuff, strlen(sendBuff), 0);
			}else{
				snprintf(sendBuff, sizeof(sendBuff), "Escreva no arquivo: \n");
				send(connfd,sendBuff,strlen(sendBuff), 0);
				
				tamBuff = recv(connfd,recvBuff,BYTE, 0);
				recvBuff[tamBuff] = 0x00;
					if((fprintf(arquivo,recvBuff))>0){
						snprintf(sendBuff, sizeof(sendBuff), "Escrita no arquivo realizada com sucesso! \n");
						send(connfd,sendBuff,strlen(sendBuff), 0);
					}else{
						snprintf(sendBuff, sizeof(sendBuff), "Escrita no arquivo não realizada com sucesso! \n");
						send(connfd,sendBuff,strlen(sendBuff), 0);
					}
			}
			fclose(arquivo);
		pthread_mutex_unlock(&mutex);
	}else{
	
		stpcpy(current_dir,current_dir_raiz);
		snprintf(sendBuff, sizeof(sendBuff),"Erro ao acessar o diretório atual. Redirecionando ao diretório raiz.\n");
		send(connfd, sendBuff,strlen(sendBuff),0);
	}
	
} 



void Mostrar_FILE(int connfd, char *current_dir)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	int tamBuff=0;

	if(chdir(current_dir)==0){
	
		snprintf(sendBuff, sizeof(sendBuff), "Listar arquivo, digite o nome: \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		
		// A função recv() retorna o número de bytes recebidos
		tamBuff = recv(connfd,recvBuff,BYTE, 0);
		recvBuff[tamBuff] = 0x00;
		retENT(recvBuff);
		
			// Mutex somente para mostrar
			pthread_mutex_lock(&mutex);
			FILE *arquivo; 
			arquivo = fopen(recvBuff,"r");
			
				if(arquivo == NULL){
					strcpy(sendBuff, "Falha ao abrir arquivo!\n");
					send(connfd, sendBuff, strlen(sendBuff), 0);
				}else{
					memset(sendBuff, 0, sizeof (sendBuff));
					fread(sendBuff, sizeof(char), BYTE-1, arquivo);
					strcat(sendBuff, "\n");
					send(connfd, sendBuff, strlen(sendBuff), 0);
				}
			fclose(arquivo);
			pthread_mutex_unlock(&mutex);
	}else{
		stpcpy(current_dir,current_dir_raiz);
		snprintf(sendBuff, sizeof(sendBuff),"Erro ao acessar o diretório atual. Redirecionando ao diretório raiz.\n");
		send(connfd, sendBuff,strlen(sendBuff),0);
	
	}
		
} 



void CMD(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	char current_dir_name[BYTE];
	int tamBuff=0;
	
	getcwd(current_dir_name, sizeof(current_dir_name));
	printf("> %s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Digite o comando: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	// A função recv() retorna o número de bytes recebidos
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);

	char comando[1024]  = "";
	strcat(comando,recvBuff);
	
	if (system(comando) == 0)
	{
		snprintf(sendBuff, sizeof(sendBuff), "OK \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), "Erro ao chamar comando. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
		
}



void Invalido(int connfd)
{
	char sendBuff[BYTE];
	
	snprintf(sendBuff, sizeof(sendBuff), "Comando inválido. Menu: -h \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
}

void retENT(char *recvBuff)	// remove ENTER do final do buffer recebido
{
	
	if (recvBuff[strlen(recvBuff)-1] == 10) // 10 é o ENTER em ASCII
		recvBuff[strlen(recvBuff)-1] = 0x00;	
}