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
/*Header File  for Chat*/
/*-------Define----------*/
#define MAXLENGTHMESSAGE 1024
#define IPSERVER "127.0.0.1"
#define PORTLOGIN 2001
#define PORTCHAT 2002
#define PORTMESSAGE 2017
#define MAXCLIENT 3
/*----Enum Principali----*/
typedef enum{
	BUSY = 0,
	NEWMESSAGE = 1,
	CONNECTED = 2,
	LISTUSER = 3,
	LOGIN = 4,
	SERVEROFF = 5,
	OFFLINE = 6,
	} commandcli;
typedef enum {
	USER = 0,
	MODERATOR = 1,
	} group;
typedef enum {
	EXIT = 0,
	ONLINE = 1,
	STAND_BY = 2,
	PRIVATE = 3,
	PUBLIC = 4,
	} status;
/*--Struct Principali--*/
typedef struct {
	time_t send_time;
	char message[MAXLENGTHMESSAGE];
	int CLID, MSGTOID;
	group CLGRP;
	status STAT;
	}msgcontext;
typedef struct {
	int SOCK;
	group CLGRP;
	status STAT;
	short CLID;
	}conn;
typedef struct {
	commandcli CMD;
	msgcontext MSGSTOC;
	}servertoclient;
typedef struct {
	conn *connection;
	int server_on;
	pthread_mutex_t *gmem;
	}parse;
typedef struct {
	int socket_clientchat;
	pthread_mutex_t *gmem;
	conn *connection;
	}parth;

/*---Def.Variabili Statiche---*/
typedef unsigned short boolean;
