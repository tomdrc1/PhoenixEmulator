#include "phoenixArcadeMachine.h"

void startEmulation(phoenixArcadeMachine* machine)
{
	initMachine(machine);
}

void initMachine(phoenixArcadeMachine* machine)
{
	machine->i8080 = (State8080*)malloc(sizeof(State8080));
	initCPU(machine->i8080);
}

void initCPU(State8080* i8080)
{
	i8080->a = 0;
	i8080->b = 0;
	i8080->c = 0;
	i8080->d = 0;
	i8080->e = 0;
	i8080->h = 0;
	i8080->l = 0;

	i8080->pc = 0;
	i8080->sp = 0;

	i8080->memory = (byte*)malloc(MEMORY_SIZE * sizeof(byte));
	memset(i8080->memory, NULL, MEMORY_SIZE * sizeof(byte));

	i8080->cc.z = 0;
	i8080->cc.s = 0;
	i8080->cc.p = 0;
	i8080->cc.cy = 0;
	i8080->cc.ac = 0;
	i8080->cc.pad1 = 1;
	i8080->cc.pad2 = 0;
	i8080->cc.pad3 = 0;

	i8080->int_enable = 0;
	i8080->cycles = 0;
}

void freeMachine(phoenixArcadeMachine* machine)
{
	free(machine->i8080->memory);
	free(machine->i8080);
}
