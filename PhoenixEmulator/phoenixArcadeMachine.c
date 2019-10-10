#include "phoenixArcadeMachine.h"

void startEmulation(phoenixArcadeMachine* machine)
{
	byte exit = 0;

	initMachine(machine);
	
	readFileToMemory(machine->i8085, "Phoenix.rom", 0);

	while (!exit)
	{
		emulate8085Op(machine->i8085);
	}

	freeMachine(machine);
}

void initMachine(phoenixArcadeMachine* machine)
{
	machine->i8085 = (i8085*)malloc(sizeof(i8085));
	initCPU(machine->i8085);
}

void initCPU(i8085* i8085)
{
	i8085->a = 0;
	i8085->b = 0;
	i8085->c = 0;
	i8085->d = 0;
	i8085->e = 0;
	i8085->h = 0;
	i8085->l = 0;

	i8085->pc = 0;
	i8085->sp = 0;

	i8085->memory = (byte*)malloc(MEMORY_SIZE * sizeof(byte));
	memset(i8085->memory, NULL, MEMORY_SIZE * sizeof(byte));

	i8085->cc.z = 0;
	i8085->cc.s = 0;
	i8085->cc.p = 0;
	i8085->cc.cy = 0;
	i8085->cc.ac = 0;
	i8085->cc.pad1 = 1;
	i8085->cc.pad2 = 0;
	i8085->cc.pad3 = 0;

	i8085->int_enable = 0;
	i8085->cycles = 0;
}

void readFileToMemory(i8085* i8085, char* fileName, unsigned short offset)
{
	FILE* f = fopen(fileName, "rb");

	if (!f)
	{
		printf("Error can't open file %s\n", fileName);
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	int fileSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	byte* buffer = &i8085->memory[offset];
	fread(buffer, fileSize, 1, f);
	fclose(f);
}

void freeMachine(phoenixArcadeMachine* machine)
{
	free(machine->i8085->memory);
	free(machine->i8085);
}
