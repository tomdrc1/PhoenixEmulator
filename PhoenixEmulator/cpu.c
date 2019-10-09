#include "cpu.h"

byte pairtyCheck(byte num, byte size)
{
	byte count = 0;
	byte i = 0;

	for (i = 0; i < size; i++)
	{
		if ((num & 0x01) == 0x01)
		{
			count++;
		}

		num >>= 1;
	}

	return (count % 2 == 0);
}

byte emulate8080Op(State8080* state)
{
	byte* instruction = &state->memory[state->pc];

	state->cycles += OPCODES_CYCLES[*instruction];
	state->pc++;

	switch (instruction[0])
	{
		case 0x00:
			break;
		case 0x01:
			state->c = instruction[1];
			state->b = instruction[2];
			state->pc += 2;
			break;
		case 0x02:
			{
				unsigned short bc = (state->b << 8) | state->c;
				writeToMemory(state, bc, state->a);
			}
			break;
		case 0x03:
			{
				unsigned short bc = (state->b << 8) | state->c;
				bc++;

				state->b = (bc & 0xFF00) >> 8;
				state->c = bc & 0x00FF;
			}
			break;
		case 0x04:
			{
				state->cc.ac = ((((state->b & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10);
				byte res = state->b + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->b = res;
			}
			break;
		case 0x05:
			{
				state->cc.ac = (((state->b & 0x0F) - (0x01 & 0x0F)) < 0);
				byte res = state->b - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->b = res;
			}
			break;
		case 0x06:
			state->b = instruction[1];
			state->pc++;
			break;
		case 0x07:
			state->cc.cy = (state->a & 0x80) == 0x80;
			state->a <<= 1;
			state->a = ((state->a & 0xFE) | state->cc.cy);
			break;
		case 0x08:
			break;
		case 0x09:
			{
				unsigned short bc = (state->b << 8) | state->c;
				unsigned short hl = (state->h << 8) | state->l;

				unsigned int res = hl + bc;

				state->h = (res & 0x0000FF00) >> 8;
				state->l = (res & 0x000000FF);

				state->cc.cy = ((res & 0xFFFF0000) != 0);
			}
			break;
		case 0x0A:
			{
				unsigned short bc = (state->b << 8) | state->c;
				state->a = state->memory[bc];
			}
			break;
		case 0x0B:
			{
				unsigned short bc = (state->b << 8) | state->c;
				bc--;

				state->b = (bc & 0xFF00) >> 8;
				state->c = (bc & 0x00FF);
			}
			break;
		case 0x0C:
			{
				state->cc.ac = ((((state->c & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10);
				byte res = state->c + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->c = res;
			}
			break;
		case 0x0D:
			{
				state->cc.ac = (((state->c & 0x0F) - (0x01 & 0x0F)) < 0);
				byte res = state->c - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->c = res;
			}
			break;
		case 0x0E:
			state->c = instruction[1];
			state->pc++;
			break;
		case 0x0F:
			state->cc.cy = (state->a & 0x01) == 0x01;
			state->a >>= 1;
			state->a = ((state->a & 0x7F) | (state->cc.cy << 7));
			break;
		case 0x10:
			break;
		case 0x11:
			state->e = instruction[1];
			state->d = instruction[2];
			state->pc += 2;
			break;
		case 0x12:
			{
				unsigned short de = (state->d << 8) | state->e;
				writeToMemory(state, de, state->a);
			}
			break;
		case 0x13:
			{
				unsigned short de = (state->d << 8) | state->e;
				de++;

				state->d = (de & 0xFF00) >> 8;
				state->e = de & 0x00FF;
			}
			break;
		case 0x14:
			{
				state->cc.ac = ((((state->d & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10);
				byte res = state->d + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->d = res;
			}
			break;
		case 0x15:
			{
				state->cc.ac = (((state->d & 0x0F) - (0x01 & 0x0F)) < 0);
				byte res = state->d - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->d = res;
			}
			break;
		case 0x16:
			state->d = instruction[1];
			state->pc++;
			break;
		case 0x17:
			{
				byte prevCY = state->cc.cy;
				state->cc.cy = (state->a & 0x80) == 0x80;
				state->a <<= 1;
				state->a = (state->a & 0xFE) | prevCY;
			}
			break;
		case 0x18:
			break;
		case 0x19:
			{
				unsigned short hl = (state->h << 8) | state->l;
				unsigned short de = (state->d << 8) | state->e;

				unsigned int res = hl + de;

				state->h = (res & 0x0000FF00) >> 8;
				state->l = res & 0x000000FF;

				state->cc.cy = ((res & 0xFFFF0000) != 0);
			}
			break;
		case 0x1A:
			{
				unsigned short de = (state->d << 8) | state->e;
				state->a = state->memory[de];
			}
			break;
		case 0x1B:
			{
				unsigned short de = (state->d << 8) | state->e;
				de--;

				state->d = (de & 0xFF00) >> 8;
				state->e = de & 0x00FF;
			}
			break;
		case 0x1C:
			{
				state->cc.ac = ((((state->e & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10);
				byte res = state->e + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->e = res;
			}
			break;
		case 0x1D:
			{
				state->cc.ac = (((state->e & 0x0F) - (0x01 & 0x01)) < 0);
				byte res = state->e - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->e = res;
			}
			break;
		case 0x1E:
			state->e = instruction[1];
			state->pc++;
			break;
		case 0x1F:
			{
				byte prevCy = state->cc.cy;
				state->cc.cy = (state->a & 0x01) == 0x01;
				state->a >>= 1;
				state->a = (state->a & 0x7F) | (prevCy << 7);
			}
			break;
		case 0x20:
			break;
		case 0x21:
			state->l = instruction[1];
			state->h = instruction[2];
			state->pc += 2;
			break;
		case 0x22:
			{
				unsigned short adr = (instruction[2] << 8) | instruction[1];
				writeToMemory(state, adr, state->l);
				writeToMemory(state, adr + 1, state->h);
				state->pc += 2;
			}
			break;
		case 0x23:
			{
				unsigned short hl = (state->h << 8) | state->l;
				hl++;

				state->h = (hl & 0xFF00) >> 8;
				state->l = hl & 0x00FF;
			}
			break;
		case 0x24:
			{
				state->cc.ac = (((state->h & 0x0F) + (0x01 & 0x0F) & 0x10) == 0x10);
				byte res = state->h + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->h = res;
			}
			break;
		case 0x25:
			{
				state->cc.ac = ((state->h & 0x0F) - (0x01 & 0x0F) < 0);
				byte res = state->h - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->h = res;
			}
			break;
		case 0x26:
			state->h = instruction[1];
			state->pc++;
			break;
		case 0x27:
			if ((state->a & 0x0F) > 0x09 || state->cc.ac == 0x01) 
			{
				state->cc.ac = (((state->a & 0x0F) + (0x06 & 0x0F)) & 0x10) == 0x10;
				state->a += 0x06;
			}
			if (((state->a >> 4) & 0x0F) > 0x09 || state->cc.cy == 0x01)
			{
				state->cc.cy = ((((state->a >> 4) & 0x0F) + (0x06 & 0x0F)) & 0x10) == 0x10;
				state->a = (state->a) + (0x06 << 4);
			}

			state->cc.z = (0 == state->a);
			state->cc.p = pairtyCheck(state->a, 8);
			state->cc.s = (0x80 == (state->a & 0x80));
			break;
		case 0x28:
			break;
		case 0x29:
			{
				unsigned short hl = (state->h << 8) | state->l;
				
				unsigned int res = hl << 1;

				state->h = (res & 0x0000FF00) >> 8;
				state->l = res & 0x000000FF;

				state->cc.cy = ((res & 0xFFFF0000) != 0);
			}
			break;
		case 0x2A:
			{
				unsigned short adr = (instruction[2] << 8) | instruction[1];

				state->l = state->memory[adr];
				state->h = state->memory[adr + 1];
				state->pc += 2;
			}
			break;
		case 0x2B:
			{
				unsigned short hl = (state->h << 8) | state->l;
				hl--;

				state->h = (hl & 0xFF00) >> 8;
				state->l = hl & 0x00FF;
			}
			break;
		case 0x2C:
			{
				state->cc.ac = (((state->l & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10;
				byte res = state->l + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->l = res;
			}
			break;
		case 0x2D:
			{
				state->cc.ac = ((state->l & 0x0F) - (0x01 & 0x0F) < 0);
				byte res = state->l - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->l = res;
			}
			break;
		case 0x2E:
			state->l = instruction[1];
			state->pc++;
			break;
		case 0x2F:
			state->a = ~state->a;
			break;
		case 0x30:
			break;
		case 0x31:
			state->sp = (instruction[2] << 8) | instruction[1];
			state->pc += 2;
			break;
		case 0x32:
			{
				unsigned short adr = (instruction[2] << 8) | instruction[1];
				writeToMemory(state, adr, state->a);

				state->pc += 2;
			}
			break;
		case 0x33:
			state->sp++;
			break;
		case 0x34:
			{
				unsigned short hl = (state->h << 8) | state->l;
				byte value = state->memory[hl];

				state->cc.ac = (((value & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10;
				byte res = value + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				writeToMemory(state, hl, res);
			}
			break;
		case 0x35:
			{
				unsigned short hl = (state->h << 8) | state->l;
				byte value = state->memory[hl];

				state->cc.ac = ((value & 0x0F) - (0x01 & 0x0F)) < 0;
				byte res = value - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				writeToMemory(state, hl, res);
			}
			break;
		case 0x36:
			{
				unsigned short hl = (state->h << 8) | state->l;
				writeToMemory(state, hl, instruction[1]);

				state->pc++;
			}
			break;
		case 0x37:
			state->cc.cy = 0x01;
			break;
		case 0x38:
			break;
		case 0x39:
			{
				unsigned short hl = (state->h << 8) | state->l;

				unsigned int res = hl + state->sp;

				state->h = (res & 0x0000FF00) >> 8;
				state->l = res & 0x000000FF;

				state->cc.cy = ((res & 0xFFFF0000) != 0);
			}
			break;
		case 0x3A:
			{
				unsigned short adr = (instruction[2] << 8) | instruction[1];
				state->a = state->memory[adr];
				state->pc += 2;
			}
			break;
		case 0x3B:
			state->sp--;
			break;
		case 0x3C:
			{
				state->cc.ac = (((state->a & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10;
				byte res = state->a + 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->a = res;
			}
			break;
		case 0x3D:
			{
				state->cc.ac = (((state->a & 0x0F) - (0x01 & 0x0F)) < 0);
				byte res = state->a - 1;

				state->cc.z = (0 == res);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = pairtyCheck(res, 8);

				state->a = res;
			}
			break;
		case 0x3E:
			state->a = instruction[1];
			state->pc++;
			break;
		case 0x3F:
			state->cc.cy = !state->cc.cy;
			break;
		case 0x40:
			state->b = state->b;
			break;
		case 0x41:
			state->b = state->c;
			break;
		case 0x42:
			state->b = state->d;
			break;
		case 0x43:
			state->b = state->e;
			break;
		case 0x44:
			state->b = state->h;
			break;
		case 0x45:
			state->b = state->l;
			break;
		case 0x46:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->b = state->memory[hl];
			}
			break;
		case 0x47:
			state->b = state->a;
			break;
		case 0x48:
			state->c = state->b;
			break;
		case 0x49:
			state->c = state->c;
			break;
		case 0x4A:
			state->c = state->d;
			break;
		case 0x4B:
			state->c = state->e;
			break;
		case 0x4C:
			state->c = state->h;
			break;
		case 0x4D:
			state->c = state->l;
			break;
		case 0x4E:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->c = state->memory[hl];
			}
			break;
		case 0x4F:
			state->c = state->a;
			break;
		case 0x50:
			state->d = state->b;
			break;
		case 0x51:
			state->d = state->c;
			break;
		case 0x52:
			state->d = state->d;
			break;
		case 0x53:
			state->d = state->e;
			break;
		case 0x54:
			state->d = state->h;
			break;
		case 0x55:
			state->d = state->l;
			break;
		case 0x56:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->d = state->memory[hl];
			}
			break;
		case 0x57:
			state->d = state->a;
			break;
		case 0x58:
			state->e = state->b;
			break;
		case 0x59:
			state->e = state->c;
			break;
		case 0x5A:
			state->e = state->d;
			break;
		case 0x5B:
			state->e = state->e;
			break;
		case 0x5C:
			state->e = state->h;
			break;
		case 0x5D:
			state->e = state->l;
			break;
		case 0x5E:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->e = state->memory[hl];
			}
			break;
		case 0x5F:
			state->e = state->a;
			break;
		case 0x60:
			state->h = state->b;
			break;
		case 0x61:
			state->h = state->c;
			break;
		case 0x62:
			state->h = state->d;
			break;
		case 0x63:
			state->h = state->e;
			break;
		case 0x64:
			state->h = state->h;
			break;
		case 0x65:
			state->h = state->l;
			break;
		case 0x66:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->h = state->memory[hl];
			}
			break;
		case 0x67:
			state->h = state->a;
			break;
		case 0x68:
			state->l = state->b;
			break;
		case 0x69:
			state->l = state->c;
			break;
		case 0x6A:
			state->l = state->d;
			break;
		case 0x6B:
			state->l = state->e;
			break;
		case 0x6C:
			state->l = state->h;
			break;
		case 0x6D:
			state->l = state->l;
			break;
		case 0x6E:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->l = state->memory[hl];
			}
			break;
		case 0x6F:
			state->l = state->a;
			break;
		case 0x70:
			{
				unsigned short hl = (state->h << 8) | state->l;
				writeToMemory(state, hl, state->b);
			}
			break;
		case 0x71:
			{
				unsigned short hl = (state->h << 8) | state->l;
				writeToMemory(state, hl, state->c);
			}
			break;
		case 0x72:
			{
				unsigned short hl = (state->h << 8) | state->l;
				writeToMemory(state, hl, state->d);
			}
			break;
		case 0x73:
			{
				unsigned short hl = (state->h << 8) | state->l;
				writeToMemory(state, hl, state->e);
			}
			break;
		case 0x74:
			{
				unsigned short hl = (state->h << 8) | state->l;
				writeToMemory(state, hl, state->h);
			}
			break;
		case 0x75:
			{
				unsigned short hl = (state->h << 8) | state->l;
				writeToMemory(state, hl, state->l);
			}
			break;
		case 0x76:
			printf("Not implemented!\n");
			break;
		case 0x77:
			{
				unsigned short hl = (state->h << 8) | state->l;
				writeToMemory(state, hl, state->a);
			}
			break;
		case 0x78:
			state->a = state->b;
			break;
		case 0x79:
			state->a = state->c;
			break;
		case 0x7A:
			state->a = state->d;
			break;
		case 0x7B:
			state->a = state->e;
			break;
		case 0x7C:
			state->a = state->h;
			break;
		case 0x7D:
			state->a = state->l;
			break;
		case 0x7E:
			{
				unsigned short hl = (state->h << 8) | state->l;
				state->a = state->memory[hl];
			}
			break;
		case 0x7F:
			state->a = state->a;
			break;
		case 0x80:
			add(state, state->b);
			break;
		case 0x81:
			add(state, state->c);
			break;
		case 0x82:
			add(state, state->d);
			break;
		case 0x83:
			add(state, state->e);
			break;
		case 0x84:
			add(state, state->h);
			break;
		case 0x85:
			add(state, state->l);
			break;
		case 0x86:
			{
				unsigned short hl = (state->h << 8) | state->l;
				add(state, state->memory[hl]);
			}
			break;
		case 0x87:
			add(state, state->a);
			break;
		case 0x88:
			adc(state, state->b);
			break;
		case 0x89:
			adc(state, state->c);
			break;
		case 0x8A:
			adc(state, state->d);
			break;
		case 0x8B:
			adc(state, state->e);
			break;
		case 0x8C:
			adc(state, state->h);
			break;
		case 0x8D:
			adc(state, state->l);
			break;
		case 0x8E:
			{
				unsigned short hl = (state->h << 8) | state->l;
				adc(state, state->memory[hl]);
			}
			break;
		case 0x8F:
			adc(state, state->a);
			break;
		case 0x90:	
			sub(state, state->b);
			break;
		case 0x91:
			sub(state, state->c);
			break;
		case 0x92:
			sub(state, state->d);
			break;
		case 0x93:
			sub(state, state->e);
			break;
		case 0x94:
			sub(state, state->h);
			break;
		case 0x95:
			sub(state, state->l);
			break;
		case 0x96:
			{
				unsigned short hl = (state->h << 8) | state->l;
				sub(state, state->memory[hl]);
			}
			break;
		case 0x97:
			sub(state, state->a);
			break;
		case 0x98:
			sbb(state, state->b);
			break;
		case 0x99:
			sbb(state, state->c);
			break;
		case 0x9A:
			sbb(state, state->d);
			break;
		case 0x9B:
			sbb(state, state->e);
			break;
		case 0x9C:
			sbb(state, state->h);
			break;
		case 0x9D:
			sbb(state, state->l);
			break;
		case 0x9E:
			{
				unsigned short hl = (state->h << 8) | state->l;
				sbb(state, state->memory[hl]);
			}
			break;
		case 0x9F:
			sbb(state, state->a);
			break;
		case 0xA0:
			ana(state, state->b);
			break;
		case 0xA1:
			ana(state, state->c);
			break;
		case 0xA2:
			ana(state, state->d);
			break;
		case 0xA3:
			ana(state, state->e);
			break;
		case 0xA4:
			ana(state, state->h);
			break;
		case 0xA5:
			ana(state, state->l);
			break;
		case 0xA6:
			{
				unsigned short hl = (state->h << 8) | state->l;	
				ana(state, state->memory[hl]);
			}
			break;
		case 0xA7:
			ana(state, state->a);
			break;
		case 0xA8:
			xra(state, state->b);
			break;
		case 0xA9:
			xra(state, state->c);
			break;
		case 0xAA:
			xra(state, state->d);
			break;
		case 0xAB:
			xra(state, state->e);
			break;
		case 0xAC:
			xra(state, state->h);
			break;
		case 0xAD:
			xra(state, state->l);
			break;
		case 0xAE:
			{
				unsigned short hl = (state->h << 8) | state->l;
				xra(state, state->memory[hl]);
			}
			break;
		case 0xAF:
			xra(state, state->a);
			break;
		case 0xB0:
			ora(state, state->b);
			break;
		case 0xB1:
			ora(state, state->c);
			break;
		case 0xB2:
			ora(state, state->d);
			break;
		case 0xB3:
			ora(state, state->e);
			break;
		case 0xB4:
			ora(state, state->h);
			break;
		case 0xB5:
			ora(state, state->l);
			break;
		case 0xB6:
			{
				unsigned short hl = (state->h << 8) | state->l;
				ora(state, state->memory[hl]);
			}
			break;
		case 0xB7:
			ora(state, state->a);
			break;
		case 0xB8:
			cmp(state, state->b);
			break;
		case 0xB9:
			cmp(state, state->c);
			break;
		case 0xBA:
			cmp(state, state->d);
			break;
		case 0xBB:
			cmp(state, state->e);
			break;
		case 0xBC:
			cmp(state, state->h);
			break;
		case 0xBD:
			cmp(state, state->l);
			break;
		case 0xBE:
			{
				unsigned short hl = (state->h << 8) | state->l;
				cmp(state, state->memory[hl]);
			}
			break;
		case 0xBF:
			cmp(state, state->a);
			break;
		case 0xC0:
			retConditional(state, !state->cc.z);
			break;
		case 0xC1:
			pop(state, &state->b, &state->c);
			break;
		case 0xC2:
			jmpConditional(state, !state->cc.z, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xC3:
			state->pc = (instruction[2] << 8) | instruction[1];
			break;
		case 0xC4:
			callConditional(state, !state->cc.z, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xC5:
			push(state, state->b, state->c);
			break;
		case 0xC6:
			add(state, instruction[1]);
			state->pc++;
			break;
		case 0xC7:
			state->pc += 2;
			push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
			state->pc = 0;
			break;
		case 0xC8:
			retConditional(state, state->cc.z);
			break;
		case 0xC9:
			state->pc = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
			state->sp += 2;
			break;
		case 0xCA:
			jmpConditional(state, state->cc.z, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xCB:
			break;
		case 0xCC:
			callConditional(state, state->cc.z, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xCD:
			state->pc += 2;

			push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
			state->pc = (instruction[2] << 8) | instruction[1];
			break;
		case 0xCE:
			adc(state, instruction[1]);
			state->pc++;
			break;
		case 0xCF:
			state->pc += 2;
			push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
			state->pc = 0x08;
			break;
		case 0xD0:
			retConditional(state, !state->cc.cy);
			break;
		case 0xD1:
			pop(state, &state->d, &state->e);
			break;
		case 0xD2:
			jmpConditional(state, !state->cc.cy, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xD4:
			callConditional(state, !state->cc.cy, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xD5:
			push(state, state->d, state->e);
			break;
		case 0xD6:
			sub(state, instruction[1]);
			state->pc++;
			break;
		case 0xD7:
			state->pc += 2;
			push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
			state->pc = 0x10;
			break;
		case 0xD8:
			retConditional(state, state->cc.cy);
			break;
		case 0xD9:
			break;
		case 0xDA:
			jmpConditional(state, state->cc.cy, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xDC:
			callConditional(state, state->cc.cy, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xDD:
			break;
		case 0xDE:
			sbb(state, instruction[1]);
			state->pc++;
			break;
		case 0xDF:
			state->pc += 2;
			push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
			state->pc = 0x18;
			break;
		case 0xE0:
			retConditional(state, !state->cc.p);
			break;
		case 0xE1:
			pop(state, &state->h, &state->l);
			break;
		case 0xE2:
			jmpConditional(state, !state->cc.p, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xE3:
			{
				byte tempH = state->h;
				byte tempL = state->l;

				state->l = state->memory[state->sp];
				state->h = state->memory[state->sp + 1];
				
				state->memory[state->sp] = tempL;
				state->memory[state->sp + 1] = tempH;
			}
			break;
		case 0xE4:
			callConditional(state, !state->cc.p, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xE5:
			push(state, state->h, state->l);
			break;
		case 0xE6:
			ana(state, instruction[1]);
			state->pc++;
			break;
		case 0xE7:
			state->pc += 2;
			push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
			state->pc = 0x20;
			break;
		case 0xE8:
			retConditional(state, state->cc.p);
			break;
		case 0xE9:
			state->pc = (state->h << 8) | state->l;
			break;
		case 0xEA:
			jmpConditional(state, state->cc.p, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xEB:
			{
				byte tempH = state->h;
				byte tempL = state->l;

				state->h = state->d;
				state->l = state->e;

				state->d = tempH;
				state->e = tempL;
			}
			break;
		case 0xEC:
			callConditional(state, state->cc.p, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xED:
			break;
		case 0xEE:
			xra(state, instruction[1]);
			state->pc++;
			break;
		case 0xEF:
			state->pc += 2;
			push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
			state->pc = 0x28;
			break;
		case 0xF0:
			retConditional(state, !state->cc.s);
			break;
		case 0xF1:
			pop(state, &state->a, (byte*)&state->cc);
			break;
		case 0xF2:
			jmpConditional(state, !state->cc.s, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xF3:
			state->int_enable = 0;
			break;
		case 0xF4:
			callConditional(state, !state->cc.s, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xF5:
			push(state, state->a, *(byte*)&state->cc);
			break;
		case 0xF6:
			ora(state, instruction[1]);
			state->pc++;
			break;
		case 0xF7:
			state->pc += 2;
			push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
			state->pc = 0x30;
			break;
		case 0xF8:
			retConditional(state, state->cc.s);
			break;
		case 0xF9:
			state->sp = (state->h << 8) | state->l;
			break;
		case 0xFA:
			jmpConditional(state, state->cc.s, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xFB:
			state->int_enable = 1;
			break;
		case 0xFC:
			callConditional(state, state->cc.s, (instruction[2] << 8) | instruction[1]);
			break;
		case 0xFD:
			break;
		case 0xFE:
			cmp(state, instruction[1]);
			state->pc++;
			break;
		case 0xFF:
			state->pc += 2;
			push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
			state->pc = 0x38;
			break;
	}

	return 0;
}

/*
	This function will emulate the ADD instruction
	Input: A pointer to the struct that represents the current state of the CPU, a byte that holds the value of the register that we want to add to the acumlator
*/
void add(State8080* state, byte r)
{
	state->cc.ac = (((state->a & 0x0F) + (r & 0x0F)) & 0x10) == 0x10;
	unsigned short res = state->a + r;

	state->cc.z = (0 == (res & 0x00FF));
	state->cc.s = (0x80 == (res & 0x80));
	state->cc.p = pairtyCheck(res, 8);
	state->cc.cy = ((res & 0xFF00) != 0);

	state->a = (res & 0x00FF);
}

/*
	This function will emulate the ADC instruction (Add with carry)
	Input: A pointer to the struct that represents the current state of the CPU, a byte that holds the value of the register that we want to add plus the carry
*/
void adc(State8080* state, byte r)
{
	state->cc.ac = (((state->a & 0x0F) + (r & 0x0F) + state->cc.cy) & 0x10) == 0x10;
	unsigned short res = state->a + r + state->cc.cy;

	state->cc.z = (0 == (res & 0x00FF));
	state->cc.s = (0x80 == (res & 0x80));
	state->cc.p = pairtyCheck(res, 8);
	state->cc.cy = ((res & 0xFF00) != 0);

	state->a = (res & 0x00FF);
}

/*
	This function will emulate the SUB instruction
	Input: A pointer to the struct that represents the current state of the CPU, a byte that holds the value of the reigster that we want to sub
*/
void sub(State8080* state, byte r)
{
	state->cc.ac = ((state->a & 0x0F) - (r & 0x0F)) < 0;
	unsigned short res = state->a - r;

	state->cc.z = (0 == (res & 0x00FF));
	state->cc.s = (0x80 == (res & 0x80));
	state->cc.p = pairtyCheck(res, 8);
	state->cc.cy = ((res & 0xFF00) != 0);

	state->a = (res & 0x00FF);
}

/*
	This function will emulate the SBB insturction (sub register with carry)
	Input: A pointer to a struct that represents the current state of the CPU, a byte that holds the value of the register that we want to sub
*/
void sbb(State8080* state, byte r)
{
	state->cc.ac = ((state->a & 0x0F) - (r & 0x0F) - state->cc.cy) < 0;
	unsigned short res = state->a - r - state->cc.cy;

	state->cc.z = (0 == (res & 0x00FF));
	state->cc.s = (0x80 == (res & 0x80));
	state->cc.p = pairtyCheck(res, 8);
	state->cc.cy = ((res & 0xFF00) != 0);

	state->a = (res & 0x0FF);
}

/*
	This function will emulate the ANA instruction (And operator on A register with r register)
	Input: A pointer to a struct that represents the current state of the CPU, a byte that holds the value of the register that we want to AND with
*/
void ana(State8080* state, byte r)
{
	//state->cc.ac ???
	byte res = state->a & r;

	state->cc.z = (0 == res);
	state->cc.s = (0x80 == (res & 0x80));
	state->cc.p = pairtyCheck(res, 8);
	state->cc.cy = 0; //There will never be any carry from the AND instruction

	state->a = res;
}

/*
	This function will emulate the XRA instruction (XOR operator on A register with r register
	Input: A pointer to a struct that represents the current state of the CPU, a byte that holds teh value of the register that we want to XOR with
*/
void xra(State8080* state, byte r)
{
	//state->cc.ac ???
	byte res = state->a ^ r;

	state->cc.z = (0 == res);
	state->cc.s = (0x80 == (res & 0x80));
	state->cc.p = pairtyCheck(res, 8);
	state->cc.cy = 0; //There will never be any carry from the XOR instrucion

	state->a = res;
}

/*
	This function will emulate the ORA instruction (OR operator on A with the R register)
	Input: A pointer to the struct that represents the current state of the CPU, A byte that holds the register value
*/
void ora(State8080* state, byte r)
{
	//state->cc.ac ???
	byte res = state->a | r;

	state->cc.z = (0 == res);
	state->cc.s = (0x80 == (res & 0x80));
	state->cc.p = pairtyCheck(res, 8);
	state->cc.cy = 0; //There will never be any carry from the OR instruction

	state->a = res;
}

/*
	This function will emulate the CMP instruction. It will subtract r from A and will turn on the flags by the result.
	Input: A pointer to a struct that represents the current state of the CPU, A byte that holds the register value
*/
void cmp(State8080* state, byte r)
{
	state->cc.ac = ((state->a & 0x0F) - (r & 0x0F)) < 0;
	unsigned short res = state->a - r;

	state->cc.z = (0 == (res & 0x00FF));
	state->cc.s = (0x80 == (res & 0x80));
	state->cc.p = pairtyCheck(res, 8);
	state->cc.cy = ((res & 0xFF00) != 0);
}

void push(State8080* state, byte high, byte low)
{
	writeToMemory(state, state->sp - 1, high);
	writeToMemory(state, state->sp - 2, low);
	state->sp -= 2;
}

void pop(State8080* state, byte* high, byte* low)
{
	*low = state->memory[state->sp];
	*high = state->memory[state->sp + 1];

	state->sp += 2;
}

void retConditional(State8080* state, byte Conditional)
{
	if (Conditional)
	{
		state->pc = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
		state->sp += 2;
		state->cycles += 6;
	}
}

void callConditional(State8080* state, byte Conditional, unsigned short adr)
{
	state->pc += 2;

	if (Conditional)
	{
		push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
		state->pc = adr;
		state->cycles += 6;
	}
}

void jmpConditional(State8080* state, byte Conditional, unsigned short adr)
{
	if (Conditional)
	{
		state->pc = adr;
	}
	else
	{
		state->pc += 2;
	}
}

void writeToMemory(State8080* state, unsigned short adr, byte value)
{
	if (adr < 0x2000)
	{
		printf("Writing ROM not allowed %x\n", adr);
		return;
	}
	
	if (adr >= 0x4000 && adr < 0x6000) 
	{ 
		adr -= 0x2000;
	}
	
	state->memory[adr] = value;
}

void generateInterrupt(State8080* state, byte interrupt_num)
{
	if (!state->int_enable)
	{
		return;
	}

	push(state, (state->pc & 0xFF00) >> 8, state->pc & 0x00FF);
	state->cycles += 11;
	state->pc = 8 * interrupt_num;
	state->int_enable = 0;
}

