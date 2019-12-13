#pragma once
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "i8085.h"
#include "rom.h"

#define MEMORY_SIZE 0x10000 //64K
#define TILES_SIZE 0x2000
#define PROMS_SIZE 0x200
#define PALETTE_SIZE 0x80
#define BGTILES_MEMORY_START 0x4800
#define FGTILES_MEMORY_START 0x4000

#define CYCLES_PER_FRAME 740000 / 60
#define CYCLES_TO_VBLANK 12000

#define SCREEN_HEIGHT 208
#define SCREEN_WIDTH 256

typedef unsigned int u32;
typedef unsigned short u16;

typedef struct phoenixArcadeMachine
{
	SDL_Window* screen;
	SDL_Renderer* renderer;
	SDL_Event sdlEvent;

	i8085* i8085;
	rom* tiles;
	rom* proms;

	byte dswSwitch;

	byte videoControl;
	byte scrollReg;
	u32 palette[PALETTE_SIZE];
	
	u32**** characters;

	/*
		Active low.
		bit 0 - Coin
		bit 1 - Start 1
		bit 2 - Start 2
		bit 3 - Unused 
		bit 4 - Fire
		bit 5 - Right
		bit 6 - Left
		bit 7 - Barrier
	*/
	byte inPort; 
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
	Will draw a single tile (8x8 pixels), will start drawing 8*8 tile from the xy and y
	Input: A pointer to the PhoenixArcadeMachine struct, the x coordinate, the y coordinate, the character array, the position in the memory to get the wanted tile, offset to which character to take in the array (for example all the background characters start from 0x100)
*/
void drawTile(phoenixArcadeMachine* machine, byte x, byte y, u32*** characterArr, u16 memoryPos, u16 characterOffset);

/*
	Will initiate the machine (allocate memory and reset all the struct stuff)
	Input: A pointer to the phoenixArcadeMachine struct
*/
void initMachine(phoenixArcadeMachine* machine);

/*
	Will initiate the CPU (allocate memory and reset all registers)
	input: A pointer to the i8085 struct
*/
void initCPU(i8085* i8085);

/*
	Will generate the palette from the palette roms
	Input: A pointer to the Phoenix Arcade Machine struct
*/
void makePalette(phoenixArcadeMachine* machine);

/*
	Will generate the characters of the graphics (8*8 kind of tiles)
	Input: A pointer to the Phoenix arcade machine struct, The index of the palette(0, 1)
*/
void generateCharacters(phoenixArcadeMachine* machine, byte paletteIndex);

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