#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <signal.h>
#include <string>
#include <signal.h>
#include <ws2tcpip.h>
#define INADDR_BROADCAST  

using namespace std;
#pragma warning(disable: 4996)
SOCKET connection;
bool endGameFlag = false;
void MessHandler()
{
	int mess_length;
	while (true)
	{
		if (recv(connection, (char*)&mess_length, sizeof(int), NULL) > 0)
		{
			char* array_clients = new char[mess_length + 1];
			array_clients[mess_length] = '\0';
			if (recv(connection, array_clients, mess_length, NULL) >0)
			{
				cout << array_clients << endl;
			}
			else
			{
				cout<<"The End" << endl;
				endGameFlag = true;
				break;
			}
			//cout << array_clients << endl;
			delete[] array_clients;
		}
		else
		{
			endGameFlag = true;
			cout << "The End" << endl;
			break;
		}
	}
}


void Bad_Key_Ctrl_C(int sig) 
{
	signal(sig, Bad_Key_Ctrl_C); 
}

int iResult;



void GetAllIpAddressServers() 
{

	char buf[1024];
	ZeroMemory(buf, sizeof(buf));
	SOCKADDR_IN addr_in;
	int sizeAddrServers = sizeof(addr_in);
	addr_in.sin_port = htons(1111);
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = inet_addr("255.255.255.255");

	SOCKET socketU = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketU == INVALID_SOCKET) 
	{
		printf("Error %ld\n", WSAGetLastError());
	}
	char opt = 5;
	setsockopt(socketU, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
	socketU;
	
	char mes[256] = "1111";
	if (sendto(socketU, mes, sizeof(mes), NULL, (sockaddr*)&addr_in, sizeAddrServers) == SOCKET_ERROR)
		cout << "Haven't message " + WSAGetLastError() << endl;
	else cout << "Search servers" << endl;

	
	int repeat = 1;
	string ipServer;
	while (repeat != 4)
	{
		repeat++;
		DWORD opt = 2000;
		setsockopt(socketU, SOL_SOCKET, SO_RCVTIMEO, (const char*)&opt, sizeof DWORD);
		iResult = recvfrom(socketU, buf, sizeof(buf), NULL, (sockaddr*)&addr_in, &sizeAddrServers);
		if (iResult < 0)
		{
			break;
		}
		char clientIp[256];
		ZeroMemory(clientIp, 256);
		inet_ntop(AF_INET, &addr_in.sin_addr, clientIp, 256);

		ipServer =  clientIp;
		cout << "Found ip server" + ipServer << endl;
	}
	closesocket(socketU);
	cout << "Socket closed \n" << endl;
}

int main(int args, char* argv[])
{
	void (*isBadKeys)(int);
	isBadKeys = signal(SIGINT, Bad_Key_Ctrl_C);
	SetConsoleCtrlHandler(NULL, true); 
	
	WSADATA wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{
		cout << "Error" << endl;
		exit(1);
	}

	GetAllIpAddressServers();
	char* ip = new char[16];
	cin >> ip;

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;
	
	connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		cout << "Error \n";
		return 1;
	}
	cout << "Connected \n";

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MessHandler, NULL, NULL, NULL);
	string message;
	while (true)
	{
		getline(cin,message);
		int length_without_date = message.size();
		if (message.length() > 1 && !endGameFlag)
		{
			cout << "Is not char. Pls write one char" << endl;
			continue;
		}
		if (message[0] == '0' && message.length() == 1) 
		{
			GetAllIpAddressServers();
			continue;
		}
		if (cin.fail() || cin.eof() || length_without_date <= 0)
		{
			cin.clear();
			continue;
		}
		send(connection, (char*)&length_without_date, sizeof(int), NULL);
		send(connection, message.c_str(), length_without_date, NULL);
		signal(SIGINT, isBadKeys);
		message.clear();
	}
	system("pause");
	return 0;
}
