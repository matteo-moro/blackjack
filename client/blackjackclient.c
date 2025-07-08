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
// username e password sono le stringhe per username e password da usare
// mode può essere LOGIN_MODE o REGISTER_MODE e specifica l'operazione da eseguire
// ritorna 0 in caso di successo, 1 in caso di errore
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

    if (iResult < 0)
    {
        printf("recv failed: %d\n", WSAGetLastError());
        return 1;
    }
    else if (iResult == 0)
    {
        printf("server connection closed without warning\n");
        return 1;
    }

    // manda password da salvare
    sprintf_s(qbuffer, DEFAULT_BUFLEN, "Password:%s", password);

    iResult = send(ConnectSocket, qbuffer, DEFAULT_BUFLEN, 0);

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
        close_connection();
        return 1;
    }

    iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);

    if (iResult < 0)
    {
        printf("recv failed: %d\n", WSAGetLastError());
        close_connection();
        return 1;
    }
    else if (iResult == 0)
    {
        printf("server connection closed without warning\n");
        close_connection();
        return 1;
    }

    // esegui registrazione o login
    if (mode == LOGIN_MODE)
    {

        // manda query di login al server
        strcpy(qbuffer, "Login");

        iResult = send(ConnectSocket, qbuffer, DEFAULT_BUFLEN, 0);

        if (iResult == SOCKET_ERROR)
        {
            printf("send failed: %d\n", WSAGetLastError());
            close_connection();
            return 1;
        }

        // ricevi risultato del login e restituisci valore corretto
        iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);

        if (strcmp(recvbuf, "logok") == 0)
        {
            printf("login successful\n");

            iResult = shutdown(ConnectSocket, SD_SEND);

            if (iResult == SOCKET_ERROR)
            {
                printf("shutdown failed: %d\n", WSAGetLastError());
                return 1;
            }

            closesocket(ConnectSocket);
            WSACleanup();
            return LOGIN_SUCCESS;
        }
        else if (strcmp(recvbuf, "logfail") == 0)
        {
            printf("login failed\n");

            iResult = shutdown(ConnectSocket, SD_SEND);

            if (iResult == SOCKET_ERROR)
            {
                printf("shutdown failed: %d\n", WSAGetLastError());
                close_connection();
                return 1;
            }

            close_connection();
            return LOGIN_FAIL;
        }
    }
    else if (REGISTER_MODE)
    {

        // invia query di register al server
        strcpy(qbuffer, "Register");

        iResult = send(ConnectSocket, qbuffer, DEFAULT_BUFLEN, 0);

        if (iResult == SOCKET_ERROR)
        {
            printf("send failed: %d\n", WSAGetLastError());
            close_connection();
            return 1;
        }

        // ricevi risultato register e ritorna valore corretto
        iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);

        if (strcmp(recvbuf, "regok") == 0)
        {
            printf("register successful\n");

            iResult = shutdown(ConnectSocket, SD_SEND);

            if (iResult == SOCKET_ERROR)
            {
                printf("shutdown failed: %d\n", WSAGetLastError());
                close_connection();
                return 1;
            }

            return REGISTER_SUCCESS;
        }

        close_connection();
        return REGISTER_FAIL;


    }
}

// aggiorna il valore del saldo del giocatore sul database
// modify_value è il valore che indica di quanto deve essere modificato il saldo
// ritorna 0 in caso di successo, 1 in caso di errore
int money_value_update(int modify_value)
{

    init_result = client_connection_init();

    if (init_result == 1)
    {
        return 1;
    }

    // invia query di update al server
    sprintf_s(qbuffer, DEFAULT_NUFLEN, "Modify:%d", modify_value);

    iResult = send(ConnectSocket, qbuffer, DEFAULT_BUFLEN, 0);

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());

        close_connection();
        return 1;
    }

    // controlla se l'update ha avuto successo e ritorna il risultato dell'operazione
    iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);

    if (iResult < 0)
    {
        printf("recv failed: %d\n", WSAGetLastError());
        close_connection();
        return 1;
    }
    else if (iResult == 0)
    {
        printf("server connection closed without warning\n");
        close_connection();
        return 1;
    }

    close_connection();
    return 0;
}

// ritorna il valore del saldo salvato sul server
// in caso di errore ritorna 1
// in caso di valore non valido sul database ritorna 0
int money_value_get()
{
    init_result = client_connection_init();
    if (init_result == 1)
    {
        return 1;
    }

    // invia query per ricevere valore al server
    strcpy(qbuffer, "GetVal");

    iResult = send(ConnectSocket, qbuffer, DEFAULT_BUFLEN, 0);

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
        close_connection();
        return 1;
    }

    // ritorna il valore del saldo
    iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);

    if (strmp(recvbuf, "") != 0)
    {
        int return_value = (int)strtol(recvbuf, recvbuf[strlen(recvbuf)], 10);
        close_connection();
        return return_value;
    }

    printf("failure in receiving money amount\n");
    close_connection();
    return 0;
}

