/****************************************************************************
* Copyright © 2017 Alessio Greco
* github: https://github.com/RootPOI
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

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
/*----Header Personali----*/
#include "Funzioni.h"
/*---------Define--------*/
#define SERVER_ON 1
#define MAXCLIENT 3
/*--Variabili Server--*/
conn connessione[MAXCLIENT];
/*-------Main---------*/
int main (void){
	printf("Avvio Server Chat in Corso...\n");
	sleep(1);
	ServerLogin(connessione, SERVER_ON);
}
