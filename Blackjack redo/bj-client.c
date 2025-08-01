#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windsock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string.h>
#include "bj-client.h"

#ifndef STRUCTS_INCLUDE
#define STRUCTS_INCLUDE
#include "bj-structs.h"
#endif

#ifndef BOOL_INCLUDE
#define BOOL_INCLUDE
#include <sdtbool.h>
#endif

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 128
#define SOCK_SUCCESS 0
#define NAME_NULL NULL
#define NO_FLAGS 0
#define STANDARD_ERROR 0 //il nome non ha un significato particolare, ma almeno indica che è un errore

WSADATA wsaData;
int iResult;
bool init_result;
struct addrinfo* result = NULL, *ptr = NULL, hints;
SOCKET ConnectionSocket;
char qbuffer[DEFAULT_BUFLEN] = "";
char recvbuf[DEFAULT_BUFLEN] = "";

//metodo creato per facilitare lettura, compilatore dovrebbe ottimizzarlo via
void close_connection()
{
    closesocket(ConnectionSocket);
    WSACleanup();
}

bool client_connection_init()
{
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData); // 2.2 = versione (penso)

    if (iResult != SOCK_SUCCESS)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return false;
    }


    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(NAME_NULL, DEFAULT_PORT, &hints, &result);

    if (iResult != SOCK_SUCCESS)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return false;
    }


    ConnectionSocket = INVALID_SOCKET;
    ptr = result;

    ConnectionSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (ConnectionSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }


    iResult = connect(ConnectionSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

    if (iResult == SOCKET_ERROR)
    {
        closesocket(ConnectionSocket);
        ConnectionSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);


    if (ConnectionSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server\n");
        WSACleanup();
        return false;
    }

    return true;

}

bool send_query(char query[])
{
    iResult = send(ConnectionSocket, query, DEFAULT_BUFLEN, NO_FLAGS);

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
        close_connection();
        return false;
    }

    iResult = recv(ConnectionSocket, recvbuf, DEFAULT_BUFLEN, NO_FLAGS);

    if (iResult < STANDARD_ERROR)
    {
        printf("recv failed: %d\n", WSAGetLastError());
        close_connection();
        return false;
    }
    else if (iResult == STANDARD_ERROR)
    {
        printf("server connection closed without warning\n");
        close_connection();
        return false;
    }

    return true;
}

bool server_login(char username[], char password[], MODE mode)
{
    init_result = client_connection_init();

    if (!init_result)
    {
        return false;
    }

    sprintf_s(qbuffer, DEFAULT_BUFLEN, "Username:%s", username);

    if (!send_query(qbuffer))
    {
        return false;
    }

    sprintf_s(qbuffer, DEFAULT_BUFLEN, "Password:%s", password);

    if (!send_query(qbuffer))
    {
        return false;
    }


    if (mode == LOGIN_MODE)
    {
        iResult = send(ConnectionSocket, "Login", DEFAULT_BUFLEN, NO_FLAGS);
    }
    else if (mode == REGISTER_MODE)
    {
        iResult = send(ConnectionSocket, "Register", DEFAULT_BUFLEN, NO_FLAGS);
    }


    if (iResult == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
        close_connection();
        return false;
    }

    
    iResult = recv(ConnectionSocket, recvbuf, DEFAULT_BUFLEN, NO_FLAGS);

    if (strcmp(recvbuf, "op_success") == 0)
    {
        printf("operation successful\n");

        iResult = shutdown(ConnectionSocket, SD_SEND);

        if (iResult == SOCKET_ERROR)
        {
            printf("shutdown failed: %d\n", WSAGetLastError());
        }

        close_connection();
        return true;
    }
    else if (strcmp(recvbuf, "op_fail") == 0)
    {
        printf("operation failed\n");

        iResult = shutdown(ConnectionSocket, SD_SEND);

        if (iResult == SOCKET_ERROR)
        {
            printf("shutdown failed: %d\n", WSAGetLastError());
        }

        close_connection();
        return false;
    }

}

bool money_value_update(int modify_value)
{
    init_result = client_connection_init();

    if (!init_result)
    {
        return false;
    }


    sprintf_s(qbuffer, DEFAULT_BUFLEN, "Modify:%d", modify_value);

    if (!send_query(qbuffer))
    {
        return false:
    }

    close_connection();
    return true;
}

int money_value_get()
{
    init_result = client_connection_init();

    if (!init_result)
    {
        return -1;
    }


    if (!send_query("GetVal"))
    {
        return -1;
    }


    iResult = recv(ConnectionSocket, recvbuf, DEFAULT_BUFLEN, NO_FLAGS);

    if (strcmp(recvbuf, "") != 0)
    {
        int return_value = (int)strtol(recvbuf, NULL, 10);
        close_connection();
        return return_value;
    }

    printf("failure in receiving money amount\n");
    close_connection();
    return -1;
}