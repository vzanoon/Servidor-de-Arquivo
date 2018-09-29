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
#include <semaphore.h>
#include <sys/stat.h>

#define BYTE 16384
#define PORTA 5000
#define BACKLOG 10

void Ajuda(int connfd);
void Criar_DIR(int connfd);
void Remover_DIR(int connfd);
void Entrar_DIR(int connfd, DIR **current_dir);
void Sair_DIR(int connfd, DIR **current_dir);
void Mostrar_DIR(int connfd, DIR *current_dir);
void Criar_FILE(int connfd);
void Remover_FILE(int connfd);
void Escrever_FILE(int connfd);
void Mostrar_FILE(int connfd);
void CMD(int connfd);
void Invalido(int connfd);
void retENT(char *recvBuff);


int main(int argc, char *argv[]){
	
	setlocale(LC_ALL, "Portuguese");
	
	/*Listen File Descriptor (listenfd) and Conection File Descriptor (connfd)*/

	int listenfd = 0, connfd = 0, sktbind = 0, sktlisten = 0;
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

	
	
	while(1){
		
		printf("Aguardando conexao.\n");
		
		while(connfd = accept(listenfd, (struct sockaddr*)NULL, NULL))
		{
	
			printf(">: Cliente conectado!\n");
			// O tipo DIR representa um fluxo de diretório, isto é, uma seq. ordenada
			// de todas as entradas de um diretório até um diretório específico.
			DIR *current_dir = NULL; 
			char current_dir_name[BYTE]; // área onde será armazenado o nome do diretório de trabalho
			// Função getcwd() obtém o nome do caminho do diretório de trabalho
			getcwd(current_dir_name, sizeof(current_dir_name)); 
			current_dir = opendir(current_dir_name);// abre um diretório

			printf("> Diretório atual: %s\n\n",current_dir_name);		
			
			snprintf(sendBuff, sizeof(sendBuff), "Conectado!\n");
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
					printf("> %s\n",recvBuff);
					Criar_DIR(connfd);
				}else
				
				if (strcmp(recvBuff,"rdir") == 0)
				{
					printf("> %s\n",recvBuff);
					Remover_DIR(connfd);
				}else
				
				if (strcmp(recvBuff,"edir") == 0)
				{
					printf("> %s\n",recvBuff);
					Entrar_DIR(connfd,&current_dir);
				}else
					
				if (strcmp(recvBuff,"sdir") == 0)
				{
					printf("> %s\n",recvBuff);
					Sair_DIR(connfd,&current_dir);
				}else

				if (strcmp(recvBuff,"mdir") == 0)
				{
					printf("> %s\n",recvBuff);
					Mostrar_DIR(connfd,current_dir);
				}else
				
				if (strcmp(recvBuff,"cfile") == 0)
				{
					printf("> %s\n",recvBuff);
					Criar_FILE(connfd);
				}else
				
				if (strcmp(recvBuff,"rfile") == 0)
				{
					printf("> %s\n",recvBuff);
					Remover_FILE(connfd);
				}else
				
				if (strcmp(recvBuff,"efile") == 0)
				{
					printf("> %s\n",recvBuff);
					Escrever_FILE(connfd);
				}else

				if (strcmp(recvBuff,"mfile") == 0)
				{
					printf("> %s\n",recvBuff);
					Mostrar_FILE(connfd);
				}else
				
				if (strcmp(recvBuff,"cmd") == 0)
				{
					printf("> %s\n",recvBuff);
					CMD(connfd);		
				}else
				
				if (strcmp(recvBuff,"-h") == 0)
				{
					printf("> %s\n",recvBuff);
					Ajuda(connfd);
				}else
					{
						printf("> Cliente digitou comando inválido.\n");
						Invalido(connfd);				
					}
			}while(strcmp(recvBuff,"sair") != 0);
		
			printf(">: Cliente desconectado.\n");
			close(connfd);
			sleep(1);
		}

	}
}

void Ajuda(int connfd)
{
	char sendBuff[BYTE];
	//char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;
	
	snprintf(sendBuff, sizeof(sendBuff), "\n\t\t   | CODE |             MENU        |  #  |\n\t\t   | cdir |  Criar      Diretório   |  #  |\n\t\t   | rdir |  Remover    Diretório   |  #  |\n\t\t   | edir |  Entrar     Diretório   |  #  |\n\t\t   | sdir |  Sair       Diretório   |  #  |\n\t\t   | mdir |  Mostrar    Diretório   |  #  |\n\t\t   | cfile|  Criar      Arquivo     |  #  |\n\t\t   | rfile|  Remover    Arquivo     |  #  |\n\t\t   | efile|  Escrever   Arquivo     |  #  |\n\t\t   | mfile|  Mostrar    Arquivo     |  #  |\n\t\t   | cmd  |  Prompt     Comando     |  #  |\n\t\t   | sair |  Encerrar   Programa    |  #  |\n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
}



void Criar_DIR(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
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
}



void Remover_DIR(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
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
	
}  



void Entrar_DIR(int connfd, DIR **current_dir)
{
	// O tipo DIR representa um fluxo de diretório, isto é, uma seq. ordenada
	// de todas as entradas de um diretório até um diretório específico.
	
	char sendBuff[BYTE]; // buffer de envio
	char recvBuff[BYTE]; // buffer de recebimento
	char current_dir_name[BYTE]; // área onde será armazenado o nome do diretório de trabalho
	char pasta[BYTE]; // buffer que define as pastas do diretório de trabalho
	int tamBuff=0;

	// Esta função fornece o diretório atual de trabalho do processo.
	getcwd(current_dir_name, sizeof(current_dir_name));
	
	snprintf(sendBuff, sizeof(sendBuff), "Entrar em diretório, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	// A função recv() retorna o número de bytes recebidos
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
	
	memset(pasta, 0, sizeof(pasta)); // zera buffer
	
    strcpy(pasta,current_dir_name); // copiar o nome do diretorio corrente p/ buffer pasta
	strcat(pasta,"/"); // concatena com uma barra
	strcat(pasta,recvBuff); // concatena com o buffer recebido
	
    if (chdir(pasta) == 0)	// altera p/ o diretório de trabalho atual definido pelo buffer pasta
	{
        getcwd(current_dir_name, sizeof(current_dir_name)); // obtém o nome do caminho do diretório de trabalho atual
        *current_dir = opendir(current_dir_name);	// abre um diretório
		printf("Entrou no diretório> %s \n",current_dir_name);
		
		snprintf(sendBuff, sizeof(sendBuff), "Diretório atual: %s\n",current_dir_name);
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), " Erro ao entrar em diretório. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
	
} 


void Sair_DIR(int connfd, DIR **current_dir)
{
	// O tipo DIR representa um fluxo de diretório, isto é, uma seq. ordenada
	// de todas as entradas de um diretório até um diretório específico.
	
	char sendBuff[BYTE]; // buffer de envio
	//char recvBuff[BYTE]; // buffer de recebimento
	char current_dir_name[BYTE]; // área onde será armazenado o nome do diretório de trabalho
	char pasta[BYTE]; // buffer que define as pastas do diretório de trabalho
	int tamBuff=0;

	// Esta função fornece o diretório atual de trabalho do processo.
	getcwd(current_dir_name, sizeof(current_dir_name));
	
	memset(pasta, 0, sizeof(pasta)); // zera buffer
	
    strcpy(pasta,current_dir_name); // copiar o nome do diretorio corrente p/ buffer pasta
	strcat(pasta,"/.."); // concatena com uma barra e dois pontos
	
    if (chdir(pasta) == 0)	// altera p/ o diretório de trabalho atual definido pelo buffer pasta
	{
        getcwd(current_dir_name, sizeof(current_dir_name)); // obtém o nome do caminho do diretório de trabalho atual
        *current_dir = opendir(current_dir_name);	// abre um diretório
		printf("Retornou para o diretório> %s \n",current_dir_name);
		
		snprintf(sendBuff, sizeof(sendBuff), "Diretório atual: %s\n",current_dir_name);
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), " Erro ao retornar em diretório. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}

}



void Mostrar_DIR(int connfd, DIR *current_dir)
{	
	// O tipo DIR representa um fluxo de diretório, isto é, uma seq. ordenada
	// de todas as entradas de um diretório até um diretório específico.
		
	char sendBuff[BYTE]; // buffer de envio
	//char recvBuff[BYTE]; // buffer de recebimento
	char current_dir_name[BYTE]; // área onde será armazenado o nome do diretório de trabalho
	struct dirent *dir = NULL; // O dirent da estrutura descreve uma entrada de diretório. 
	
	// Esta função fornece o diretório atual de trabalho do processo.
	getcwd(current_dir_name, sizeof(current_dir_name));

	memset(sendBuff, 0, sizeof(sendBuff)); // preenche área de memoria com 0
	// concatena o nome do diretorio de trabalho ao buffer de envio  
	strcat(sendBuff,"Diretório > ");
	strcat(sendBuff,current_dir_name);
	strcat(sendBuff,"\n\n\t");
	
	// A função readdir () retorna um ponteiro para uma estrutura que representa
    // a entrada de diretório na posição atual no fluxo de diretório especificado
    // pelo argumento current_dir e posiciona o fluxo de diretório na próxima entrada. 
	// Ela retorna um ponteiro nulo ao atingir o final do fluxo do diretório. 
	
	while(dir = readdir(current_dir)){ // listagem do diretório
		strcat(sendBuff, dir->d_name);
		strcat(sendBuff, "\t\n\t");
	}
	strcat(sendBuff, "\n");
	// função redefine a posição de um fluxo de diretório para o início do diretório raiz
	rewinddir(current_dir);	
	send(connfd,sendBuff,strlen(sendBuff),0);	
	
} 



void Criar_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
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
}



void Remover_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Remover arquivo, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	// A função recv() retorna o número de bytes recebidos
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
	
	char comando[BYTE]  = "rm ";
	strcat(comando,recvBuff);
	
	if (system(comando) == 0)
	{
		snprintf(sendBuff, sizeof(sendBuff), "Arquivo removido com sucesso. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
	}else
		{			
		snprintf(sendBuff, sizeof(sendBuff), "Erro ao remover arquivo. \n");
		send(connfd,sendBuff,strlen(sendBuff), 0);
		}
	
}  



void Escrever_FILE(int connfd)
{
	
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Escrever no arquivo, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	// A função recv() retorna o número de bytes recebidos
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
	strcat(sendBuff, "\n");
	

 
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
			//fwrite(sendBuff, sizeof(char), 1024, arquivo);	
		}
		fclose(arquivo);
	
} 



void Mostrar_FILE(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;
	int i;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Listar arquivo, digite o nome: \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
	
	// A função recv() retorna o número de bytes recebidos
	tamBuff = recv(connfd,recvBuff,BYTE, 0);
	recvBuff[tamBuff] = 0x00;
	retENT(recvBuff);
 
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
		
} 



void CMD(int connfd)
{
	char sendBuff[BYTE];
	char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;
	
	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
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
	//char recvBuff[BYTE];
	//char current_dir_name[BYTE];
	int tamBuff=0;

	//getcwd(current_dir_name, sizeof(current_dir_name));
	//printf("%s\n",current_dir_name);
	
	snprintf(sendBuff, sizeof(sendBuff), "Comando inválido. Ajuda -h \n");
	send(connfd,sendBuff,strlen(sendBuff), 0);
}

void retENT(char *recvBuff)	// remove ENTER do final do buffer recebido
{
	
	if (recvBuff[strlen(recvBuff)-1] == 10) // 10 é o ENTER em ASCII
		recvBuff[strlen(recvBuff)-1] = 0x00;	
}