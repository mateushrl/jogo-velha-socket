#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// **************************************************************************
// definicoes iniciais
#define TAMANHO_BUFFER 512      // tamanho do buffer de recepcao
#define DEFAULT_PORT "27015"    // porta de comunicacao
#define IP_SERVIDOR "127.0.0.1" // ip do servidor
// **************************************************************************

int main(int argc, char **argv)
{
    // variaveis
    WSADATA wsaData; // variavel para o winsock
    int iResult;     // variavel de status
    // sockets para conexao com o servidor
    SOCKET ConnectSocket = INVALID_SOCKET;
    // estrutura de enderecos
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    // buffers de recepcao e envio
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

    // inicializa o tipo de conexao
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; // datagramas TCP
    hints.ai_protocol = IPPROTO_TCP;

    // resolve o endereco e porta do servidor
    iResult = getaddrinfo(IP_SERVIDOR, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo falhou com o erro: %d\n", iResult);
        WSACleanup();
        system("pause");
        return 1;
    }

    // tenta conectar ao endereco do servidor
    printf("Aguardando outro jogador...");
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {

        // cria um socket para se conectar ao servidor
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("Socket falhou com o erro: %ld\n", WSAGetLastError());
            WSACleanup();
            system("pause");
            return 1;
        }

        // tenta conecta ao servidor
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Nao foi possivel se conectar\n");
        system("pause");
        WSACleanup();
        return 1;
    }

    // *********************************************************************
    // Estabeleceu conexao com o servidor e pode trocar dados
    char posicao[2];
    int player;
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

    if (recvbuf[0] == '0')
    {
        printf("\nVoce e o player X\n");
        player = 0;
    }
    else
    {
        printf("\nVoce e o player O\n");
        player = 1;
    }

    while (sendbuf != "0")
    {

        if (player == 0)
        {
            gets(posicao);
            sprintf(sendbuf, posicao);
            iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("Mensagem recebida: \"%s\"\n", recvbuf);

            // guarda na matriz e printa
            // verifica se ha vitoria, derrota ou empate
        }

        else
        {
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("Mensagem recebida: \"%s\"\n", recvbuf);
            gets(posicao);
            sprintf(sendbuf, posicao);
            iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);

            // guarda na matriz e printa
            // verifica se ha vitoria, derrota ou empate
        }

        Sleep(1000);
    }

    // *********************************************************************
    // Encerra a conexao
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("Falhou encerrar a conexao com o erro: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        system("pause");
        return 1;
    }

    // cleanup
    printf("Encerrando socket...");
    closesocket(ConnectSocket);
    WSACleanup();

    system("pause");
    return 0;
}