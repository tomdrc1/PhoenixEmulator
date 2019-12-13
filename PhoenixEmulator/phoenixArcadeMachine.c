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

	readFileToMemory(machine->tiles->memory, "Game\\b1-ic39.3b", 0x0000);
	readFileToMemory(machine->tiles->memory, "Game\\ic23.3d", 0x0800);
	readFileToMemory(machine->tiles->memory, "Game\\b2-ic40.4b", 0x1000);
	readFileToMemory(machine->tiles->memory, "Game\\ic24.4d", 0x1800);

	readFileToMemory(machine->proms->memory, "Game\\mmi6301.ic40", 0x0000);
	readFileToMemory(machine->proms->memory, "Game\\mmi6301.ic41", 0x0100);

	makePalette(machine);
	generateCharacters(machine, 0);
	generateCharacters(machine, 1);
	
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
				switch (key)
				{
					case SDL_SCANCODE_C:
						machine->inPort &= ~(1 << 0); // Coin
						break;
					case SDL_SCANCODE_RETURN:
						machine->inPort &= ~(1 << 1); // Start 1 player
						break;
					case SDL_SCANCODE_2:
						machine->inPort &= ~(1 << 2); // Start 2 player
						break;
					case SDL_SCANCODE_SPACE:
						machine->inPort &= ~(1 << 4); // Fire
						break;
					case SDL_SCANCODE_RIGHT:
						machine->inPort &= ~(1 << 5); // Player move right
						break;
					case SDL_SCANCODE_LEFT:
						machine->inPort &= ~(1 << 6); // Player move left
						break;
					case SDL_SCANCODE_X:
						machine->inPort &= ~(1 << 7); // Shield
						break;
				}
			}

			else if (machine->sdlEvent.type == SDL_KEYUP)
			{
				const unsigned int key = machine->sdlEvent.key.keysym.scancode;
				switch (key)
				{
				case SDL_SCANCODE_C:
					machine->inPort |= (1 << 0); // Coin
					break;
				case SDL_SCANCODE_RETURN:
					machine->inPort |= (1 << 1); // Start 1 player
					break;
				case SDL_SCANCODE_2:
					machine->inPort |= (1 << 2); // Start 2 player
					break;
				case SDL_SCANCODE_SPACE:
					machine->inPort |= (1 << 4); // Fire
					break;
				case SDL_SCANCODE_RIGHT:
					machine->inPort |= (1 << 5); // Player move right
					break;
				case SDL_SCANCODE_LEFT:
					machine->inPort |= (1 << 6); // Player move left
					break;
				case SDL_SCANCODE_X:
					machine->inPort |= (1 << 7); // Shield
					break;
				}
			}
			else if (machine->sdlEvent.type == SDL_WINDOWEVENT && machine->sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				SDL_RenderSetScale(machine->renderer, (float)machine->sdlEvent.window.data1 / SCREEN_HEIGHT, (float)machine->sdlEvent.window.data2 / SCREEN_WIDTH);
			}
		}

		if (SDL_GetTicks() - timer > (1.f / 60) * 1000)
		{
			timer = SDL_GetTicks();
			machineUpdate(machine);
			draw(machine);
			machine->dswSwitch = 1;
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
		currentCycles = machine->i8085->cycles;
		byte* instruction = &machine->i8085->memory[machine->i8085->pc];

		emulate8085Op(machine->i8085);
		cyclesCount += machine->i8085->cycles - currentCycles;


		
		if (machine->i8085->cycles >= CYCLES_TO_VBLANK)
		{
			machine->i8085->cycles -= CYCLES_TO_VBLANK;
			machine->dswSwitch = 1;
			//draw(machine);
		}
	}
}

void draw(phoenixArcadeMachine* machine)
{
	SDL_SetRenderDrawColor(machine->renderer, 0, 0, 0, 0);
	SDL_RenderClear(machine->renderer);
	SDL_SetRenderDrawColor(machine->renderer, 255, 255, 255, 255);

	byte paletteControl = (machine->videoControl & 0x02) >> 1;
	
	u32*** characterArr = machine->characters[paletteControl];
	
	byte y = 0, x = 0;
	byte i = 0, j = 0;
	
	u16 memoryPos = 0;

	for (y = 0; y < 32; ++y)
	{
		memoryPos = BGTILES_MEMORY_START + 32 * 25 + y;

		// The reason for this "Real Y" is because the scroll register will tell us where to start in the screen (For scrolling effect)
		byte realY = ((y * 8) + (256 - machine->scrollReg)) & 0xFF;

		for (x = 0; x < 26; ++x)
		{
			drawTile(machine, x * 8, realY, characterArr, memoryPos, 0x100);
			memoryPos -= 32;
		}
	}

	for (y = 0; y < 32; ++y)
	{
		memoryPos = FGTILES_MEMORY_START + 32 * 25 + y;
		for (x = 0; x < 26; ++x)
		{
			drawTile(machine, x * 8, y * 8, characterArr, memoryPos, 0);
			memoryPos -= 32;
		}
	}

	SDL_RenderPresent(machine->renderer);
}

void drawTile(phoenixArcadeMachine* machine, byte x, byte y, u32*** characterArr, u16 memoryPos, u16 characterOffset)
{
	byte i = 0, j = 0;

	for (i = 0; i < 8; ++i)
	{
		for (j = 0; j < 8; ++j)
		{
			u32 color = characterArr[characterOffset + machine->i8085->memory[memoryPos]][i][j];

			if (color == 0xFF000000)
			{
				continue;
			}
			SDL_SetRenderDrawColor(machine->renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF);
			SDL_RenderDrawPoint(machine->renderer, x + i, y + j);
		}
	}
}

void initMachine(phoenixArcadeMachine* machine)
{
	machine->i8085 = (i8085*)malloc(sizeof(i8085));
	initCPU(machine->i8085);

	machine->tiles = (rom*)malloc(sizeof(rom));
	machine->proms = (rom*)malloc(sizeof(rom));

	machine->tiles->memory = (byte*)malloc(sizeof(byte) * TILES_SIZE);
	machine->proms->memory = (byte*)malloc(sizeof(byte) * PROMS_SIZE);

	machine->tiles->size = TILES_SIZE;
	machine->proms->size = PROMS_SIZE;

	machine->inPort = 0xFF;
	machine->dswSwitch = 0;
	machine->scrollReg = 0;
	machine->videoControl = 0;

	machine->i8085->data = (phoenixArcadeMachine*)machine;
	machine->i8085->writeMemory = wb;
	machine->i8085->readMemory = rb;

	machine->characters = (u32****)malloc(sizeof(u32***) * 2);

	machine->characters[0] = (u32***)malloc(sizeof(u32**) * 0x200);
	machine->characters[1] = (u32***)malloc(sizeof(u32**) * 0x200);

	u16 i = 0;
	for (i = 0; i < 0x200; ++i)
	{
		machine->characters[0][i] = (u32**)malloc(sizeof(u32*) * 8);
		machine->characters[1][i] = (u32**)malloc(sizeof(u32*) * 8);

		machine->characters[0][i][0] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[0][i][1] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[0][i][2] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[0][i][3] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[0][i][4] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[0][i][5] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[0][i][6] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[0][i][7] = (u32*)malloc(sizeof(u32) * 8);

		machine->characters[1][i][0] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[1][i][1] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[1][i][2] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[1][i][3] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[1][i][4] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[1][i][5] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[1][i][6] = (u32*)malloc(sizeof(u32) * 8);
		machine->characters[1][i][7] = (u32*)malloc(sizeof(u32) * 8);
	}

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	SDL_CreateWindowAndRenderer(SCREEN_HEIGHT, SCREEN_WIDTH, 0, &machine->screen, &machine->renderer);
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

void makePalette(phoenixArcadeMachine* machine)
{
	int i = 0;

	byte plane0 = 0;
	byte plane1 = 0;

	byte r = 0, g = 0, b = 0;
	for (i = 0; i < PALETTE_SIZE; ++i)
	{
		plane0 = machine->proms->memory[i];
		plane1 = machine->proms->memory[i + 0x100];
		
		r = (((plane0 >> 0) & 0x01) | (((plane1 >> 0) & 0x01) << 1)) * 85;
		g = (((plane0 >> 2) & 0x01) | (((plane1 >> 2) & 0x01) << 1)) * 85;
		b = (((plane0 >> 1) & 0x01) | (((plane1 >> 1) & 0x01) << 1)) * 85;

		machine->palette[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
	}
}

void generateCharacters(phoenixArcadeMachine* machine, byte paletteIndex)
{
	byte charSector = 0, charBank = 0;
	byte line1 = 0, line2 = 0;
	u16 charIndex = 0;

	byte i = 0, j = 0;
	byte mask = 0;
	byte bit1 = 0, bit2 = 0;
	byte paletteLookup = 0;
	u32*** charactersArr = machine->characters[paletteIndex];

	for (charIndex = 0; charIndex < 0x200; charIndex++)
	{
		charSector = ((charIndex & 0xFF) >> 5);
		charBank = (charIndex & 0x100) == 0 ? 1 : 0;

		for (i = 0; i < 8; i++)
		{
			// +7 -i to flip the inverted data in the memory (Take the last first and the first last)
			line1 = machine->tiles->memory[(charIndex * 8) + 7 - i];
			line2 = machine->tiles->memory[(charIndex * 8) + 7 - i + 0x1000];

			for (j = 0; j < 8; ++j)
			{
				mask = 1 << j;

				bit1 = (line1 & mask) >> j;
				bit2 = (line2 & mask) >> j;

				paletteLookup = charSector | (bit1 << 3) | (bit2 << 4) | (charBank << 5) | (paletteIndex << 6);

				charactersArr[charIndex][i][j] = machine->palette[paletteLookup];
			}
		}
	}
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

	u16 i = 0;

	for (i = 0; i < 0x200; ++i)
	{
		free(machine->characters[0][i][0]);
		free(machine->characters[0][i][1]);
		free(machine->characters[0][i][2]);
		free(machine->characters[0][i][3]);
		free(machine->characters[0][i][4]);
		free(machine->characters[0][i][5]);
		free(machine->characters[0][i][6]);
		free(machine->characters[0][i][7]);

		free(machine->characters[1][i][0]);
		free(machine->characters[1][i][1]);
		free(machine->characters[1][i][2]);
		free(machine->characters[1][i][3]);
		free(machine->characters[1][i][4]);
		free(machine->characters[1][i][5]);
		free(machine->characters[1][i][6]);
		free(machine->characters[1][i][7]);

		free(machine->characters[0][i]);
		free(machine->characters[1][i]);
	}

	free(machine->characters[0]);
	free(machine->characters[1]);
	
	free(machine->characters);


	free(machine->tiles->memory);
	free(machine->proms->memory);
	free(machine->tiles);
	free(machine->proms);

	free(machine->i8085->memory);
	free(machine->i8085);
}

void wb(void* data, unsigned short addr, byte value)
{
	phoenixArcadeMachine* machine = (phoenixArcadeMachine*)data;

	// 0x0000 - 0x3FFF ROM
	if (addr < 0x4000) 
	{
		return;
	}
	if ((addr >= 0x4000 && addr <= 0x43FF) || (addr >= 0x4800 && addr <= 0x4BFF)) // 0x4000 - 0x43FF FG characters, 0x4800 - 0x4BFF BG characters
	{
		machine->i8085->memory[addr] = value;
	}
	else if (addr >= 0x5000 && addr <= 0x53FF)
	{
		machine->videoControl = value;
	}	
	else if (addr >= 0x5800 && addr <= 0x5BFF)
	{
		machine->scrollReg = value;
	}
	else if (addr >= 0x6000 && addr <= 0x63FF)
	{
		printf("Wrote to data: %x\n", value);
	}
	else if (addr >= 0x6800 && addr <= 0x6BFF)
	{
		printf("Wrote to data: %x\n", value);
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
	else if (addr >= 0x7000 && addr <= 0x73FF)
	{
		return machine->inPort;
	}
	else if ((addr >= 0x4000 && addr <= 0x43FF) || (addr >= 0x4800 && addr <= 0x4BFF))
	{
		return machine->i8085->memory[addr];
	}
	else if (addr >= 0x7800 && addr <= 0x7BFF)
	{
		if (machine->dswSwitch)
		{
			machine->dswSwitch = 0;
			return 0x80;
		}
		else
		{
			return 0;
		}
	}
	return machine->i8085->memory[addr];
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
