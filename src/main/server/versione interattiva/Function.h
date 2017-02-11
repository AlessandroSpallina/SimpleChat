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
/*----------------------------Function Server-------------------------------*/
#include "Header.h"

const char* Hours (void){
	time_t now;
	int i = 11,c;
	char tmp[25];
	char *nowtime = (char*)malloc(sizeof(char)*25);
	now = time(NULL);
	strcpy(tmp, ctime(&now));
	for (c = 0; c < 8; c++){
			nowtime[c] = tmp[i];
			i++;
	}
	nowtime[8]= '\0';
	return nowtime;	
}

void *ServerLogin (void *arg){
	parse *a = (parse*)arg;
	conn sconn;
	servertoclient stoc;
	int tmp = 0;
	char on_client[7] = "Online", stand_bycli[8] = "Standby";
	char user[5] = "User", moderator[10] = "Moderator";
	char psk[73] = "1806";
	int socket_server, socket_client;
	time_t forfile;
	FILE *filelog;
	forfile = time(NULL);
	char nmf[30];
	strcpy(nmf,ctime(&forfile));
	strcat(nmf, "-sl");
	filelog = fopen(nmf, "a");
	/*-------*/
	void AllOutLogin (){
	printf("[%s,LOGIN]Ricevuto Segnale di Chiusura\n", Hours());
	fprintf(filelog,"[%s,LOGIN]Ricevuto Segnale di Chiusura\n", Hours());
	if(close(socket_server) == -1){
		printf("[%s,LOGIN]Errore Chiusura Socket Message\n", Hours());
		fprintf(filelog,"[%s,LOGIN]Errore Chiusura Socket Message\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	printf("[%s,LOGIN]Socket Chiusa, Terminazione Thread.\n", Hours());
	fprintf(filelog,"[%s,LOGIN]Socket Chiusa, Terminazione Thread.\n", Hours());
	fclose(filelog);
	pthread_exit(NULL);
	}
	/*------*/
	signal(SIGPROF,AllOutLogin);
	strcat(psk, PSK);
	strcat(psk, "2016");
	fprintf(filelog,"Log SERVERCHAT ThreadServerLogin: %s\n",ctime(&forfile));
	if (TURNOFF) printf("[%s,LOGIN]Avvio SocketLogin\n", Hours());
	fprintf(filelog,"[%s,LOGIN]Avvio SocketLogin\n", Hours());
	if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		if (TURNOFF) printf("[%s,LOGIN]Errore Creazione Socket\n", Hours());
		fprintf(filelog,"[%s,LOGIN]Errore Creazione Socket\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	inet_aton(IPSERVER, &address.sin_addr);
	address.sin_port = htons(PORTLOGIN);
	if (TURNOFF) printf("[%s,LOGIN]: Porta Usata: %d, Indirizzo Usato: %s\n", Hours(), ntohs (address.sin_port), inet_ntoa(address.sin_addr));
	fprintf(filelog,"[%s,LOGIN]: Porta Usata: %d, Indirizzo Usato: %s\n", Hours(), ntohs (address.sin_port), inet_ntoa(address.sin_addr));
	socklen_t lensock;
	lensock = (socklen_t)sizeof(address);
	if (bind(socket_server, (struct sockaddr*)&address, lensock) == -1){
		if (TURNOFF) printf("[%s,LOGIN]Errore Avvio Bind\n", Hours());
		fprintf(filelog,"[%s,LOGIN]Errore Avvio Bind\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	if (listen(socket_server, 5) == -1){
		if (TURNOFF) printf("[%s,LOGIN]Errore Avvio Listen\n", Hours());
		fprintf(filelog,"[%s,LOGIN]Errore Avvio Listen\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	if (TURNOFF) printf("[%s,LOGIN]In Ricezione\n", Hours());
	fprintf(filelog,"[%s,LOGIN]In Ricezione\n", Hours());
/*------Server di Registrazione-----*/	
	while (a->server_on){
		socket_client = accept(socket_server, (struct sockaddr*)&address, &lensock);
		if (socket_client == -1){
			printf("[%s,LOGIN]Errore Avvio Accept\n", Hours());
			fprintf(filelog,"[%s,LOGIN]Errore Avvio Accept\n", Hours());
			fclose(filelog);
			exit(EXIT_FAILURE);
		}
		printf("[%s,LOGIN]Ricezione Richiesta in corso...\n", Hours());
		fprintf(filelog,"[%s,LOGIN]Ricezione Richiesta in corso...\n", Hours());
		if (read(socket_client, &sconn, sizeof(conn)) == -1){
			printf("[%s,LOGIN]Errore Ricezione\n", Hours());
			fprintf(filelog,"[%s,LOGIN]Errore Ricezione\n", Hours());
			fclose(filelog);
			exit(EXIT_FAILURE);
		}
		if (!strcmp(sconn.psk, psk)){
			switch (sconn.CLGRP){
				case USER: {
					switch (sconn.STAT){		
						case ONLINE:
							printf("[%s,LOGIN]Richiesta -> Id: %d, Gruppo: %s, Status: %s\n", Hours(), sconn.CLID, user, on_client);
							fprintf(filelog,"[%s,LOGIN]Richiesta -> Id: %d, Gruppo: %s, Status: %s\n", Hours(), sconn.CLID, user, on_client);
							break;
						case STAND_BY:
							printf("[%s,LOGIN]Richiesta -> Id: %d, Gruppo: %s, Status: %s\n", Hours(), sconn.CLID, user, stand_bycli);
							fprintf(filelog,"[%s,LOGIN]Richiesta -> Id: %d, Gruppo: %s, Status: %s\n", Hours(), sconn.CLID, user, stand_bycli);
							break;
					}
					break;
				}
				case MODERATOR: {
					switch (sconn.STAT){
						case ONLINE:
							printf("[%s,LOGIN]Richiesta da -> Id: %d, Gruppo: %s, Status: %s\n", Hours(), sconn.CLID, moderator, on_client);
							fprintf(filelog,"[%s,LOGIN]Richiesta da -> Id: %d, Gruppo: %s, Status: %s\n", Hours(), sconn.CLID, moderator, on_client);
							break;
						case STAND_BY:
							printf("[%s,LOGIN]Richiesta da -> Id: %d, Gruppo: %s, Status: %s\n", Hours(), sconn.CLID, moderator, stand_bycli);
							fprintf(filelog,"[%s,LOGIN]Richiesta da -> Id: %d, Gruppo: %s, Status: %s\n", Hours(), sconn.CLID, moderator, stand_bycli);
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
					printf("[%s,LOGIN]%d CONNECTED\n", Hours(), sconn.CLID);
					fprintf(filelog,"[%s,LOGIN]%d CONNECTED\n", Hours(), sconn.CLID);
					break;
					}
				}
				if (tmp == MAXCLIENT){
					printf("[%s,LOGIN]%d BUSY\n", Hours(), sconn.CLID);
					fprintf(filelog,"[%s,LOGIN]%d BUSY\n", Hours(), sconn.CLID);
					stoc.CMD = BUSY;
					write(socket_client, &stoc, sizeof(servertoclient));
				}
				pthread_mutex_unlock(a->gmem);
/*---------Uscita Sezione Critica-------------*/	
			}
			else {
				printf("[%s,LOGIN]PSK ERRATA\n", Hours());
				fprintf(filelog,"[%s,LOGIN]PSK ERRATA\n", Hours());
				fclose(filelog);
				close(socket_client);
			}
			}
		}
		
void *ThreadChat (void *arg){
	parth *paramthread = (parth*)arg;
	servertoclient msg;
	int count, on = 1;
	int sockclientnew = paramthread->socket_clientchat;
	//printf("[%s,TCHAT]Attivo, in Ricezione\n");
	while (on){
	if(read(sockclientnew, &msg, sizeof(servertoclient)) == -1){
			printf("[%s,TCHAT]Errore Ricezione Msg\n", Hours());
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
							//printf("[%s,TCHAT]Eliminato dalla stanza %s\n", Hours(), paramthread->stanza[count].roomname);
							paramthread->stanza[count].userin[count2] = 0;
							break;
						}
					}
				}
				pthread_mutex_unlock(paramthread->gmem);
				/*--Reg.Critica Off--*/
				if (close(sockclientnew) == -1){
					printf("[%s,TCHAT]Errore Chiusura Client Utente\n", Hours());
					exit(EXIT_FAILURE);
				}
				printf("[%s,TCHAT]Client: %d chiuso\n", Hours(), msg.MSGSTOC.CLID);
				on = 0;
				break;
			}
			case LISTUSER:{
				int tmp[MAXCLIENT];
				pthread_mutex_lock(paramthread->gmem);
				for (count = 0; count < MAXCLIENT; count++){
					tmp[count] = paramthread->connection[count].CLID;
					//printf("[%s,TCHAT]Id Presente: %d\n", Hours(),tmp[count]);
				}
				if (write (sockclientnew, tmp, sizeof(tmp)) == -1){
						printf("[%s,TCHAT]Errore Invio Informazioni\n", Hours());
						exit(EXIT_FAILURE);
				}				
				pthread_mutex_unlock(paramthread->gmem);
				break;
			}
			case NEWROOM: {
				printf("[%s,TCHAT]Creazione Stanza: %s Owner: %d\n", Hours(), msg.stanza.roomname, msg.MSGSTOC.CLID);
				pthread_mutex_lock(paramthread->gmem);
				for (count = 0; count < MAXROOM; count++){
					if (!paramthread->stanza[count].CLID){
						strcpy(paramthread->stanza[count].roomname, msg.stanza.roomname);
						paramthread->stanza[count].CLID = msg.MSGSTOC.CLID;
						msg.CMD = ROOMOK;
						write(sockclientnew, &msg, sizeof(servertoclient));
						//printf("[%s,TLCHAT]Stanza Creata\n", Hours());
						break;
					}
				}
				if (count == MAXROOM){
					msg.CMD = BUSY;
					printf("[%s,TCHAT]Memoria Stanze Piena, impossibile creare ulteriori stanze\n", Hours());
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
						//printf("%s %d\n", paramthread->stanza[count].roomname,paramthread->stanza[count].CLID);
						lista[count].CLID = paramthread->stanza[count].CLID;
						strcpy(lista[count].roomname, paramthread->stanza[count].roomname);
					}
					else lista[count].CLID = 0;
				}
				pthread_mutex_unlock(paramthread->gmem);
				write(sockclientnew, &lista, sizeof(lista));
				break;
			}
			case ENTERROOM: {
				int i;
				pthread_mutex_lock(paramthread->gmem);
				for (count = 0; count < MAXROOM; count++){
					if (!strcmp(paramthread->stanza[count].roomname, msg.stanza.roomname)){
						for (i = 0; i < MAXUROOM; i++){
							if(paramthread->stanza[count].userin[i] == msg.MSGSTOC.CLID){
								//printf("[%s,TCHAT]Utente già registrato nella stanza\n", Hours());
								msg.CMD = ERRORROOM;
								write(sockclientnew, &msg, sizeof(servertoclient));
								break;
							}
						}
						if (i == MAXUROOM){
							printf("[%s,TCHAT]Inserimento utente %d -> %s\n", Hours(), msg.MSGSTOC.CLID, msg.stanza.roomname);
							for (i = 0; i < MAXUROOM; i++){
								if (!paramthread->stanza[count].userin[i]){
									//printf("[%s,TCHAT]Utente Inserito\n", Hours());
									paramthread->stanza[count].userin[i] = msg.MSGSTOC.CLID;
									msg.CMD = ROOMOK;
									write (sockclientnew, &msg, sizeof(servertoclient));
									break;
								}
							}
							if (i == MAXUROOM){
								//printf("[%s,TCHAT]Stanza Piena\n", Hours());
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
					//printf("[%s,TCHAT]La Stanza Non Esiste\n", Hours());
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
									printf("[%s,TCHAT]%d Owner OK, Cancellazione: %s\n", Hours(),msg.MSGSTOC.CLID, msg.stanza.roomname);
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
								printf("[%s,TCHAT]MOD:%d, Cancellazione Stanza: %s\n", Hours(),msg.MSGSTOC.CLID, msg.stanza.roomname);
								paramthread->stanza[count].CLID = 0;
								strcpy(paramthread->stanza[count].roomname, "\0");
								msg.CMD = ROOMOK;
								write(sockclientnew, &msg, sizeof(servertoclient));
								break;
							}
						}
						break;
					}
				}
				pthread_mutex_unlock(paramthread->gmem);
				if (count == MAXROOM){
					msg.CMD = ERRORROOM2;
					//printf("[%s,TCHAT]La stanza non esiste\n", Hours());
					write(sockclientnew, &msg, sizeof(servertoclient));
				}
				break;
			}
			case EXITROOM: {
				int i;
				pthread_mutex_lock(paramthread->gmem);
				for (count = 0; count < MAXROOM; count++){
					if (!strcmp(paramthread->stanza[count].roomname, msg.stanza.roomname)){
						for (i = 0; i < MAXUROOM; i++){
							if(paramthread->stanza[count].userin[i] == msg.MSGSTOC.CLID){
								paramthread->stanza[count].userin[i] = 0;
								msg.CMD = ROOMOK;
								printf("[%s,TCHAT]:%d, Rimosso da: %s\n", Hours(),msg.MSGSTOC.CLID, msg.stanza.roomname);
								write(sockclientnew, &msg, sizeof(servertoclient));
								break;
							}
						}
						break;
					}
				}
				pthread_mutex_unlock(paramthread->gmem);
				if (count == MAXROOM){
					msg.CMD = ERRORROOM2;
					write(sockclientnew, &msg, sizeof(servertoclient));
				}
				break;
			}
		};
	}
}

void *ServerChat (void *arg){
	parse *b = (parse*)arg;
	pthread_t thclient;
	parth paramthread;
	int socket_serverchat, socket_clientchat;
	time_t forfile;
	FILE *filelog;
	forfile = time(NULL);
	char nmf[30];
	strcpy(nmf,ctime(&forfile));
	strcat(nmf, "-sc");
	filelog = fopen(nmf, "a");
	/*-------*/
	void AllOutChat (){
	printf("[%s,CHAT]Ricevuto Segnale di Chiusura\n", Hours());
	fprintf(filelog,"[%s,CHAT]Ricevuto Segnale di Chiusura\n", Hours());
	if(close(socket_serverchat) == -1){
		printf("[%s,CHAT]Errore Chiusura Socket Chat\n", Hours());
		fprintf(filelog,"[%s,CHAT]Errore Chiusura Socket Chat\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	printf("[%s,CHAT]Socket Chiusa, Terminazione Thread.\n", Hours());
	fprintf(filelog,"[%s,CHAT]Socket Chiusa, Terminazione Thread.\n", Hours());
	fclose(filelog);
	pthread_exit(NULL);
	}
	/*------*/
	signal(SIGUSR2,AllOutChat);
	if (TURNOFF) printf("[%s,CHAT]Avvio SocketChat\n", Hours());
	fprintf(filelog,"[%s,CHAT]Avvio SocketChat\n", Hours());
	if ((socket_serverchat = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		if (TURNOFF) printf("[%s,CHAT]Errore Creazione SocketChat\n", Hours());
		fprintf(filelog,"[%s,CHAT]Errore Creazione SocketChat\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in addresschat;
	addresschat.sin_family = AF_INET;
	inet_aton(IPSERVER, &addresschat.sin_addr);
	addresschat.sin_port = htons(PORTCHAT);
	if (TURNOFF) printf("[%s,CHAT]: Porta Usata: %d, Indirizzo Usato: %s\n", Hours(), ntohs(addresschat.sin_port), inet_ntoa(addresschat.sin_addr));
	fprintf(filelog,"[%s,CHAT]: Porta Usata: %d, Indirizzo Usato: %s\n", Hours(), ntohs(addresschat.sin_port), inet_ntoa(addresschat.sin_addr));
	socklen_t lensockchat;
	lensockchat = (socklen_t)sizeof(addresschat);
	if (bind(socket_serverchat, (struct sockaddr*)&addresschat, lensockchat) == -1){
		if (TURNOFF) printf("[%s,CHAT]Errore Avvio BindChat\n", Hours());
		fprintf(filelog,"[%s,CHAT]Errore Avvio BindChat\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	if (listen(socket_serverchat, 5) == -1){
		if (TURNOFF) printf("[%s,CHAT]Errore Avvio ListenChat\n", Hours());
		fprintf(filelog,"[%s,CHAT]Errore Avvio ListenChat\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	if (TURNOFF) printf("[%s,CHAT]In Ricezione\n", Hours());
	fprintf(filelog,"[%s,CHAT]In Ricezione\n", Hours());
	while(b->server_on){
		socket_clientchat = accept(socket_serverchat, (struct sockaddr*)&addresschat, &lensockchat);
		if (socket_clientchat == -1){
			printf("[%s,CHAT]Errore Avvio Accept\n", Hours());
			fprintf(filelog,"[%s,CHAT]Errore Avvio Accept\n", Hours());
			fclose(filelog);
			exit(EXIT_FAILURE);
		}
		printf("[%s,CHAT]Accept Attiva, Creazione nuovo Thread Dedicato in corso...\n", Hours());
		fprintf(filelog,"[%s,CHAT]Accept Attiva, Creazione nuovo Thread Dedicato in corso...\n", Hours());
		paramthread.gmem = b->gmem;
		paramthread.socket_clientchat = socket_clientchat;
		paramthread.connection = b->connection;
		paramthread.stanza = b->stanza;
		if (pthread_create(&thclient, NULL, ThreadChat, (void*)&paramthread) == -1){
			printf("[%s,CHAT]Errore Creazione ThreadClient\n", Hours());
			fprintf(filelog,"[%s,CHAT]Errore Creazione ThreadClient\n", Hours());
			fclose(filelog);
			exit(EXIT_FAILURE);
		}
	}
}	

void *ThreadMessage (void *arg){
	parth *param = (parth*)arg;
	servertoclient message;
	int socketmessage = param->socket_clientchat;
	read (socketmessage, &message, sizeof(servertoclient));
	//printf("[%s,MESSAGE]ID %d Sock %d\n",Hours() ,message.MSGSTOC.CLID, socketmessage);
	int count, tmp, on = 1;
	pthread_mutex_lock(param->gmem);
	for (count = 0; count < MAXCLIENT; count++){
			if(param->connection[count].CLID == message.MSGSTOC.CLID){
				param->connection[count].SOCK = socketmessage;
				//printf("[%s,MESSAGE]id %d sock %d\n",Hours(),param->connection[count].CLID, param->connection[count].SOCK);
				pthread_mutex_unlock(param->gmem);
				break;
			}
		}
	while(on){
		read (socketmessage, &message, sizeof(servertoclient));
		switch (message.CMD){
			case EXIT:{
				if (close(socketmessage) == -1){
					printf("[%s,MESSAGE]Errore Chiusura Socket Client \n",Hours());
				}
			on = 0;
			break;
			}
			case PRIVATE:{
				//printf("[%s,MESSAGE] from %d -> %d: %s\n",Hours(), message.MSGSTOC.CLID, message.MSGSTOC.MSGTOID, message.MSGSTOC.message);
				pthread_mutex_lock (param->gmem);
				for(count = 0; count < MAXCLIENT; count++){
					if (param->connection[count].CLID == message.MSGSTOC.MSGTOID){
						//printf("[%s,MESSAGE]%d\n",Hours(), param->connection[count].SOCK);
						tmp = param->connection[count].SOCK;
						//printf("[%s,MESSAGE] Sock %d\n",Hours(), tmp);
						write(tmp, &message, sizeof(servertoclient));
						break;
					}
				}
				pthread_mutex_unlock(param->gmem);
				if (count == MAXCLIENT){
					//printf("[%s,MESSAGE]Destinatario non esistente\n",Hours());
					message.CMD = OFFLINE;
					write (socketmessage, &message, sizeof(servertoclient));
				}
				break;
			}
			case PUBLIC:{
				switch (message.MSGSTOC.CLGRP){
					case USER:{
						printf("[%s,MESSAGE]<PUBLIC> from %d: %s\n",Hours(), message.MSGSTOC.CLID, message.MSGSTOC.message);
						pthread_mutex_lock (param->gmem);
						for(count = 0; count < MAXCLIENT; count++){
							if (param->connection[count].CLID){
								tmp = param->connection[count].SOCK;
								write(tmp, &message, sizeof(servertoclient));
							}
						}
						pthread_mutex_unlock(param->gmem);
						break;
					}
					case MODERATOR:{
						printf("[%s,MESSAGE]<PUBLIC> from MODERATOR: %s\n",Hours(), message.MSGSTOC.message);
						pthread_mutex_lock (param->gmem);
						for(count = 0; count < MAXCLIENT; count++){
							if (param->connection[count].CLID){
								tmp = param->connection[count].SOCK;
								message.CMD = PUBLICMOD;
								write(tmp, &message, sizeof(servertoclient));
							}
						}
						pthread_mutex_unlock(param->gmem);
						break;
					}
				}
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
												//printf("[%s,MESSAGE]<%s>Spedisco il messagio a %d\n",Hours(), message.stanza.roomname, param->connection[c].CLID);
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
							//printf("[%s,MESSAGE]Utente %d Non Registrato alla stanza\n",Hours(), message.MSGSTOC.CLID);
							message.CMD = ERRORROOM;
							write(socketmessage, &message, sizeof(servertoclient));
							break;
						}
						break;
					}
				}
				pthread_mutex_unlock(param->gmem);
				if (count == MAXROOM){
					//printf("[%s,MESSAGE]La Stanza Non Esiste\n",Hours());
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
	int socket_message, socket_clientmessage, co;
	time_t forfile;
	FILE *filelog;
	forfile = time(NULL);
	char nmf[30];
	strcpy(nmf,ctime(&forfile));
	strcat(nmf, "-sm");
	filelog = fopen(nmf, "a");
	/*-------*/
	void AllOutMessage (){
	printf("[%s,MESSAGE]Ricevuto Segnale di Chiusura\n", Hours());
	fprintf(filelog,"[%s,MESSAGE]Ricevuto Segnale di Chiusura\n", Hours());
	if(close(socket_message) == -1){
		printf("[%s,MESSAGE]Errore Chiusura Socket Message\n", Hours());
		fprintf(filelog,"[%s,MESSAGE]Errore Chiusura Socket Message\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	printf("[%s,MESSAGE]Socket Chiusa, Terminazione Thread.\n", Hours());
	fprintf(filelog,"[%s,MESSAGE]Socket Chiusa, Terminazione Thread.\n", Hours());
	fclose(filelog);
	pthread_exit(NULL);
	}
	/*------*/
	signal(SIGUSR1,AllOutMessage);
	if (TURNOFF) printf("[%s,MESSAGE]Avvio SocketMessage\n", Hours());
	fprintf(filelog,"[%s,MESSAGE]Avvio SocketMessage\n", Hours());
	if ((socket_message = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("[%s,MESSAGE]Errore Creazione SocketChat\n", Hours());
		fprintf(filelog,"[%s,MESSAGE]Errore Creazione SocketChat\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in addresschat;
	addresschat.sin_family = AF_INET;
	inet_aton(IPSERVER, &addresschat.sin_addr);
	addresschat.sin_port = htons(PORTMESSAGE);
	if (TURNOFF) printf("[%s,MESSAGE]: Porta Usata: %d, Indirizzo Usato: %s\n", Hours(), ntohs(addresschat.sin_port), inet_ntoa(addresschat.sin_addr));
	fprintf(filelog,"[%s,MESSAGE]: Porta Usata: %d, Indirizzo Usato: %s\n", Hours(), ntohs(addresschat.sin_port), inet_ntoa(addresschat.sin_addr));
	socklen_t lensockchat;
	lensockchat = (socklen_t)sizeof(addresschat);
	if (bind(socket_message, (struct sockaddr*)&addresschat, lensockchat) == -1){
		if (TURNOFF) printf("[%s,MESSAGE]Errore Avvio BindMessage\n", Hours());
		fprintf(filelog,"[%s,MESSAGE]Errore Avvio BindMessage\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	if (listen(socket_message, 5) == -1){
		if (TURNOFF) printf("[%s,MESSAGE]Errore Avvio ListenMessage\n", Hours());
		fprintf(filelog,"[%s,MESSAGE]Errore Avvio ListenMessage\n", Hours());
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
	if (TURNOFF) printf("[%s,MESSAGE]In Ricezione\n", Hours());
	fprintf(filelog,"[%s,MESSAGE]In Ricezione\n", Hours());
	while(c->server_on){
		socket_clientmessage = accept(socket_message, (struct sockaddr*)&addresschat, &lensockchat);
		if (socket_clientmessage == -1){
			printf("[%s,MESSAGE]Errore Avvio AcceptMessage\n", Hours());
			fprintf(filelog,"[%s,MESSAGE]Errore Avvio AcceptMessage\n", Hours());
			fclose(filelog);
			exit(EXIT_FAILURE);
		}
		paramthread.gmem = c->gmem;
		paramthread.socket_clientchat = socket_clientmessage;
		paramthread.connection = c->connection;
		paramthread.stanza = c->stanza;
		printf("[%s,MESSAGE]Accept Attiva, Creazione nuovo Thread Dedicato in corso...\n", Hours());
		fprintf(filelog,"[%s,MESSAGe]Accept Attiva, Creazione nuovo Thread Dedicato in corso...\n", Hours());
		if (pthread_create(&threadmessage, NULL, ThreadMessage, (void*)&paramthread) == -1){
			printf("[%s,MESSAGE]Errore Creazione ThreadMessage\n", Hours());
			fprintf(filelog,"[%s,MESSAGE]Errore Creazione ThreadMessage\n", Hours());
			fclose(filelog);
			exit(EXIT_FAILURE);
		}
	}
}

/*--------------------------------------------------------------------------*/
