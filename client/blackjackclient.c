// diverse variabili sono dichiarate come globali per essere usate da tutte le funzioni liberamente.
// le variabili sono dichiarate come static per chiarire il loro scopo, ma la dichiarazione come static o meno non influisce sul funzionamento del codice.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string.h>
#include "blackjackclient.h"

// valori di default usati per la porta.
// il client si connette di default a localhost
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 128

static WSADATA wsaData;
static int iResult, init_result;
static struct addrinfo* result = NULL, * ptr = NULL, hints;
static SOCKET ConnectSocket;
static char qbuffer[DEFAULT_BUFLEN] = "", recvbuf[DEFAULT_BUFLEN] = "";

void close_connection();
int client_connection_init();

// usata per accorciare il codice e aumentare la leggibilità
void close_connection()
{
    closesocket(ConnectSocket);
    WSACleanup();
}

// inizia la connessione del client al server
// valori di ritorno: 1 in caso di errore nella connessione, 0 in caso di successo
int client_connection_init()
{

    // inizio libreria
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // setup socket
    ZeroMemory(&hints, sizeoff(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // crea socket
    ConnectSocket = INVALID_SOCKET;
    ptr = result;

    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // connetti al server
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

    if (iResult == SOCKET_ERROR)
    {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n);
        WSACleanup();
        return 1;
    }

    return 0;
}
    
// gestisce il processo di registrazione e di login con il server
int server_login(char username[], char password[], MODE mode)
{
    init_result = client_connection_init();

    if (init_result == 1)
    {
        return 1;
    }

    // manda username da salvare
    sprintf_s(qbuffer, DEFAULT_BUFLEN, "Username:%s", username);

    iResult = send(ConnectSocket, qbuffer, DEFAULT_BUFLEN, 0);
}

