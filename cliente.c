#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <windows.h>
#include <locale.h>

#define SERVER_IP "127.0.0.1"
#define BYTE 1024
#define PORTA 5000
#define TITULO "\n ###### EXPLORADOR DE ARQUIVOS CLIENTE ######\n\n"

/* Declaração de Métodos */
void imprimirAguarde(void);


int main(int argc, char *argv[])
{

	setlocale(LC_ALL, "Portuguese");
    char mensagem[BYTE], *loc;
    int tbuf, skt, escolha;
    struct sockaddr_in serv;
    system("clear");

    /**Estrutura os campos do struct*/
    skt = socket(AF_INET, SOCK_STREAM, 6);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv.sin_port = htons (PORTA);
    memset (&(serv.sin_zero), 0x00, sizeof (serv.sin_zero));

    /**Inicializa a comunicação com o Servidor*/
    while(connect (skt, (struct sockaddr *)&serv, sizeof (struct sockaddr)) != 0){
        imprimirAguarde();      ///AGUARDA SERVIDOR SE COMUNICAR
    }
    printf(">> A Conexão com o Servidor %s foi estabelecida na porta %d\n\n",SERVER_IP,PORTA);
    printf(">> Envie sair pra encerrar ou -h para ajuda \n\n");


    /**Recebe Mensagem do Servidor*/
    tbuf = recv (skt, mensagem, BYTE, 0);
    mensagem[tbuf] = 0x00;
    printf (">: %s\n",mensagem);

    /**Envia mensagem para o Servidor*/
    //strcpy(mensagem, "Cliente diz: olá!!!");
    //send(skt, mensagem, strlen(mensagem), 0 );


    /**Loop de Comunicação entre Cliente e Servidor*/
    do{
        ///envia
        printf("> ");
        gets(mensagem);
		if (strlen(mensagem)==0) strcpy(mensagem," ");
        send(skt, mensagem, strlen(mensagem), 0);

		if (strcmp(mensagem,"sair")!= 0)
		{
			tbuf = recv (skt, mensagem, BYTE, 0);
			mensagem[tbuf] = 0x00;
			printf ("> %s\n",mensagem);
		}
    }while(strcmp(mensagem,"sair")!= 0);


    /**Finaliza a conexão! */
    close(skt);
    printf (">>A conexão com o servidor foi finalizada!!!\n\n");
	sleep(3);
    exit(0);
}

/* Função responsável por imprimir mensagem na tela enquanto aguarda algum servidor estabelecer comunicação */

void imprimirAguarde(){
    int i=0;
    char dot[12] = "";
    for(i=0; i<4;i++){
        system("clear");
        printf(TITULO);
        printf("\n\nProcurando servidor.");
        printf("\nAguarde %s\n\n", dot);
        strcat(dot,".");
        sleep(1);
    }
    strcpy(dot, "");
}