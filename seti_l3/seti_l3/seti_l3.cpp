#include <iostream>
#include <winsock2.h>
#include <string>
#include <signal.h>
#include <conio.h>
#include <string> 
#include <sstream>
#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable: 4996)
using namespace std;


SOCKET Connections[100]; // массив подключений
int Counter = 0; // нужен для кол-ва подключенных клиентов
int maxCountClient=3; // кол-во максимального размера подключенных клиентов
int currentGamer = 0;
string hiddenLetter;

//CRITICAL_SECTION  cs;

void Bad_Key_Ctrl_C(int but) {

	signal(but, Bad_Key_Ctrl_C); 
}


void ReceivingBroadcastAddressHendler(SOCKET udpSocket) {
	char buf[256];
	sockaddr_in sockADDR;
	int lengthMes = sizeof(sockADDR);
	ZeroMemory(&sockADDR, lengthMes);
	ZeroMemory(&buf, sizeof(buf));

	while (true) 
	{
		if (recvfrom(udpSocket, (char*)&buf, sizeof(buf), NULL, (sockaddr*)&sockADDR, &lengthMes) == SOCKET_ERROR)
			cout << "Haven't message " + WSAGetLastError() << endl;
		if (sendto(udpSocket, (char*)&buf, sizeof(buf), NULL,(sockaddr*)&sockADDR, lengthMes) == SOCKET_ERROR)
			cout <<  "Error: Message don't send " + WSAGetLastError() << endl;
	}
	closesocket(udpSocket);
}

void giveAwayMyIpAddress() {
	
	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;
	int sizeAddr = sizeof(addr);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) 
	{
		printf("Error \n", WSAGetLastError());
		return;
	}

	if (bind(sock, (SOCKADDR*)&addr, sizeAddr) == SOCKET_ERROR) 
	{
		cout << "Error \n";
		closesocket(sock);
		return;
	}

	char optval = 5;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ReceivingBroadcastAddressHendler,(LPVOID)(sock), NULL, NULL);

}




void SendMess(int index)
{
	int length_message;
	int countWin;
	//EnterCriticalSection(&cs);
	while (true)
	{
		recv(Connections[index], (char*)&length_message, sizeof(int), NULL);
		char* msg = new char[length_message + 1];
		msg[length_message] = '\0';
		recv(Connections[index], msg, length_message, NULL);

		if (currentGamer == index)
		{
			for (int i = 0; i < Counter; i++)
			{
				const char* mes = new char[256];
				int len;
				if (hiddenLetter == msg)
				{
					bool flag = false;
					if (index == i)
					{
						len = 10;
						mes = " You win";
						send(Connections[i], (char*)&len, sizeof(int), NULL);
						send(Connections[i], mes, len, NULL);
						closesocket(Connections[i]);
						flag = true;
					}
					else
					{
						len = 10;
						mes = "You lose";
						send(Connections[i], (char*)&len, sizeof(int), NULL);
						send(Connections[i], mes, len, NULL);
						closesocket(Connections[i]);
					}
					if (flag && i==(Counter-1))
					{
						return;
					}

				}
				else
				{
					if (index == i)
					{
						len = 10;
						mes = "No";
						send(Connections[i], (char*)&len, sizeof(int), NULL);
						send(Connections[i], mes, len, NULL);
					}
				}
			}
			currentGamer+=1;
			if (currentGamer >= Counter)
			{
				currentGamer = 0;
			}
		}
		else
		{
			const char* mes = new char[256];
			int len=15;
			mes = "Not your tern";
			send(Connections[index], (char*)&len, sizeof(int), NULL);
			send(Connections[index], mes, len, NULL);
		}
		delete[] msg;
	}
	//LeaveCriticalSection(&cs);
}

int RandomNumberRange(int a, int b)
{
	srand(time(NULL));
	int d= rand() % (b - a + 1) + a;
	return d;
}

int main(int args, char* argv[])
{
	char n= RandomNumberRange(65, 90);
	hiddenLetter = n;

	char* ip = new char[16];
	cin >> ip;

	WSADATA wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{
		cout << "Error" << endl;
		exit(1);
	}

	giveAwayMyIpAddress();

	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;
	int sizeofaddr = sizeof(addr);

	SOCKET sock = socket(AF_INET,SOCK_STREAM, NULL);
	


	bind(sock,(SOCKADDR*)&addr,sizeof(addr));
	listen(sock, maxCountClient);

	SOCKET newConnection;
	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(sock, (SOCKADDR*)&addr, &sizeofaddr);
		if (newConnection == 0)
		{
			cout << "Error \n";
			return 1;
		}
		else
		{
			cout << "Connected successful \n";
		}
		Connections[i] = newConnection;
		Counter++;
		string message = "Hidden letter " + hiddenLetter;
		send(newConnection, (char*)&message, sizeof(message), NULL);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendMess, (LPVOID)(i), NULL, NULL);
	}
	system("pause");
	return 0;
}



