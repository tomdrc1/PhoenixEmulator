#pragma once
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "i8085.h"

#define MEMORY_SIZE 0x10000 //64K

typedef struct phoenixArcadeMachine
{
	i8085* i8085;


} phoenixArcadeMachine;

void startEmulation(phoenixArcadeMachine* machine);

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
	 Input: A pointer to the i8085 struct, the file name, and the offset
*/
void readFileToMemory(i8085* i8085, char* fileName, unsigned short offset);

/*
	Will free all the allocation we made in the `init` function
	Input: A pointer to the phoenixArcadeMachine struct
*/
void freeMachine(phoenixArcadeMachine* machine);