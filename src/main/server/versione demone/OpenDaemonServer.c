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
/*ServerDemone-- Greco Alessio--UniCT/Inf.Inf/Sistemi Opearativi/Prof.sa LLB*/
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
/*--------------------------------------------------------------------------*/
int main (void){
	pid_t daemonserver;
	printf("Avvio Server Chat, Modalità Servizio\n");
	daemonserver = fork();
	switch (daemonserver){
		case -1:{
			printf("Errore avvio Server Chat in Modalità Servizio (fork)\n");
			exit(EXIT_FAILURE);	
		}
		case 0:{
			if (execl("Server","Chat",(char*)0) == -1){
				printf("Errore Avvio Server Chat in Modalità Servizio (exec)\n");
				exit(EXIT_FAILURE);
			}
			return(1);
		}
		default: {
			printf("Servizio Avviato con PID: %d\n", daemonserver);
			printf("Premere un tasto per continuare\n");
			exit(EXIT_SUCCESS);
		}
	} 
}
/*--------------------------------------------------------------------------*/
