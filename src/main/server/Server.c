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
/*Server -- Greco Alessio -- UniCT/Inf.Inf/Sistemi Opearativi/Prof.sa LLB-- */
/*-------------------------------Header-------------------------------------*/
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
#include <pthread.h>
#include <wait.h>
#include <signal.h>
#include <sys/stat.h>
#include "Prototype.h"
/*------------------------------Variabili Server----------------------------*/
conn connessione[MAXCLIENT];
pthread_t serlo, serchat, sermess;
int threadlogin, threadclient, off;
parse paramserverlog, paramserverchat, paramservermessage;
pthread_mutex_t gmem;
servertoclient tomex;
room vector[MAXROOM];
time_t forfile;
/*------------------------------------Main----------------------------------*/
int main (void){
	FILE *filelog;
	forfile = time(NULL);
	char nmf[30];
	strcpy(nmf,ctime(&forfile));
	strcat(nmf, "-pp");
	filelog = fopen(nmf, "a");
	fprintf(filelog,"Log SERVERCHAT Processo Iniziale: %s\n", ctime(&forfile));
	printf("[%s]AvvioServer, Inizializzazione Memoria\n", Hours());
	fprintf(filelog,"[%s]AvvioServer,Inizializzazione Memoria\n", Hours());
	for(int count = 0; count < MAXCLIENT; count++){
		connessione[count].CLID = 0;
	}
	printf("[%s]Inizializzazione Mutex\n", Hours());
	fprintf(filelog,"[%s]Inizializzazione Mutex\n", Hours());
	if (pthread_mutex_init(&gmem, NULL) == -1){
		fprintf(filelog,"[%s]Errore Inizializzazione Mutex\n", Hours());
		printf("Errore Inizializzazione Mutex\n");
		fclose(filelog);
		exit(EXIT_FAILURE);
	}
/*---------------------------------Server Login-----------------------------*/
	sleep (TIMEWAIT);
	printf("[%s]Avvio Server Login\n", Hours());
	fprintf(filelog,"[%s]Avvio Server Login\n", Hours());
	printf("[%s]Generazione ThreadLogin\n", Hours());
	fprintf(filelog,"[%s]Generazione ThreadLogin\n", Hours());
	paramserverlog.connection = connessione;
	paramserverlog.server_on = SERVER_LOGIN_ON;
	paramserverlog.gmem = &gmem;
	if (pthread_create(&serlo, NULL, ServerLogin, (void *)&paramserverlog) == -1) {
		printf("[%s]Errore Creazione Thread\n", Hours());
		fprintf(filelog,"[%s]Errore Creazione Thread\n", Hours());
		fclose(filelog);
		exit (EXIT_FAILURE);
	}
/*----------------------------------Server Chat-----------------------------*/
	sleep (TIMEWAIT);
	printf("[%s]Avvio Server Chat\n", Hours());
	printf("[%s]Generazione ThreadChat\n", Hours());
	fprintf(filelog,"[%s]Avvio Server Chat\n", Hours());
	fprintf(filelog,"[%s]Generazione ThreadChat\n", Hours());
	paramserverchat.connection = connessione;
	paramserverchat.server_on = SERVER_CHAT_ON;
	paramserverchat.gmem = &gmem;
	paramserverchat.stanza = vector;
	if (pthread_create(&serchat, NULL, ServerChat, (void *)&paramserverchat) == -1) {
		printf("[%s]Errore Creazione Thread\n",Hours());
		fprintf(filelog,"[%s]Errore Creazione Thread\n",Hours());
		fclose(filelog);
		exit (EXIT_FAILURE);
	}
/*----------------------------------Server Message--------------------------*/
	printf("[%s]Avvio Server Message\n", Hours());
	printf("[%s]Generazione ThreadMessage\n", Hours());
	fprintf(filelog,"[%s]Avvio Server Message\n", Hours());
	fprintf(filelog,"[%s]Generazione ThreadMessage\n", Hours());
	sleep(TIMEWAIT);
	paramservermessage.connection = connessione;
	paramservermessage.server_on = SERVER_MESSAGE_ON;
	paramservermessage.gmem = &gmem;
	paramservermessage.stanza = vector;
	if (pthread_create(&sermess, NULL, ServerMessage, (void *)&paramservermessage) == -1) {
		printf("[%s]Errore Creazione Thread\n",Hours());
		fprintf(filelog,"[%s]Errore Creazione Thread\n",Hours());
		fclose(filelog);
		exit (EXIT_FAILURE);
	}
/*--------------------------Spegnimento Interattivo-------------------------*/
	while (TURNOFF){
		scanf("%d", &off);
		if (off == PASSINTOFF){
			tomex.CMD = SERVEROFF;
			pthread_mutex_lock (&gmem);
			for (off = 0; off < MAXCLIENT; off++){
				if(connessione[off].SOCK){
					printf("[%s]Socket connesse %d\n", Hours(), connessione[off].SOCK);
					fprintf(filelog,"[%s]Socket connesse %d\n", Hours(), connessione[off].SOCK);
					if(write(connessione[off].SOCK,&tomex,sizeof(servertoclient)) == -1){
						printf("[%s]Errore Invio Chiusura\n",Hours());
						fprintf(filelog,"[%s]Errore Invio Chiusura\n",Hours());
						fclose(filelog);
						exit(EXIT_FAILURE);
					}
					printf("[%s]Sock %d chiusa\n",Hours(), connessione[off].SOCK);
					fprintf(filelog,"[%s]Sock %d chiusa\n",Hours(), connessione[off].SOCK);
				}
			}
				pthread_mutex_unlock(&gmem);
				pthread_kill (sermess,SIGUSR1);
				pthread_join (sermess, NULL);
				pthread_kill (serchat, SIGUSR2);
				pthread_join (serchat, NULL);
				pthread_kill (serlo, SIGPROF);
				pthread_join (serlo, NULL);
				fprintf(filelog,"[%s]Server Spento\n",Hours());
				fclose(filelog);
				exit(EXIT_SUCCESS);	
		}
	}
	pthread_join(serlo, NULL);
}
/*--------------------------------------------------------------------------*/
