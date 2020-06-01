#pragma once

typedef unsigned char byte;

typedef struct rom
{
	byte* memory;
	unsigned short size;
} rom;