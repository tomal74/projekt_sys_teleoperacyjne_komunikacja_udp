/*
	Simple udp client
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<winsock2.h>
#include "packet.h"
#include <ctime>
#include <chrono>
#include <ws2ipdef.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define SERVER "192.168.8.105"	//ip address of udp server
#define BUFLEN 128UL	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data

#define MULTICAST_ADDR "224.1.1.1" // Multicast address for communication
//#define LOCAL_ADDR "192.168.1.146"  

enum task {
	KOMUNIKACJA,
	STEROWANIE
};



int main(void)
{
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;
	Packet Ttest;
	Packet1 Tcom;
	Packet1 TRecived;
	struct ip_mreq mreq;
	task new_task;

	// Ustawianie zadania
	new_task = KOMUNIKACJA;

	Tcom.cnt=0;
	strcpy_s(Tcom.msg, sizeof(Tcom.msg), "Testowa wiadomosc.");


	float v = 0.5; //predkosc w osi x
	float omega = 0.5; // predkosc w osi y

	
	
	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	unsigned long ul = 1; 
	int nRet;
	nRet = ioctlsocket(s, FIONBIO, (unsigned long*)&ul);
	if (nRet == SOCKET_ERROR) {
		printf("Filed to put the socket into non-blocking moode");
	}

	//Join the multicast group

	//mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
	//mreq.imr_interface.s_addr = inet_addr(LOCAL_ADDR);
	//setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));

	//setup address structure
	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
	int wys = 0;
	int wys_non_rec = 0;
	//start communication

	while (1)
	{
		auto t = std::chrono::system_clock::now();
		std::chrono::duration<double> TimeDiff;
		std::chrono::duration<double> TimeComm;

		//sterowanie robotem strzalkami
		if (new_task == STEROWANIE) {
			while (1) {
				auto time_now = std::chrono::system_clock::now();

				TimeDiff = time_now - t;

				if (GetKeyState(VK_UP) & 0x8000 && !(GetKeyState(VK_DOWN) & 0x8000)) {
					Ttest.omega = omega;
				}
				else if (GetKeyState(VK_DOWN) & 0x8000 && !(GetKeyState(VK_UP) & 0x8000)) {
					Ttest.omega = -omega;
				}
				else {
					Ttest.omega = 0;
				}
				if (GetKeyState(VK_LEFT) & 0x8000 && !(GetKeyState(VK_RIGHT) & 0x8000)) {
					Ttest.v = -v;
				}
				else if (GetKeyState(VK_RIGHT) & 0x8000 && !(GetKeyState(VK_LEFT) & 0x8000)) {
					Ttest.v = v;
				}
				else
				{
					Ttest.v = 0;
				}
				if (TimeDiff.count() > 0.5) {
					break;
				}
			}
			if (sendto(s, (const char*)&Ttest, sizeof(Ttest), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			printf("Wyslano.\n");
		}

		// end - sterowanie robotem strzalkami

		// wysylanie do testu komunikacji
		if (new_task == KOMUNIKACJA) {
			if (sendto(s, (const char*)&Tcom, sizeof(Tcom), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
		}
		//end-  wysylanie do testu komunikacji
	
		//receive a reply and print it
		//clear the buffer by filling null,
		//it might have previously received data
		memset(buf, '\0', BUFLEN);
		//try to receive some data, this is a blocking call
		
		

		//Sprawdzanie packet losów oraz prędkości komunikacji
		if (new_task == KOMUNIKACJA) {
			while (1) {
				auto time_now = std::chrono::system_clock::now();
				TimeDiff = time_now - t;
				if (recvfrom(s, (char*)&TRecived, BUFLEN, 0, (struct sockaddr*)&si_other, &slen) == SOCKET_ERROR)
				{
					if (WSAGetLastError() == WSAEWOULDBLOCK) {

					}
					else {
						printf("recvfrom() failed with error code : %d", WSAGetLastError());
						exit(EXIT_FAILURE);
					}

				}
				else break;

				if (TimeDiff.count() > 0.5) {
					printf("timeout ");
					wys_non_rec++;
					break;
				}
			}

			puts(buf);
			TimeComm = (std::chrono::system_clock::now() - t) * 1000;
			printf("%d \n", TRecived.cnt);
			printf("Czas komunikacji: %f ms \n", TimeComm.count());
			printf("Procent utraconych pakietow: %f \n", (float)wys_non_rec * 100 / Tcom.cnt);
			Tcom.cnt++;
			memset(&TRecived, 0, sizeof(TRecived));
		}
		// end - Sprawdzanie packet losów oraz prędkości komunikacji
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
