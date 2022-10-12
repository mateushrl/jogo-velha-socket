#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// **************************************************************************
// definicoes iniciais
#define TAMANHO_BUFFER 2     // tamanho do buffer de recepcao e envio
#define DEFAULT_PORT "27015" // porta de comunicacao que sera usada
// **************************************************************************

int __cdecl main(void)
{
    // variaveis
    WSADATA wsaData; // variavel para o winsock
    int iResult;     // variavel de status
    // sockets para conexao com o servidor
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    // estrutura de enderecos
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    // buffers de recepcao e envio
    int iSendResult;
    char sendbuf[TAMANHO_BUFFER];
    char recvbuf[TAMANHO_BUFFER];
    int recvbuflen = TAMANHO_BUFFER; // comprimento do buffer de recepcao

    // inicializa o winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup falhou com o erro: %d\n", iResult);
        system("pause");
        return 1;
    }

    // pega informacoes da maquina
    char hostname[NI_MAXHOST];         // guarda nome da maquina
    gethostname(hostname, NI_MAXHOST); // pega nome da maquina

    // pega o ip da maquina e exibe
    PHOSTENT phe = gethostbyname(hostname);
    int i;
    for (i = 0; phe->h_addr_list[i] != 0; ++i)
    {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve o endereco do servidor e a porta
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo falhou com erro: %d\n", iResult);
        WSACleanup();
        system("pause");
        return 1;
    }

    // Cria um socket para conexao com o servidor
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("socket falhou com erro: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        system("pause");
        return 1;
    }

    // Configura o socket TCP do servidor
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind falhou com erro: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        system("pause");
        return 1;
    }

    freeaddrinfo(result);

    // Escuta aguardando um cliente
    printf("Aguardando outro jogador...", DEFAULT_PORT);
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen falhou com erro: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        system("pause");
        return 1;
    }

    // Aceita o socket com o cliente
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("accept falhou com erro: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        system("pause");
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    char posicao[2];
    printf("\nO jogo vai comecar");
    srand(time(NULL));
    // define qual é o player 'x' ou 'o' sendo 0=x e 1=o
    int player = rand() % 2;
    if (player == 0)
    {
        printf("\nVoce e o player X\n");
        sprintf(sendbuf, "1");
        iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
    }
    else
    {
        printf("\nVoce e o player O\n");
        sprintf(sendbuf, "0");
        iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
    }

    // *********************************************************************
    // Estabeleceu conexao com o cliente e pode trocar dados
    do
    {

        if (player == 0)
        {
            gets(posicao);
            sprintf(sendbuf, posicao);
            iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            printf("Mensagem recebida: \"%s\"\n", recvbuf);

            // guarda na matriz e printa
            // verifica se ha vitoria, derrota ou empate
        }

        else
        {
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            printf("Mensagem recebida: \"%s\"\n", recvbuf);
            gets(posicao);
            sprintf(sendbuf, posicao);
            iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);

            // guarda na matriz e printa
            // verifica se ha vitoria, derrota ou empate
        }

    } while (posicao != "-1");

    // *********************************************************************
    // Encerra a conexao
    iResult = shutdown(ClientSocket, SD_SEND);
    printf("\nJogo encerrado");
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        system("pause");
        return 1;
    }

    // cleanup
    printf("Encerrando socket...");
    closesocket(ClientSocket);
    WSACleanup();

    system("pause");
    return 0;
}