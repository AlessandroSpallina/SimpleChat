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
#include "Header.h"
/*--Test Funzioni--*/
void *ServerLogin (void *arg){
	parse *a = (parse*)arg;
	conn sconn;
	servertoclient stoc;
	int tmp = 0;
	char on_client[7] = "Online", stand_bycli[8] = "Standby";
	char user[5] = "User", moderator[10] = "Moderator";
	char psk[73] = "1806";
	printf("Avvio SocketLogin in corso...\n");
	int socket_server, socket_client;
	/*-------*/
	void AllOutLogin (){
	printf("[LOGIN]Ricevuto Segnale di Chiusura\n");
	if(close(socket_server) == -1){
		printf("[LOGIN]Errore Chiusura Socket Message\n");
		exit(EXIT_FAILURE);
	}
	printf("[LOGIN]Socket Chiusa, Terminazione Thread.\n");
	pthread_exit(NULL);
	}
	/*------*/
	signal(SIGPROF,AllOutLogin);
	strcat(psk, PSK);
	strcat(psk, "2016");
	//printf("%s\n", psk);
	if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Errore Creazione Socket\n");
		exit(EXIT_FAILURE);
	}
	printf("Definizione Strutture E Socket\n");
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	inet_aton(IPSERVER, &address.sin_addr);
	address.sin_port = htons(PORTLOGIN);
	printf("Server Login: Porta Usata: %d, Indirizzo Usato: %s\n", ntohs (address.sin_port), inet_ntoa(address.sin_addr));
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
			printf("Errore Avvio Accept\n");
			exit(EXIT_FAILURE);
		}
		printf("[LOGIN]Accept Attiva, Ricezione Richiesta in corso...\n");
		if (read(socket_client, &sconn, sizeof(conn)) == -1){
			printf("Errore Ricezione\n");
			exit(EXIT_FAILURE);
		}
		if (!strcmp(sconn.psk, psk)){
			switch (sconn.CLGRP){
				case USER: {
					switch (sconn.STAT){		
						case ONLINE:
							printf("[LOGIN]Richiesta da...-> Id: %d, Gruppo: %s, Status: %s\n", sconn.CLID, user, on_client);
							break;
						case STAND_BY:
							printf("[LOGIN]Richiesta da...-> Id: %d, Gruppo: %s, Status: %s\n", sconn.CLID, user, stand_bycli);
							break;
					}
					break;
				}
				case MODERATOR: {
					switch (sconn.STAT){
						case ONLINE:
							printf("[LOGIN]Richiesta da...-> Id: %d, Gruppo: %s, Status: %s\n", sconn.CLID, moderator, on_client);
							break;
						case STAND_BY:
							printf("[LOGIN]Richiesta da...-> Id: %d, Gruppo: %s, Status: %s\n", sconn.CLID, moderator, stand_bycli);
							break;
					}
				}
			}
/*------Ingresso in Sezione Critica-----------*/
				pthread_mutex_lock (a->gmem);		
				for (tmp = 0;tmp < MAXCLIENT; tmp++){
					if (!a->connection[tmp].CLID){
					a->connection[tmp].CLID = sconn.CLID;
					stoc.CMD = CONNECTED;
					write (socket_client, &stoc, sizeof(servertoclient));
					printf("[LOGIN]Inviato CONNECTED\n");
					break;
					}
				}
				if (tmp == MAXCLIENT){
					printf("[LOGIN]Inviato BUSY\n");
					stoc.CMD = BUSY;
					write(socket_client, &stoc, sizeof(servertoclient));
				}
				pthread_mutex_unlock(a->gmem);
/*---------Uscita Sezione Critica-------------*/	
			}
			else {
				printf("[LOGIN]PSK ERRATA, Chiusura Socket\n");
				
				close(socket_client);
			}
			}
		}
		
void *ThreadChat (void *arg){
	parth *paramthread = (parth*)arg;
	servertoclient msg;
	int count, on = 1;
	int sockclientnew = paramthread->socket_clientchat;
	printf("[TCL]Attivo, in Ricezione\n");
	while (on){
	if(read(sockclientnew, &msg, sizeof(servertoclient)) == -1){
			printf("[TCL]Errore Ricezione Msg\n");
			exit(EXIT_FAILURE);
	}
		switch (msg.CMD){
			case EXIT: {
				/*--Reg.Critica--*/
				pthread_mutex_lock(paramthread->gmem);
				for (count = 0; count < MAXCLIENT; count++){
					if (paramthread->connection[count].CLID == msg.MSGSTOC.CLID) {
							paramthread->connection[count].CLID = 0;
							paramthread->connection[count].SOCK = 0;
							break;
					}	
				}
				for (count = 0; count < MAXROOM; count++){
					for (int count2 = 0; count2 < MAXUROOM; count2++){
						if (paramthread->stanza[count].userin[count2] == msg.MSGSTOC.CLID){
							//printf("[TLC]Eliminato dalla stanza %s\n", paramthread->stanza[count].roomname);
							paramthread->stanza[count].userin[count2] = 0;
							break;
						}
					}
				}
				pthread_mutex_unlock(paramthread->gmem);
				/*--Reg.Critica Off--*/
				if (close(sockclientnew) == -1){
					printf("[TLC]Errore Chiusura Client Utente\n");
					exit(EXIT_FAILURE);
				}
				printf("[TLC]Client: %d chiuso\n", msg.MSGSTOC.CLID);
				on = 0;
				break;
			}
			case LISTUSER:{
				int tmp[MAXCLIENT];
				pthread_mutex_lock(paramthread->gmem);
				for (count = 0; count < MAXCLIENT; count++){
					tmp[count] = paramthread->connection[count].CLID;
					//printf("[TLC]Id Presente: %d\n",tmp[count]);
				}
				if (write (sockclientnew, tmp, sizeof(tmp)) == -1){
							printf("[TLC]Errore Invio Informazioni\n");
							exit(EXIT_FAILURE);
				}				
				pthread_mutex_unlock(paramthread->gmem);
				break;
			}
			case NEWROOM: {
				printf("[TLC]Creazione Stanza: %s da parte di %d\n", msg.stanza.roomname, msg.MSGSTOC.CLID);
				pthread_mutex_lock(paramthread->gmem);
				for (count = 0; count < MAXROOM; count++){
					if (!paramthread->stanza[count].CLID){
						strcpy(paramthread->stanza[count].roomname, msg.stanza.roomname);
						paramthread->stanza[count].CLID = msg.MSGSTOC.CLID;
						msg.CMD = ROOMOK;
						write(sockclientnew, &msg, sizeof(servertoclient));
						printf("[TLC]Stanza Creata\n");
						break;
					}
				}
				if (count == MAXROOM){
					msg.CMD = BUSY;
					printf("[TLC]Memoria Stanze Piena, impossibile creare ulteriori stanze\n");
					write(sockclientnew, &msg, sizeof(servertoclient));
				}
				pthread_mutex_unlock(paramthread->gmem);
				break;
			}
			case ROOMLIST: {
				room lista[MAXROOM];
				pthread_mutex_lock(paramthread->gmem);
				for (count = 0; count < MAXROOM; count++){
					if (paramthread->stanza[count].CLID){
						printf("%s %d\n", paramthread->stanza[count].roomname,paramthread->stanza[count].CLID);
						lista[count].CLID = paramthread->stanza[count].CLID;
						strcpy(lista[count].roomname, paramthread->stanza[count].roomname);
					}
					else lista[count].CLID = 0;
				}
				pthread_mutex_unlock(paramthread->gmem);
				write(sockclientnew, &lista, sizeof(lista));
				printf("[TLS]Risposta mandata\n");
				break;
			}
			case ENTERROOM: {
				int i;
				pthread_mutex_lock(paramthread->gmem);
				for (count = 0; count < MAXROOM; count++){
					if (!strcmp(paramthread->stanza[count].roomname, msg.stanza.roomname)){
						for (i = 0; i < MAXUROOM; i++){
							if(paramthread->stanza[count].userin[i] == msg.MSGSTOC.CLID){
								printf("[TLC]Utente già registrato nella stanza\n");
								msg.CMD = ERRORROOM;
								write(sockclientnew, &msg, sizeof(servertoclient));
								break;
							}
						}
						if (i == MAXUROOM){
							printf("[TLC]Inserimento utente %d, in %s\n", msg.MSGSTOC.CLID, msg.stanza.roomname);
							for (i = 0; i < MAXUROOM; i++){
								if (!paramthread->stanza[count].userin[i]){
									printf("[TLC]Utente Inserito\n");
									paramthread->stanza[count].userin[i] = msg.MSGSTOC.CLID;
									msg.CMD = ROOMOK;
									write (sockclientnew, &msg, sizeof(servertoclient));
									break;
								}
							}
							if (i == MAXUROOM){
								printf("[TLC]Stanza Piena\n");
								msg.CMD = BUSY;
								write (sockclientnew, &msg, sizeof(servertoclient));
								break;
							}
						}
						break;
					}
				}
				pthread_mutex_unlock(paramthread->gmem);
				if (count == MAXROOM){
					printf("[TLC]La Stanza Non Esiste\n");
					msg.CMD = ERRORROOM2;
					write(sockclientnew, &msg, sizeof(servertoclient));
				}
				break;
			}
			case ERASEROOM: {
				pthread_mutex_lock(paramthread->gmem);
				for (count = 0; count < MAXROOM; count++){
					if (!strcmp(paramthread->stanza[count].roomname, msg.stanza.roomname)){
						switch (msg.MSGSTOC.CLGRP){
							case USER:{
								if (paramthread->stanza[count].CLID == msg.MSGSTOC.CLID){
									printf("[TLC]%d e' il creatore, cancellazione stanza: %s\n",msg.MSGSTOC.CLID, msg.stanza.roomname);
									paramthread->stanza[count].CLID = 0;
									strcpy(paramthread->stanza[count].roomname, "\0");
									msg.CMD = ROOMOK;
									write(sockclientnew, &msg, sizeof(servertoclient));
								}
								else {
									msg.CMD = ERRORROOM;
									write(sockclientnew, &msg, sizeof(servertoclient));
								}
								break;
							}
							case MODERATOR:{
								printf("[TLC]MOD:%d, Cancellazione Stanza: %s\n",msg.MSGSTOC.CLID, msg.stanza.roomname);
								paramthread->stanza[count].CLID = 0;
								strcpy(paramthread->stanza[count].roomname, "\0");
								msg.CMD = ROOMOK;
								write(sockclientnew, &msg, sizeof(servertoclient));
								break;
							}
						}
					}
				}
				pthread_mutex_unlock(paramthread->gmem);
				break;
			}
		}
	};
}

void *ServerChat (void *arg){
	parse *b = (parse*)arg;
	pthread_t thclient;
	parth paramthread;
	printf("Avvio SocketChat in corso...\n");
	int socket_serverchat, socket_clientchat;
	/*-------*/
	void AllOutChat (){
	printf("[CHAT]Ricevuto Segnale di Chiusura\n");
	if(close(socket_serverchat) == -1){
		printf("[CHAT]Errore Chiusura Socket Chat\n");
		exit(EXIT_FAILURE);
	}
	printf("[CHAT]Socket Chiusa, Terminazione Thread.\n");
	pthread_exit(NULL);
	}
	/*------*/
	signal(SIGUSR2,AllOutChat);
	if ((socket_serverchat = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Errore Creazione SocketChat\n");
		exit(EXIT_FAILURE);
	}
	printf("Definizione Strutture E SocketChat\n");
	struct sockaddr_in addresschat;
	addresschat.sin_family = AF_INET;
	inet_aton(IPSERVER, &addresschat.sin_addr);
	addresschat.sin_port = htons(PORTCHAT);
	printf("Server Chat: Porta Usata: %d, Indirizzo Usato: %s\n", ntohs(addresschat.sin_port), inet_ntoa(addresschat.sin_addr));
	socklen_t lensockchat;
	lensockchat = (socklen_t)sizeof(addresschat);
	printf("Avvio BindChat in corso...\n");
	if (bind(socket_serverchat, (struct sockaddr*)&addresschat, lensockchat) == -1){
		printf("Errore Avvio BindChat\n");
		exit(EXIT_FAILURE);
	}
	printf("Avvio ListenChat in corso...\n");
	if (listen(socket_serverchat, 5) == -1){
		printf("Errore Avvio ListenChat\n");
		exit(EXIT_FAILURE);
	}
	printf("Avvio AcceptChat in corso...\n");
	while(b->server_on){
		socket_clientchat = accept(socket_serverchat, (struct sockaddr*)&addresschat, &lensockchat);
		if (socket_clientchat == -1){
			printf("Errore Avvio Accept\n");
			exit(EXIT_FAILURE);
		}
		printf("[CHAT]Accept Attiva, Creazione nuovo Thread Dedicato in corso...\n");
		paramthread.gmem = b->gmem;
		paramthread.socket_clientchat = socket_clientchat;
		paramthread.connection = b->connection;
		paramthread.stanza = b->stanza;
		if (pthread_create(&thclient, NULL, ThreadChat, (void*)&paramthread) == -1){
			printf("[CHAT]Errore Creazione ThreadClient\n");
			exit(EXIT_FAILURE);
		}
	}
}	

void *ThreadMessage (void *arg){
	parth *param = (parth*)arg;
	servertoclient message;
	int socketmessage = param->socket_clientchat;
	read (socketmessage, &message, sizeof(servertoclient));
	//printf("[THMESS]ID %d Sock %d\n",message.MSGSTOC.CLID, socketmessage);
	int count, tmp, on = 1;
	pthread_mutex_lock(param->gmem);
	for (count = 0; count < MAXCLIENT; count++){
			if(param->connection[count].CLID == message.MSGSTOC.CLID){
				param->connection[count].SOCK = socketmessage;
				//printf("[THMESS FOR]id %d sock %d\n",param->connection[count].CLID, param->connection[count].SOCK);
				pthread_mutex_unlock(param->gmem);
				break;
			}
		}
	while(on){
		read (socketmessage, &message, sizeof(servertoclient));
		switch (message.CMD){
			case EXIT:{
				if (close(socketmessage) == -1){
					printf("Errore Chiusura Socket Client \n");
				}
			on = 0;
			break;
			}
			case PRIVATE:{
				printf("[MESSAGE] from %d to %d: %s\n", message.MSGSTOC.CLID, message.MSGSTOC.MSGTOID, message.MSGSTOC.message);
				pthread_mutex_lock (param->gmem);
				for(count = 0; count < MAXCLIENT; count++){
					if (param->connection[count].CLID == message.MSGSTOC.MSGTOID){
						//printf("[SOCK]%d\n", param->connection[count].SOCK);
						tmp = param->connection[count].SOCK;
						//printf("[MESSAGE] Sock %d\n", tmp);
						write(tmp, &message, sizeof(servertoclient));
						//printf("Messaggio spedito a %d\n", tmp);
						break;
					}
				}
				pthread_mutex_unlock(param->gmem);
				if (count == MAXCLIENT){
					printf("[MESSAGE]Destinatario non esistente\n");
					message.CMD = OFFLINE;
					write (socketmessage, &message, sizeof(servertoclient));
				}
				break;
			}
			case PUBLIC:{
				printf("[MESSAGE]<PUBLIC> from %d: %s\n", message.MSGSTOC.CLID, message.MSGSTOC.message);
				pthread_mutex_lock (param->gmem);
				for(count = 0; count < MAXCLIENT; count++){
					if (param->connection[count].CLID){
						tmp = param->connection[count].SOCK;
						//printf("[SOCK]%d\n", param->connection[count].SOCK);
						write(tmp, &message, sizeof(servertoclient));
						//printf("Messaggio spedito a %d\n", tmp);
					}
				}
				pthread_mutex_unlock(param->gmem);
				break;
			}
			case MESSAGETOROOM:{
				int i,b,c,d = 0,tmp;
				pthread_mutex_lock(param->gmem);
				for (count = 0; count < MAXROOM; count++){
					if (!strcmp(param->stanza[count].roomname, message.stanza.roomname)){
						d = 1;
						for (i = 0; i < MAXUROOM; i++){
							if(param->stanza[count].userin[i] == message.MSGSTOC.CLID){
								for (b = 0; b < MAXUROOM; b++){
									if (param->stanza[count].userin[b]){
										for (c = 0; c < MAXCLIENT; c++){
											if (param->connection[c].CLID == param->stanza[count].userin[b]){
												printf("[MESSAGE]<%s>Spedisco il messagio a %d\n", message.stanza.roomname, param->connection[c].CLID);
												tmp = param->connection[c].SOCK;
												write (tmp, &message, sizeof(servertoclient));
												c = MAXCLIENT;
											}
										}
									}
								}
								pthread_mutex_unlock(param->gmem);
								break;
							}
						}
						if (i == MAXUROOM){
							printf("[MESSAGE]Utente %d Non Registrato alla stanza\n", message.MSGSTOC.CLID);
							message.CMD = ERRORROOM;
							write(socketmessage, &message, sizeof(servertoclient));
							break;
						}
						break;
					}
				}
				pthread_mutex_unlock(param->gmem);
				if (count == MAXROOM){
					printf("[MESSAGE]La Stanza Non Esiste\n");
					message.CMD = ERRORROOM2;
					write(socketmessage, &message, sizeof(servertoclient));
					break;
				}
				break;
			}
		}
	}
}

void *ServerMessage (void *arg){
	parse *c = (parse*)arg;
	parth paramthread;
	pthread_t threadmessage;
	servertoclient tomex2;
	printf("Avvio SocketMessage in corso...\n");
	int socket_message, socket_clientmessage, co;
	/*-------*/
	void AllOutMessage (){
	printf("[MESSAGE]Ricevuto Segnale di Chiusura\n");
	if(close(socket_message) == -1){
		printf("[MESSAGE]Errore Chiusura Socket Message\n");
		exit(EXIT_FAILURE);
	}
	printf("[MESSAGE]Socket Chiusa, Terminazione Thread.\n");
	pthread_exit(NULL);
	}
	/*------*/
	signal(SIGUSR1,AllOutMessage);
	if ((socket_message = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Errore Creazione SocketChat\n");
		exit(EXIT_FAILURE);
	}
	printf("Definizione Strutture E SocketChat\n");
	struct sockaddr_in addresschat;
	addresschat.sin_family = AF_INET;
	inet_aton(IPSERVER, &addresschat.sin_addr);
	addresschat.sin_port = htons(PORTMESSAGE);
	printf("Server Message: Porta Usata: %d, Indirizzo Usato: %s\n", ntohs(addresschat.sin_port), inet_ntoa(addresschat.sin_addr));
	socklen_t lensockchat;
	lensockchat = (socklen_t)sizeof(addresschat);
	printf("Avvio BindMessage in corso...\n");
	if (bind(socket_message, (struct sockaddr*)&addresschat, lensockchat) == -1){
		printf("Errore Avvio BindMessage\n");
		exit(EXIT_FAILURE);
	}
	printf("Avvio ListenMessage in corso...\n");
	if (listen(socket_message, 5) == -1){
		printf("Errore Avvio ListenMessage\n");
		exit(EXIT_FAILURE);
	}
	printf("Avvio AcceptMessage in corso...\n");
	while(c->server_on){
		socket_clientmessage = accept(socket_message, (struct sockaddr*)&addresschat, &lensockchat);
		if (socket_clientmessage == -1){
			printf("Errore Avvio AcceptMessage\n");
			exit(EXIT_FAILURE);
		}
		
		printf("[MESSAGE] Attivo\n");
		paramthread.gmem = c->gmem;
		paramthread.socket_clientchat = socket_clientmessage;
		paramthread.connection = c->connection;
		paramthread.stanza = c->stanza;
		if (pthread_create(&threadmessage, NULL, ThreadMessage, (void*)&paramthread) == -1){
			printf("[CHAT]Errore Creazione ThreadClient\n");
			exit(EXIT_FAILURE);
		}
	}
}
