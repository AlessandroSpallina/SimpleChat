/*Progetto CHAT, Gestione Server -- Greco Alessio -- UniCT -- */
/*----------------------*/
/*-------Header---------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include "HeaderCHAT.h"
/*---------Define--------*/
//#define MAXLENGTHMESSAGE 1024
#define MAXCLIENT 3
#define SERVER_ON 1
/*--Variabili Server--*/
char on_client[7] = "Online", stand_bycli[8] = "Standby";
char user[5] = "User", moderator[10] = "Moderator";
conn connection[MAXCLIENT], sconn;
int countcli, tmp;
servertoclient stoc;
/*-------Main---------*/
int main (void){
	printf("Avvio Server Chat in Corso...\n");
	sleep(1);
	printf("Avvio Socket in corso...\n");
	int socket_server, socket_client;
	if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Errore Creazione Socket\n");
		exit(EXIT_FAILURE);
	}
	printf("Definizione Strutture E Socket\n");
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	inet_aton(IPSERVER, &address.sin_addr);
	address.sin_port = htons(PORTLOGIN);
	printf("Porta Usata :%d, Indirizzo Usato %s\n", ntohs (address.sin_port), inet_ntoa(address.sin_addr));
	socklen_t lensock;
	lensock = (socklen_t)sizeof(address);
	printf("Avvio Bind in corso...\n");
	if (bind(socket_server, (struct sockaddr*)&address, lensock) == -1){
		printf("Errore Avvio Bind\n");
		exit(EXIT_FAILURE);
	}
	printf("Avvio Listen in corso...\n");
	if (listen(socket_server, 5) == -1){
		printf("Errore Avvio Listen\n");
		exit(EXIT_FAILURE);
	}
	printf("Avvio Accept in corso...\n");
/*------Server di Registrazione-----*/	
	while (SERVER_ON){
		socket_client = accept(socket_server, (struct sockaddr*)&address, &lensock);
		if (socket_client == -1){
			printf("Errore Avvio Bind\n");
			exit(EXIT_FAILURE);
		}
		printf("Accept Attiva, Ricezione Richiesta in corso...\n");
		if (read(socket_client, &sconn, sizeof(conn)) == -1){
			printf("Errore Ricezione\n");
			exit(EXIT_FAILURE);
		}
		printf("%d\n", sconn.CLGRP);
		switch (sconn.CLGRP){
			case USER: {
				switch (sconn.STAT){		
					case ONLINE:
						printf("Richiesta da...-> Id: %d, Gruppo: %s, Status: %s\n", sconn.CLID, user, on_client);
						break;
					case STAND_BY:
						printf("Richiesta da...-> Id: %d, Gruppo: %s, Status: %s\n", sconn.CLID, user, stand_bycli);
						break;
				}
				break;
			}
			case MODERATOR: {
				switch (sconn.STAT){
					case ONLINE:
						printf("Richiesta da...-> Id: %d, Gruppo: %s, Status: %s\n", sconn.CLID, moderator, on_client);
						break;
					case STAND_BY:
						printf("Richiesta da...-> Id: %d, Gruppo: %s, Status: %s\n", sconn.CLID, moderator, stand_bycli);
						break;
				}
			}
		};
		printf("Stato Memoria = %d\n",tmp);
		if (tmp == MAXCLIENT){
			printf("Inviato BUSY\n");
			stoc.CMD = BUSY;
			write(socket_client, &stoc, sizeof(servertoclient));
		}
		for (;tmp < MAXCLIENT; tmp++){
			if (!connection[tmp].CLID){
				connection[tmp].SOCK = socket_client;
				stoc.CMD = CONNECTED;
				write (connection[tmp].SOCK, &stoc, sizeof(servertoclient));
				printf("Inviato CONNECTED\n");
				tmp++;
				break;
			}
		}
		
	}
/*------------------------------------*/
}
