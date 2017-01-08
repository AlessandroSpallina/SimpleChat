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
#include <pthread.h>
/*----Header Personali----*/
#include "Funzioni.h"
/*---------Define--------*/
#define SERVER_ON 1
#define MAXCLIENT 3

/*--Variabili Server--*/
conn connessione[MAXCLIENT];
pthread_t serlo, sercli;
int threadlogin, threadclient;
parse paramserverlog;
pthread_mutex_t gmem;
int stateofmemory;
/*-------Main---------*/
int main (void){
	printf("Avvio Server Chat in Corso...\n");
	sleep(1);
	printf("Avvio Server Login in Corso...\n");
	printf("Inizializzazione Mutex in Corso...\n");
	if (pthread_mutex_init(&gmem, NULL) == -1){
		printf("Errore Inizializzazione Mutex\n");
		exit(EXIT_FAILURE);
	}
	printf("Generazione ThreadLogin in corso...\n");
	paramserverlog.connection = connessione;
	paramserverlog.server_on = SERVER_ON;
	paramserverlog.gmem = &gmem;
	paramserverlog.som = stateofmemory;
	if (pthread_create(&serlo, NULL, ServerLogin, (void *)&paramserverlog) == -1) {
		printf("Errore Creazione Thread\n");
		exit (EXIT_FAILURE);
	}
	while (1);
}

