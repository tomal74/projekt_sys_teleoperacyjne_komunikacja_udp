/*
	Simple UDP Server
	Tomasz Konieczka
*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <winsock2.h>
#include <chrono>
#include <string>
#include <iostream>
#include <format>
#include <ctime>

#include "packet.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 1024UL*64UL	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data

#define RECIVE_TIMEOUT 2.0	// recive timeout in sec


std::string getTimeStr() {
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::string s(30, '\0');
	std::strftime(&s[0], s.size(), "%H:%M:%S %Y-%m-%d", std::localtime(&now));
	return s;
}


int main()
{
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;
	unsigned long ul = 1;
	int           nRet;

	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	nRet = ioctlsocket(s, FIONBIO, (unsigned long*)&ul);

	if (nRet == SOCKET_ERROR)

	{
		printf("Could not create socket in no-blocking mode : %d", WSAGetLastError());
		// Failed to put the socket into non-blocking mode

	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");
	
	Packet mPacket;

	strcpy_s(mPacket.message, sizeof(mPacket.message), "Warszawa Radar");
	mPacket.cnt = 0;

	//keep listening for data
	while ( true )	// main loop 
	{
		//printf("Waiting for data...");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		//memset(buf, '\0', BUFLEN);

		auto start = std::chrono::system_clock::now();
		std::chrono::duration<double> diffTime;

		while ( true ) {

			auto end = std::chrono::system_clock::now();
			diffTime = end - start;
			//try to receive some data, this is a blocking call
			if ((recv_len = recvfrom(s, (char*)&mPacket, BUFLEN, 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR)
			{
				//printf("recvfrom() failed with error code : %d \n", WSAGetLastError());
				//exit(EXIT_FAILURE);
			}
			else {
				std::cout << "recived!, cnt: " << mPacket.cnt << "msg: " << mPacket.message << " len: " << recv_len << " --> " << getTimeStr() << std::endl;
				break;
			}

			if (diffTime.count() > RECIVE_TIMEOUT) {
				std::cout << "Recive Timeout!    " << getTimeStr() << std::endl;
				break;
			}
		}


		//print details of the client/peer and the data received
		//  printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		//printf("Data: %s\n", buf);
		
		// printf("%d\n", mPacket.cnt);

		//now reply the client with the same data
		if (sendto(s, (const char*) & mPacket, recv_len, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			//printf("sendto() failed with error code : %d \n", WSAGetLastError());
			//exit(EXIT_FAILURE);
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}