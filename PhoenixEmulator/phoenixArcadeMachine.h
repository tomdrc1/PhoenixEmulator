#pragma once
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "i8085.h"
#include "rom.h"

#define MEMORY_SIZE 0x10000 //64K
#define FGTILES_SIZE 0x1000
#define BGTILES_SIZE 0x1000
#define PROMS_SIZE 0x200

#define CYCLES_PER_FRAME 5500000 / 60

#define SCREEN_HEIGHT 208
#define SCREEN_WIDTH 256

typedef struct phoenixArcadeMachine
{
	SDL_Window* screen;
	SDL_Renderer* renderer;
	SDL_Event sdlEvent;

	i8085* i8085;
	rom* bgtiles;
	rom* fgtiles;
	rom* proms;

	byte dswSwitch;
} phoenixArcadeMachine;

void startEmulation(phoenixArcadeMachine* machine);

/*
	Will update the machine (Exectue 5500000 / 60 cycles)
	Input: A pointer to the phoenixArcadeMachine struct
*/
void machineUpdate(phoenixArcadeMachine* machine);

/*
	Will draw to the screen every interrupt / frame
	Input: A pointer to the phoenixArcadeMachine struct
*/
void draw(phoenixArcadeMachine* machine);

/*
	Will initiate the machine (allocate memory and reset all the struct stuff)
	Input: A pointer to the phoenixArcadeMachine struct
*/
void initMachine(phoenixArcadeMachine* machine);

/*
	Will initiate the machine (allocate memory and reset all registers)
	input: A pointer to the i8085 struct
*/
void initCPU(i8085* i8085);

/*
	 Will read a file to the memory with the offset
	 Input: A pointer to the memory, the file name, and the offset
*/
void readFileToMemory(byte* memory, char* fileName, unsigned short offset);

/*
	Will free all the allocation we made in the `init` function
	Input: A pointer to the phoenixArcadeMachine struct
*/
void freeMachine(phoenixArcadeMachine* machine);

/*
	Will write to the memory of the cpu of the machine. Will be used as a pointer function
	input: A void pointer to the machine, the 16 bit address, the 8 bit value
*/
void wb(void* data, unsigned short addr, byte value);

/*
	Will return the byte data in the address. Will be used as a pointer function
	input: A void pointer to the machine, the 16 bit address
*/
byte rb(void* data, unsigned short addr);
void printMemoryToFile(phoenixArcadeMachine* machine);