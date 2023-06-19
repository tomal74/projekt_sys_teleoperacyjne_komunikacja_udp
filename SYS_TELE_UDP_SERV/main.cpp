/*
	Simple UDP Server -> LAB4 - 
	MULTICASTING
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
#include <ws2ipdef.h>
#include <vector>
#include <windows.h>

#include "softDiffRobot.h"
#include "SoftTimer.h"
#include "packet.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512UL	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data

constexpr auto LOCAL_ADDR = "192.168.1.145";
constexpr auto MULTICAS_GROUP_ADDR = "224.1.1.1";
constexpr auto RECIVE_TIMEOUT = 2.0f;	// recive timeout in sec;

/* LITERATURA */
// biblia TCP/IP
// black hat python -- bardziej dla hackerów

std::string getTimeStr() {
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::string s(30, '\0');
	std::strftime(&s[0], s.size(), "%H:%M:%S %Y-%m-%d", std::localtime(&now));
	return s;
}

softDiffRobot mRobot(0.01);


void odometryCB(SoftTimer* SoftTimer)
{
	mRobot.vOdometryUpdate();
}

void printOdometryCB(SoftTimer* SoftTimer)
{
	mRobot.xPrintOdometry();
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

	/*
	* ********** LAB4 **********
	* 
	* MULTICASTING - ustawienia multicastingu 
	* aby go w³aczyc nalezy dodac komputer nasluchujacy do grupy multicatingowej (jak poniezej).
	* Zakres aderesów multicast 224.0.0.0 do 239.255.255.255 
	* Komputer wysylajacy nie koniecznie musi byc dodany do grupy multicastingowej.
	* Dane wysy³amy na adres multicastingowy 
	*/
	
	struct ip_mreq mreq;
	mreq.imr_interface.s_addr = inet_addr(LOCAL_ADDR);
	mreq.imr_multiaddr.s_addr = inet_addr(MULTICAS_GROUP_ADDR);
	// dodaj adresy do grupy multicast
	setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));

	Packet mPacket = { 0.0, 0.0 };

	mRobot.odometryTimer.registerTimElapsedCallback(odometryCB);
	SoftTimer mOdometryTimer(0.7, 0, printOdometryCB, true);

	//strcpy_s(mPacket.message, sizeof(mPacket.message), "Warszawa Radar");
	//mPacket.cnt = 0;

	auto Timer1start = std::chrono::system_clock::now();
	std::chrono::duration<double> diffTimer1;

	//keep listening for data
	while ( true )	// main loop 
	{
		//printf("Waiting for data...");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		//memset(buf, '\0', BUFLEN);

		auto start = std::chrono::system_clock::now();
		auto MainTimerStart = std::chrono::system_clock::now();

		std::chrono::duration<double> diffTime;

		while ( true ) {

			mRobot.vOdometryEvent(mPacket.v, mPacket.omega);
			mOdometryTimer.timerProcess();

			auto end = std::chrono::system_clock::now();
			diffTime = end - start;
			//try to receive some data, this is a blocking call
			if ((recv_len = recvfrom(s, (char*)&mPacket, BUFLEN, 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR)
			{
				//printf("recvfrom() failed with error code : %d \n", WSAGetLastError());
				//exit(EXIT_FAILURE);
			}
			else 
			{
				char *ip = inet_ntoa(si_other.sin_addr);
				//printf("IP address: %s\n", ip);
				std::cout << "recived!, v: " << mPacket.v << " omega: " << mPacket.omega << " len: " << recv_len << " from ip: " << ip << " --> " << getTimeStr() << std::endl;
				break;
			}

			if (diffTime.count() > RECIVE_TIMEOUT) 
			{
				std::cout << "Recive Timeout!    " << getTimeStr() << std::endl;
				break;
			}

			// drop multicast mebership after 3 sec
			/*
			static int state = 0;
			std::chrono::duration<double> diffTimer1 = end - Timer1start;
			if (diffTimer1.count() >= 3.0 && !state)
			{
				state = 1;
				std::cout << __LINE__ << " drop multicast mebership group" << std::endl;
				auto err = setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));

				mreq.imr_interface.s_addr = inet_addr(LOCAL_ADDR);
				mreq.imr_multiaddr.s_addr = inet_addr("224.1.1.20");
				// dodaj adresy do grupy multicast
				setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));

				if (err != 0) std::cout << __LINE__ << " setsockopt ERR: " << err << std::endl;
			}
			*/

		}


		//print details of the client/peer and the data received
		//  printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		//printf("Data: %s\n", buf);
		
		// printf("%d\n", mPacket.cnt);

		// now reply the client with the same data
		// mPacket.cnt++;
		if (sendto(s, (const char*) &mPacket, recv_len, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			//printf("sendto() failed with error code : %d \n", WSAGetLastError());
			//exit(EXIT_FAILURE);
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}