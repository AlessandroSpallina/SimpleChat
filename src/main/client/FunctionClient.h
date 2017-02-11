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
/*-----------------------------Function Client-------------------------------*/
#include "HeaderClient.h"

void FFLUSH(void){
	while(getchar() != '\n');
}

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
	argforthread *server = (argforthread*)arg;
	int socket_login_thread = server->socket_login;
	int socket_chat_thread = server->socket_chat;
	int socket_message_thread = server->socket_message;
	printf("[CHAT ATTIVA]\n");
	while (1){
		read(socket_message_thread, &message, sizeof(servertoclient));
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
				if (close(socket_login_thread) == -1){
					printf("Errore Chiusura Socket Client \n");
					exit(EXIT_FAILURE);
				}
				if (close(socket_chat_thread) == -1){
					printf("Errore Chiusura Socket Client \n");
					exit(EXIT_FAILURE);
				}
				if (close(socket_message_thread) == -1){
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

/*--------------------------------------------------------------------------*/
