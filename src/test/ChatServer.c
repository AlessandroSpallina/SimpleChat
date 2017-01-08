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
parselo paramserverlog;
/*-------Main---------*/
int main (void){
	printf("Avvio Server Chat in Corso...\n");
	sleep(1);
	printf("Avvio Server Login in Corso...\n");
	printf("Generazione ThreadLogin in corso...\n");
	paramserverlog.connection = connessione;
	paramserverlog.server_on = SERVER_ON;
	printf("pronti...\n");
	if (pthread_create(&serlo, NULL, ServerLogin, (void *)&paramserverlog) == -1) {
		printf("Errore Creazione Thread\n");
		exit (EXIT_FAILURE);
	}
	while (1);
}

