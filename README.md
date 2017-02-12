# SimpleChat
Basic C Client-Server Chat by Alessio Greco

##Descrizione
Il progetto si compone in 3 file eseguibili, "Client","Server" e "OpenDaemonServer", tutti scritti in linguaggio C, utilizzando per la
sintentizazione del codice le librerie standard POSIX.
A seguire una breve descrizione sul funzionamento dei singoli eseguibili.

#CLIENT
Il Client è un programma su shell che interagisce con il Server tramite dei comandi interattivi visibili all'utente, la connessione
col server avviene in automatico, cosi come la generazione di un ID Client che verrà poi usato come ID di riconoscimento, una volta
avviato il client e stabilita la connessione col server sarà avviato oltre al menu contestuale, un thread che consente la ricezione
di messaggi da parte del server, il tipo di client di default è impostato su USER, per portarlo in modalità MODERATOR è sufficiente
modificare le opzioni sul file optionclient.h che oltre alla medesima opzione, detiene anche altri settaggi riguardanti connessione e
sicurezza(il client va poi ricompilato).

##SERVER
Il Server è un programma su shell che avvia 3 socket con protocollo TCP nel dominio internet, esse sono suddivise in relazione al 
lavoro da svolgere, in breve, sono suddivise per Login (Registrazione), Chat(Comandi) e Messenger(Messaggi), il processo padre avvia un
thread per ogni socket per un totale di 3 thread, dove ognuno di essi gestisce l'inizializzazione e l'accettazione di richieste da parte
del client, che a loro volta vengono subappaltate ad un altro thread che gestice quel client, fornendo ad esso le risposte alle varie
richieste.
E' possibile settare diverse opzioni riguardanti il server (connessione, sicurezza, max utenza, modalità etc..) nel file option.h (è 
necessario ricompilare il sorgente in caso di modifiche sul file option.h), è possibile chiudere il server interattivamente da esso
ammenno che non sia attivata dal file option.h la modalita demone, inserendo in esso una password di interi modificabile dal file
option.h, in caso fosse attivata la modalita demone, il server può essere chiuso tramite una kill dalla shell, il server gestirà la kill
chiudendosi in modo corretto, e rilasciando dei file contenenti dei brevi log.

##OPENDAEMONSERVER
Il file OpenDaemonServer è un mini programma che permette di avviare il server come un servizio in background, restituendo a video l'id
del processo.

##Descrizione File
Nella repository è possibile trovare file di diversa natura con nomi simili, suddivisi per utilizzo.
I file Client.c Server.c e OpenDaemonServer.c contengono il codice principale che viene poi compilato.
I file OptionClient.h e Option.h contengono i settaggi primari del client e del server.
I file HeaderClient e Header.h contengono le strutture dati.
I file FunctionClient.h e Function.h contengono le relative funzioni nella loro totalità.
I file PrototypeClient.h e Prototype.h contengono i relativi prototipi delle funzioni contenutite nei file Function.

##Compilazione
I file eseguibili sono ottenuti tramite la compilazione da shell nella seguente sintassi:
cc -o Server Server.c -lpthread
cc -o Client Client.h -lpthread
cc -o OpenDaemonServer OpenDaemonServer.c

