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

int client_connection_init()
{

  // inizio libreria
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

  if (iResult != 0)
  {
    printf("WSAStartup failed: %d\n", iResult);
    return 1;
  }

