#include "phoenixArcadeMachine.h"

void startEmulation(phoenixArcadeMachine* machine)
{
	byte exit = 0;

	initMachine(machine);
	
	readFileToMemory(machine->i8085->memory, "Game\\ic45", 0x0000);
	readFileToMemory(machine->i8085->memory, "Game\\ic46", 0x0800);
	readFileToMemory(machine->i8085->memory, "Game\\ic47", 0x1000);
	readFileToMemory(machine->i8085->memory, "Game\\ic48", 0x1800);
	readFileToMemory(machine->i8085->memory, "Game\\h5-ic49.5a", 0x2000);
	readFileToMemory(machine->i8085->memory, "Game\\h6-ic50.6a", 0x2800);
	readFileToMemory(machine->i8085->memory, "Game\\h7-ic51.7a", 0x3000);
	readFileToMemory(machine->i8085->memory, "Game\\h8-ic52.8a", 0x3800);

	readFileToMemory(machine->bgtiles->memory, "Game\\ic23.3d", 0x0000);
	readFileToMemory(machine->bgtiles->memory, "Game\\ic24.4d", 0x0800);

	readFileToMemory(machine->fgtiles->memory, "Game\\b1-ic39.3b", 0x0000);
	readFileToMemory(machine->fgtiles->memory, "Game\\b2-ic40.4b", 0x0800);

	readFileToMemory(machine->proms->memory, "Game\\mmi6301.ic40", 0x0000);
	readFileToMemory(machine->proms->memory, "Game\\mmi6301.ic41", 0x0100);

	unsigned int timer = SDL_GetTicks();

	while (!exit)
	{
		if (SDL_PollEvent(&machine->sdlEvent) != 0)
		{
			if (machine->sdlEvent.type == SDL_QUIT)
			{
				exit = 1;
			}

			else if (machine->sdlEvent.type == SDL_KEYDOWN)
			{
				const unsigned int key = machine->sdlEvent.key.keysym.scancode;
			}

			else if (machine->sdlEvent.type == SDL_WINDOWEVENT && machine->sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				SDL_RenderSetScale(machine->renderer, (float)machine->sdlEvent.window.data1 / SCREEN_WIDTH, (float)machine->sdlEvent.window.data2 / SCREEN_HEIGHT);
			}
		}

		if (SDL_GetTicks() - timer > (1.f / 60) * 1000)
		{
			timer = SDL_GetTicks();
			machineUpdate(machine);
			draw(machine);
		}
	}

	freeMachine(machine);
}

void machineUpdate(phoenixArcadeMachine* machine)
{
	unsigned int cyclesCount = 0;
	unsigned int currentCycles = 0;

	while (cyclesCount <= CYCLES_PER_FRAME)
	{
		//printMemoryToFile(machine);
		currentCycles = machine->i8085->cycles;
		byte* instruction = &machine->i8085->memory[machine->i8085->pc];

		emulate8085Op(machine->i8085);
		cyclesCount += machine->i8085->cycles - currentCycles;

		if (*instruction == 0xdb)
		{
			printf("IN ADDRESS %x", instruction[1]);
		}
		else if (*instruction == 0xd3) //OUT    
		{
			printf("OUT ADDRESS %x", instruction[1]);
		}
		printf("PC: %x, SP: %x\n", machine->i8085->pc, machine->i8085->sp);
		printf("A: %x, B: %x, C: %x, D: %x, E: %x, H: %x, L: %x, M: %x\n", machine->i8085->a, machine->i8085->b, machine->i8085->c, machine->i8085->d, machine->i8085->e, machine->i8085->h, machine->i8085->l, machine->i8085->memory[(machine->i8085->h << 8) | machine->i8085->l]);
	}
}

void draw(phoenixArcadeMachine* machine)
{
	SDL_SetRenderDrawColor(machine->renderer, 0, 0, 0, 0);
	SDL_RenderClear(machine->renderer);
	SDL_RenderPresent(machine->renderer);
}

void initMachine(phoenixArcadeMachine* machine)
{
	machine->i8085 = (i8085*)malloc(sizeof(i8085));
	initCPU(machine->i8085);

	machine->bgtiles = (rom*)malloc(sizeof(rom));
	machine->fgtiles = (rom*)malloc(sizeof(rom));
	machine->proms = (rom*)malloc(sizeof(rom));

	machine->bgtiles->memory = (byte*)malloc(sizeof(byte) * BGTILES_SIZE);
	machine->fgtiles->memory = (byte*)malloc(sizeof(byte) * FGTILES_SIZE);
	machine->proms->memory = (byte*)malloc(sizeof(byte) * PROMS_SIZE);

	machine->bgtiles->size = BGTILES_SIZE;
	machine->fgtiles->size = FGTILES_SIZE;
	machine->proms->size = PROMS_SIZE;

	machine->i8085->data = (phoenixArcadeMachine*)machine;
	machine->i8085->writeMemory = wb;
	machine->i8085->readMemory = rb;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &machine->screen, &machine->renderer);
	SDL_SetWindowResizable(machine->screen, SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);
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

void readFileToMemory(byte* memory, char* fileName, unsigned short offset)
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

	byte* buffer = &memory[offset];
	fread(buffer, fileSize, 1, f);
	fclose(f);
}

void freeMachine(phoenixArcadeMachine* machine)
{
	SDL_DestroyRenderer(machine->renderer);
	SDL_DestroyWindow(machine->screen);

	free(machine->bgtiles->memory);
	free(machine->fgtiles->memory);
	free(machine->proms->memory);
	free(machine->bgtiles);
	free(machine->fgtiles);
	free(machine->proms);

	free(machine->i8085->memory);
	free(machine->i8085);
}

void wb(void* data, unsigned short addr, byte value)
{
	phoenixArcadeMachine* machine = (phoenixArcadeMachine*)data;
	if (addr < 0x4000)
	{
		printf("Writing ROM not allowed %x\n", addr);
		return;
	}

	machine->i8085->memory[addr] = value;
}

byte rb(void* data, unsigned short addr)
{
	phoenixArcadeMachine* machine = (phoenixArcadeMachine*)data;

	if (addr < 0x4000)
	{
		return machine->i8085->memory[addr];
	}
	else if (addr >= 0x7800 && addr < 0x7C00)
	{
		if (machine->dswSwitch)
		{
			machine->dswSwitch = 0;
			return 0;
		}
		else
		{
			machine->dswSwitch = 1;
			return 0x80;
		}
	}
	return 0;
}

void printMemoryToFile(phoenixArcadeMachine* machine)
{
	FILE* f = fopen("d.bin", "wb");

	if (!f)
	{
		printf("Can't open file!\n");
		return;
	}
	fwrite(machine->i8085->memory, MEMORY_SIZE, 1, f);

	fclose(f);
}
