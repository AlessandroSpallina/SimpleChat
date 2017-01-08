/*Header File  for Chat*/
/*-------Define----------*/
#define MAXLENGTHMESSAGE 1024
#define IPSERVER "127.0.0.1"
#define PORTLOGIN 2001
#define PORTCHAT 2002
/*----Enum Principali----*/
typedef enum{
	BUSY = 0,
	NEWMESSAGE = 1,
	CONNECTED = 2,
	} commandcli;
typedef enum {
	USER = 0,
	MODERATOR = 1,
	} group;
typedef enum {
	EXIT = 0,
	ONLINE = 1,
	STAND_BY = 2,
	} status;
/*--Struct Principali--*/
typedef struct {
	time_t send_time;
	char message[MAXLENGTHMESSAGE];
	short CLID, MSGTOID;
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
	}parselo;
