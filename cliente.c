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
#define TITULO "\n |#|                   SISTEMAS DE ARQUIVOS                   |#|\n"

/* Declaração de Métodos */
void imprimirAguarde(void);


int main(int argc, char *argv[])
{

	setlocale(LC_ALL, "Portuguese");
	
    char recvBuff[BYTE];
    int tamBuff, skt;
    struct sockaddr_in serv;
    system("clear");

    /**Estrutura os campos do struct*/
    skt = socket(AF_INET, SOCK_STREAM, 6);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv.sin_port = htons (PORTA);
    memset(&(serv.sin_zero), 0x00, sizeof (serv.sin_zero));

    /**Inicializa a comunicação com o Servidor*/
    while(connect (skt, (struct sockaddr *)&serv, sizeof (struct sockaddr)) != 0){
        imprimirAguarde();      ///AGUARDA SERVIDOR SE COMUNICAR
    }
    printf("\n\n>> A Conexao com o Servidor '%s' foi estabelecida na porta:%d\n\n",SERVER_IP,PORTA);
    printf(">> Digite:");
	printf("\n\t\t   |  #  |     INSTRUCOES DE USO    |  #  |\n\t\t   | -h  |   Abrir      Menu        |  #  |\n\t\t   | sair|   Encerrar   Programa    |  #  |\n");
	


    /**Recebe Buffer do Servidor*/
    tamBuff = recv (skt, recvBuff, BYTE, 0);
    recvBuff[tamBuff] = 0x00;
    printf (">: %s\n",recvBuff);


    /**Loop de Comunicação entre Cliente e Servidor*/
    do{
        ///envia
        printf("> ");
        gets(recvBuff);
		if (strlen(recvBuff)==0) strcpy(recvBuff," ");
        send(skt, recvBuff, strlen(recvBuff), 0);

		if (strcmp(recvBuff,"sair")!= 0)
		{
			tamBuff = recv (skt, recvBuff, BYTE, 0);
			recvBuff[tamBuff] = 0x00;
			printf (">: %s\n",recvBuff);
		}
    }while(strcmp(recvBuff,"sair")!= 0);


    /**Finaliza a conexão! */
    close(skt);
    printf (">> A conexao com o Servidor foi Finalizada!!!\n\n");
	sleep(3);
    exit(0);
}

/* Função responsável por imprimir mensagem na tela enquanto aguarda algum servidor estabelecer comunicação */

void imprimirAguarde(){
    int i=0;
    char v[12] = "";
    for(i=0; i<4;i++){
        system("clear");
        printf(TITULO);
        printf("\n |#|                   Procurando servidor.                   |#|");
        printf("\n                            Aguarde %s                      \n", v);
		printf("\n |#|                                                          |#|\n");
        strcat(v,".");
        sleep(1);
    }
    strcpy(v, "");
	    

}