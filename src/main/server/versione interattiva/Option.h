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
/*-----------------------..Option File for Client---------------------------*/
//Connection
#define IPSERVER "127.0.0.1"		//IP Server
#define PORTLOGIN 2001				//Login Port
#define PORTCHAT 2002				//Chat Port
#define PORTMESSAGE 2017			//Message Port
//Security
#define PASSINTOFF 54321			//Password Intera per lo Spegnimento (integer)
#define PSK "90dcb593d9a3e464005fb46c04edf241794f7b69a7a17099c71e2439dc4edf36" //WPA-PSK Security
//General
#define MAXLENGTHMESSAGE 1024		//Lunghezza Massima Messaggi
#define MAXCLIENT 3					//Quantità Massima di Client
#define MAXROOM 3					//Quantità Massima di Stanza
#define MAXUROOM 3					//Quantità Massima di Utenti per Stanza
//Server
#define SERVER_LOGIN_ON 1			//Abilitazione Accept Server Login (boolean)
#define SERVER_CHAT_ON 1			//Abilitazione Accept Server Chat (boolean)
#define SERVER_MESSAGE_ON 1			//Abilitazione Accept Server Message (boolean)
#define TIMEWAIT 1					//Tempo di Attesa Avvio Thread Server (second)
#define TURNOFF 1					//Abilitazione Modalita Demone(0)/Interattivo(1)
/*--------------------------------------------------------------------------*/
