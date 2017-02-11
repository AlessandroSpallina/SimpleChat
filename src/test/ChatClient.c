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
/*Progetto CHAT, Gestione Client -- Greco Alessio -- UniCT -- */
/*----------------------*/
/*-------Header---------*/
#include <string.h>
#include <signal.h>
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
#include "Header.h"
#include <sys/sem.h>
#include <sys/ipc.h>
#include <pthread.h>
/*---------Define--------*/
//#define SERVER_ON 1
#define USERTYPE USER
#define FFLUSH while(getchar() != '\n')
/*------Variabili-----*/
conn connection;
servertoclient stoc, toclient;
int scelta, count,socket_login, socket_chat, socket_message;
boolean onroom;
pthread_t recmsg;
time_t now;
/*----Funzioni------*/
void LDS (char s[], unsigned short dim){
		unsigned short i;
		for (i = 0; i < dim; i++)
			if((s[i] = getchar()) == '\n') break;
		if (i == dim -1) FFLUSH;
		s[i] = '\0';
	
}
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
void *ClientThreadMSG (void *arg){
	servertoclient message;
	printf("[CHAT ATTIVA]\n");
	while (1){
		read(socket_message, &message, sizeof(servertoclient));
		switch (message.CMD){
			case PRIVATE:{
				printf("[PRIVATO]<%s> %d: %s\n",message.MSGSTOC.timenow, message.MSGSTOC.CLID, message.MSGSTOC.message);
				break;
			}
			case PUBLIC:{
				printf("[PUBBLICO]<%s> %d: %s\n",message.MSGSTOC.timenow, message.MSGSTOC.CLID, message.MSGSTOC.message);
				break;
			}
			case OFFLINE:{
				printf("[SERVER]: L'utente digitato non esiste o non e' OFFLINE\n");
				break;
			}
			case SERVEROFF: {
				printf("[SERVER]: Il Server sta per essere chiuso, chiusura client in corso...\n");
				if (close(socket_login) == -1){
					printf("Errore Chiusura Socket Client \n");
					exit(EXIT_FAILURE);
				}
				if (close(socket_chat) == -1){
					printf("Errore Chiusura Socket Client \n");
					exit(EXIT_FAILURE);
				}
				if (close(socket_message) == -1){
					printf("Errore Chiusura Socket Client \n");
					exit(EXIT_FAILURE);
				}
				exit(EXIT_SUCCESS);
				break;
			}
			case MESSAGETOROOM:{
				printf("[%s]<%s> %d: %s\n", message.stanza.roomname,message.MSGSTOC.timenow, message.MSGSTOC.CLID, message.MSGSTOC.message);
				break;
			}
			case ERRORROOM: {
				printf("Non Sei Registrato a questa Stanza\n");
				break;
			}
			case ERRORROOM2: {
				printf("La Stanza Inserita non Esiste\n");
				break;
			}
			case PUBLICMOD:{
				printf("[MOD]<%s> %d: %s\n",message.MSGSTOC.timenow, message.MSGSTOC.CLID, message.MSGSTOC.message);
			}
		}
	}
}
/*-------Main---------*/
int main (void){
	printf("Avvio Client...\n");
	printf("Avvio Socket Client (Login) in corso...\n");
	if ((socket_login = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Errore Creazione SocketLogin\n");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in addresserver;
	addresserver.sin_family = AF_INET;
	addresserver.sin_port = htons(PORTLOGIN);
	addresserver.sin_addr.s_addr = inet_addr(IPSERVER);
	printf("Connessione al Server\n");
	if (connect (socket_login, (struct sockaddr*)&addresserver, sizeof(addresserver)) == -1){
		printf("Errore Connessione al ServerLogin\n");
		exit(EXIT_FAILURE);
	}
	srand(time(NULL));
	connection.CLID = (rand()%2500)+(rand()%2500);
	toclient.MSGSTOC.CLID = connection.CLID;
	connection.CLGRP = USERTYPE;
	connection.STAT = ONLINE;
	toclient.MSGSTOC.CLGRP = USERTYPE;
	strcpy(connection.psk, "1806");
	strcat(connection.psk, PSK);
	strcat(connection.psk, "2016");
	stoc.CMD = ERRORPSK;
	if(write (socket_login, &connection, sizeof(conn)) == -1){
		printf("Errore Invio Struct Messaggio\n");
		exit(EXIT_FAILURE);
	}
	if (read(socket_login, &stoc, sizeof(servertoclient)) == -1){
		printf("Errore Ricezione Risposta Collegamento\n");
		exit(EXIT_FAILURE);
	}
	switch (stoc.CMD){
		case ERRORPSK: {
			printf("Errore PSK\n");
			exit(EXIT_FAILURE);
		}
		case BUSY: {
			printf("Massima quantita' di client raggiunta\n");
			exit(EXIT_FAILURE);
		}
		case CONNECTED: {
			printf("Connesso\n");
			//printf("Avvio Socket Client (Chat) in corso...\n");
			if ((socket_chat = socket(AF_INET, SOCK_STREAM, 0)) == -1){
				printf("Errore Creazione SocketChat\n");
				exit(EXIT_FAILURE);
			}
			if ((socket_message = socket(AF_INET, SOCK_STREAM, 0)) == -1){
				printf("Errore Creazione SocketChat\n");
				exit(EXIT_FAILURE);
			}
			struct sockaddr_in addresserverchat, addresservermessage;
			addresserverchat.sin_family = AF_INET;
			addresserverchat.sin_port = htons(PORTCHAT);
			addresserverchat.sin_addr.s_addr = inet_addr(IPSERVER);
			addresservermessage.sin_family = AF_INET;
			addresservermessage.sin_port = htons(PORTMESSAGE);
			addresservermessage.sin_addr.s_addr = inet_addr(IPSERVER);
			//printf("Connessione in Corso al ServerLogin...\n");
			if (connect (socket_chat, (struct sockaddr*)&addresserverchat, sizeof(addresserverchat)) == -1){
				printf("Errore Connessione al ServerLogin\n");
				exit(EXIT_FAILURE);
			}
			//printf("Connessione in Corso al ServerMessage...\n");
			if (connect (socket_message, (struct sockaddr*)&addresservermessage, sizeof(addresservermessage)) == -1){
				printf("Errore Connessione al ServerMessage\n");
				exit(EXIT_FAILURE);
			}
			toclient.MSGSTOC.CLID = connection.CLID;
			write (socket_message, &toclient, sizeof(servertoclient));
			if (pthread_create(&recmsg, NULL, ClientThreadMSG, NULL) == -1){
				printf("Errore Creazione ThreadPVT\n");
				exit(EXIT_FAILURE);
				}	
			while (1){
				printf("1 - Lista Contatti Attivi\n2 - Invia Messaggio Pubblico\n3 - Invia Messaggio Privato\n");
				printf("4 - Crea Stanza\n5 - Lista Stanze\n6 - Entra nella Stanza\n7 - Messaggio in Stanza\n8 - Cancella Stanza\n");
				printf("9 - Esci dalla Stanza\n10 - Chiudi Client\n");
					scanf("%d", &scelta);
					FFLUSH;
					switch (scelta){
						case 1:{
							int tmp[MAXCLIENT];
							toclient.CMD = LISTUSER;
							if(write (socket_chat, &toclient, sizeof(servertoclient)) == -1){
								printf("Errore Invio Struct Messaggio\n");
								exit(EXIT_FAILURE);
							}
							if(read (socket_chat, tmp, sizeof(tmp)) == -1){
								printf("Errore Lettura\n");
								exit(EXIT_FAILURE);
								}
							for (count = 0; count < MAXCLIENT; count++){
									if (tmp[count] && tmp[count] != connection.CLID){
										printf("Utente Attivo %d\n", tmp[count]);
									}
							}
							break;
						}
						case 2:{
							toclient.CMD = PUBLIC;
							printf("Inserire il Messaggio da mandare\n");
							LDS(toclient.MSGSTOC.message, MAXLENGTHMESSAGE);
							strcpy(toclient.MSGSTOC.timenow, Hours());
							if (write (socket_message, &toclient, sizeof(servertoclient)) == -1){
								printf("Errore Invio Struct Messaggio\n");
								exit(EXIT_FAILURE);
							}
							break;
						}
						case 3:{
							printf("Selezionare l'id utente\n");
							toclient.CMD = PRIVATE;
							scanf("%d", &toclient.MSGSTOC.MSGTOID);
							FFLUSH;
							
							printf("Inserire il Messaggio da mandare\n");
							LDS(toclient.MSGSTOC.message, MAXLENGTHMESSAGE);
							strcpy(toclient.MSGSTOC.timenow, Hours());
							if (write (socket_message, &toclient, sizeof(servertoclient)) == -1){
								printf("Errore Invio Struct Messaggio\n");
								exit(EXIT_FAILURE);
							}
							break;
						}
						case 4:{
							printf("Inserire il nome della stanza\n");
							toclient.CMD = NEWROOM;
							LDS(toclient.stanza.roomname, MAXLENGTHMESSAGE);
							if (strcmp(toclient.stanza.roomname, "\0")){
								if(write(socket_chat, &toclient, sizeof(servertoclient)) == -1){
									printf("Errore Invio Stanza\n");
									exit(EXIT_FAILURE);
								}
								read(socket_chat, &toclient, sizeof(servertoclient));
								if (toclient.CMD == ROOMOK) printf("Stanza Creata Con Successo\n");
								if (toclient.CMD == BUSY) printf("Max quantita' Stanze Raggiunto, Impossibile Creare nuove Stanze\n");
							}
							else printf("Nome non valido\n");
							break;
						}
						case 5:{
							room lista[MAXROOM];
							toclient.CMD = ROOMLIST;
							write(socket_chat, &toclient, sizeof(servertoclient));
							read(socket_chat, &lista, sizeof(lista));
							printf("Stanza			Owner\n\n");
							for(count = 0; count < MAXROOM; count++){
								if (lista[count].CLID) printf("%s			%d\n\n", lista[count].roomname, lista[count].CLID);
							}
							break;
						}
						case 6:{
							printf("Inserire il nome della stanza\n");
							toclient.CMD = ENTERROOM;
							LDS(toclient.stanza.roomname, MAXLENGTHMESSAGE);
							if (strcmp(toclient.stanza.roomname, "\0")){
								//printf("Nome Stanza: %s\n", toclient.stanza.roomname);
								if(write(socket_chat, &toclient, sizeof(servertoclient)) == -1){
									printf("Errore Invio Stanza\n");
									exit(EXIT_FAILURE);
								}
								read(socket_chat, &toclient, sizeof(servertoclient));
								switch (toclient.CMD){
									case ERRORROOM2: {
										printf("La Stanza non esiste\n");
										break;
									}
									case ERRORROOM:{
										printf("Client gia' registrato nella stanza\n");
										break;
									}
									case BUSY: {
										printf("Stanza piena\n");
										break;
									}
									case ROOMOK:{
										printf("Utente Registrato alla stanza %s\n", toclient.stanza.roomname);
										onroom = 1;
										break;
									}
								}
							}
							break;
						}
						case 7:{
							if (!onroom){
								printf("Effettuare prima l'accesso ad una stanza\n");
								break;
							}
							else {
								printf("Inserire il nome della stanza\n");
								toclient.CMD = MESSAGETOROOM;
								LDS(toclient.stanza.roomname, MAXLENGTHMESSAGE);
								printf("Inserire il Messaggio da mandare\n");
								LDS(toclient.MSGSTOC.message, MAXLENGTHMESSAGE);
								if (strcmp(toclient.stanza.roomname, "\0")){
									printf("Nome Stanza: %s\n", toclient.stanza.roomname);
									strcpy(toclient.MSGSTOC.timenow, Hours()); 
									if(write(socket_message, &toclient, sizeof(servertoclient)) == -1){
										printf("Errore Invio Stanza\n");
										exit(EXIT_FAILURE);
									}
								}	
							break;
							}
						}
						case 8:{
							printf("Inserire il nome della stanza da eliminare\n");
							toclient.CMD = ERASEROOM;
							LDS(toclient.stanza.roomname, MAXLENGTHMESSAGE);
							if(write(socket_chat, &toclient, sizeof(servertoclient)) == -1){
								printf("Errore Invio Stanza\n");
								exit(EXIT_FAILURE);
							}
							read(socket_chat, &toclient, sizeof(servertoclient));
							switch (toclient.CMD){
								case ROOMOK:{
									printf("Stanza Cancellata\n");
									break;
								}
								case ERRORROOM:{
									printf("Non sei il creatore della stanza, impossibile eliminarla.\n");
									break;
								}
								case ERRORROOM2:{
									printf("La Stanza non esiste\n");
								}
							}
							break;
						}
						case 9:{
							if (!onroom){
								printf("Effettuare prima l'accesso ad una stanza\n");
								break;
							}
							else {
								printf("Inserire il nome della stanza da cui uscire\n");
								toclient.CMD = EXITROOM;
								LDS(toclient.stanza.roomname, MAXLENGTHMESSAGE);
								if(write(socket_chat, &toclient, sizeof(servertoclient)) == -1){
									printf("Errore Invio Stanza\n");
									exit(EXIT_FAILURE);
								}
								read(socket_chat, &toclient, sizeof(servertoclient));
								switch (toclient.CMD){
									case ROOMOK:{
										printf("Non sei piu nella Stanza: %s\n",toclient.stanza.roomname);
										onroom = 0;
										break;
									}
									case ERRORROOM2:{
										printf("La Stanza non esiste\n");
									}
								}
								break;
							}
						}
						case 10:{
							printf("Chiusura in Corso\n");
							toclient.CMD = EXIT;
							if(write (socket_chat, &toclient, sizeof(servertoclient)) == -1){
								printf("Errore Invio Struct Messaggio\n");
								exit(EXIT_FAILURE);
							}
							if(write (socket_message, &toclient, sizeof(servertoclient)) == -1){
								printf("Errore Invio Struct Messaggio\n");
								exit(EXIT_FAILURE);
							}
							if(pthread_kill(recmsg, SIGSTOP) == -1){
								printf("Errore Kill\n");
								exit(EXIT_FAILURE);
							}
							if(pthread_cancel (recmsg) == -1){
								printf("Errore Cancellazione PthreadCHAT\n");
								exit(EXIT_FAILURE);
							}
							if (close(socket_login) == -1){
								printf("Errore Chiusura Socket Client \n");
								exit(EXIT_FAILURE);
							}
							if (close(socket_chat) == -1){
								printf("Errore Chiusura Socket Client \n");
							}
							if (close(socket_message) == -1){
								printf("Errore Chiusura Socket Client \n");
							}
							exit(EXIT_SUCCESS);
						}
						default: printf("Nessuna scelta valida\n");
					}
				}
			}
		}	
	}



