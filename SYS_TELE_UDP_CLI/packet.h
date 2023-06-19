#pragma once

#include <stdio.h> 
#include <string>

#pragma pack(1)
typedef struct {
	
	float v;
	float omega;

}Packet;

typedef struct {

	char msg[64u];

	uint32_t cnt;

}Packet1;
