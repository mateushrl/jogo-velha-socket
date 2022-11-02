#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// **************************************************************************
// definicoes iniciais
#define TAMANHO_BUFFER 3      // tamanho do buffer de recepcao
#define DEFAULT_PORT "27015"    // porta de comunicacao
#define IP_SERVIDOR "127.0.0.1" // ip do servidor
// **************************************************************************
char jogoDaVelha[3][3]; 
int linhaConvertida;
int colunaConvertida;
int resultado = 0;

void imprimirAMatriz() {
	int i,j;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      printf("| %c | ", jogoDaVelha[i][j]);
    }
    printf("\n");
  }
  printf("\n\n");
}

void iniciarJogo() {
	int i, j;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      jogoDaVelha[i][j] = ' ';
    }
  }
  imprimirAMatriz();
}


int verificaPosicaoValida(int linha, int coluna) {
  if (linha >= 3 || coluna >= 3) {
    return 0;
  }
  return 1;
}


int verificaSeGanhoLinha(char player) {
	int i,j;
  for (i = 0; i < 3; i++) {
    int igual = 1;
    for (j = 0; j < 2; j++) {
      if (jogoDaVelha[i][j] == player &&
          jogoDaVelha[i][j] == jogoDaVelha[i][j + 1]) {
        igual++;
      }
    }
    if (igual == 3)
      return 1;
  }
  return 0;
}
int verificaSeGanhoColuna(char player) {
	int i,j;
  for (i = 0; i < 3; i++) {
    int igual = 1;
    for (j = 0; j < 2; j++) {
      if (jogoDaVelha[j][i] == player &&
          jogoDaVelha[j][i] == jogoDaVelha[j + 1][i]) {
        igual++;
      }
    }
    if (igual == 3)
      return 1;
  }
  return 0;
}
int verificaSeGanhoDiagonalPrimaria(char player) {
  int igual = 1;
  int i;
  for (i = 0; i < 2; i++) {
    if (jogoDaVelha[i][i] == player &&
        jogoDaVelha[i][i] == jogoDaVelha[i + 1][i + 1]) {
      igual++;
    }
  }
  if (igual == 3) {
    return 1;
  } else {
    return 0;
  }
}

int verificaSeGanhoDiagonalSecundaria(char player) {
  int igual = 1;
  int i;
  for (i = 0; i < 2; i++) {
    if (jogoDaVelha[i][3 - i - 1] == player &&
        jogoDaVelha[i][3 - i - 1] == jogoDaVelha[i + 1][3 - i - 2]) {
      igual++;
    }
  }
  if (igual == 3) {
    return 1;
  } else {
    return 0;
  }
}
void verificarVitoria(char player) {
  if (verificaSeGanhoLinha(player) == 1 || verificaSeGanhoColuna(player) == 1 ||
      verificaSeGanhoDiagonalPrimaria(player) == 1 ||
      verificaSeGanhoDiagonalSecundaria(player) == 1) {
       printf("Player %c venceu !\n", player);
       if(player == 'X'){
       	resultado = -1;
	   }
	   else if(player == 'O'){
	   	resultado = -2;
	   }
  }
}
int verificaEmpate() {
	int i,j;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      if (jogoDaVelha[i][j] == ' ') {
        return 0;
      }
    }
  }
  printf("Deu Velha !\n");
  resultado = -3;
  return 1;
}


int inserirNaMatriz(int linha, int coluna, char player) {
	int sucesso = 0;
  if (verificaPosicaoValida(linha, coluna) == 1) {
    if (player == 'X' && jogoDaVelha[linha][coluna] == ' ') {
      jogoDaVelha[linha][coluna] = 'X';
      sucesso = 1;
    } else if (player == 'O' && jogoDaVelha[linha][coluna] == ' ') {
      jogoDaVelha[linha][coluna] = 'O';
      sucesso = 1;
    }
    else{
    	printf("Posicao ja ocupada\n");
  		sucesso =0;
	}
    verificarVitoria(player);
    if(resultado == 0){
    verificaEmpate(); 
}
  }
  else{
  	printf("Posicao invalida\n");
  	sucesso =0;
  }
  imprimirAMatriz();
  return sucesso;
}

void convertCharToIntVet(char charRecebido[3]) {
  linhaConvertida = (int)charRecebido[0] - 48;
  colunaConvertida = (int)charRecebido[2] - 48;
}

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
    char posicao[3];
    int player;
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

    if (recvbuf[0] == '1')
    {
        printf("\nVoce e o player X\n");
        player = 1;
    }
    else if(recvbuf[0] == '2')
    {
        printf("\nVoce e o player O\n");
        player = 2;
    }
    
	iniciarJogo();
	
    while (1)
    {

        if (player == 1)
        {
        	int sucessoInserir = 0; 
            do{
            printf("Insira a linha e coluna da posicao(LINHA COLUNA)\n");
            gets(posicao);
            convertCharToIntVet(posicao);
            sucessoInserir = inserirNaMatriz(linhaConvertida, colunaConvertida, 'X');
        	}while(sucessoInserir != 1);
        	
            sprintf(sendbuf, posicao);
            iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
            if(resultado != 0){
            break;
			}
            
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            convertCharToIntVet(recvbuf);
            inserirNaMatriz(linhaConvertida, colunaConvertida, 'O');
            
            verificarVitoria('O');
            if(resultado == 0){
            verificaEmpate();
			}
			
            if(resultado != 0){
            	break;
			}
        }

        else if (player == 2)
        {
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            convertCharToIntVet(recvbuf);
            inserirNaMatriz(linhaConvertida, colunaConvertida, 'X');
            
            verificarVitoria('X');
            if(resultado == 0){
            verificaEmpate();
			}
            if(resultado != 0){
            	break;
			}
            
            int sucessoInserir = 0; 
            do{
            printf("Insira a linha e coluna da posicao(LINHA COLUNA)\n");
            gets(posicao);
            convertCharToIntVet(posicao);
            sucessoInserir = inserirNaMatriz(linhaConvertida, colunaConvertida, 'O');
        	}while(sucessoInserir != 1);
            sprintf(sendbuf, posicao);
            iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
            
            if(resultado != 0){
            	break;
			}
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
