/****************************************************************************
-* Copyright © 2017 Alessio Greco
-* github: https://github.com/RootPOI
-*
-* This program is free software: you can redistribute it and/or modify
-* it under the terms of the GNU General Public License as published by
-* the Free Software Foundation, either version 3 of the License, or
-* (at your option) any later version.
-*
-* This program is distributed in the hope that it will be useful,
-* but WITHOUT ANY WARRANTY; without even the implied warranty of
-* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-* GNU General Public License for more details.
-*
-* You should have received a copy of the GNU General Public License
-* along with this program. If not, see <http://www.gnu.org/licenses/>.
-****************************************************************************/
/*----Header Funzioni Server----*/
/*------Header-----*/
#include "HeaderCHAT.h"
/*------Define-----*/
#define MAXCLIENT 3
/*--Test Funzioni--*/
void *ServerLogin (void *arg){
	parse *a = (parse*)arg;
	conn sconn;
	servertoclient stoc;
	int tmp = 0;
	char on_client[7] = "Online", stand_bycli[8] = "Standby";
	char user[5] = "User", moderator[10] = "Moderator";
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
	while (a->server_on){
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
		printf("%d\n", sconn.CLID);
		printf("%d\n", sconn.CLGRP);
		printf("%d\n", sconn.STAT);
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
/*------Ingresso in Sezione Critica-----------*/
		pthread_mutex_lock (a->gmem);
		printf("Stato Memoria = %d\n",a->som);
		if (a->som == MAXCLIENT){
			printf("Inviato BUSY\n");
			stoc.CMD = BUSY;
			write(socket_client, &stoc, sizeof(servertoclient));
		}
		for (;a->som < MAXCLIENT; a->som++){
			if (!a->connection[a->som].CLID){
				a->connection[a->som].SOCK = socket_client;
				stoc.CMD = CONNECTED;
				write (a->connection[a->som].SOCK, &stoc, sizeof(servertoclient));
				printf("Inviato CONNECTED\n");
				a->som++;
				break;
			}
		}
		pthread_mutex_unlock(a->gmem);
/*---------Uscita Sezione Critica-------------*/
	}
}
