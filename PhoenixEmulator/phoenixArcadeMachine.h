#pragma once
#include <stdlib.h>
#include <string.h>
#include "i8080.h"

#define MEMORY_SIZE 0x10000 //64K

typedef struct phoenixArcadeMachine
{
	State8080* i8080;


} phoenixArcadeMachine;

void startEmulation(phoenixArcadeMachine* machine);

/*
	Will initiate the machine (allocate memory and reset all the struct stuff)
	Input: A pointer to the phoenixArcadeMachine struct
*/
void initMachine(phoenixArcadeMachine* machine);

/*
	Will initiate the machine (allocate memory and reset all registers)
	input: A pointer to the State8080 struct
*/
void initCPU(State8080* i8080);

/*
	Will free all the allocation we made in the `init` function
	Input: A pointer to the phoenixArcadeMachine struct
*/
void freeMachine(phoenixArcadeMachine* machine);