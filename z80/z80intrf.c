#include	"../neocd.h"
#include 	<stdio.h>
#include 	<stdlib.h>
#include	"z80intrf.h"
#include	"cz80.h"
#include	"../sound/2610intf.h"


cz80_struc Cz80_struc;

static __inline__ unsigned char cpu_readmem8(unsigned int address)
{
	return (subcpu_memspace[address&0xFFFF]);
}

#if CZ80_USE_WORD_HANDLER
static unsigned short cpu_readmem16(unsigned int address)
{
	return cpu_readmem8(address) | (cpu_readmem8(address + 1) << 8);
}
#endif

static __inline__ void cpu_writemem8(unsigned int address, unsigned int data)
{
       
	subcpu_memspace[address&0xFFFF]=data;
}

#if CZ80_USE_WORD_HANDLER
static void cpu_writemem16(unsigned int address, unsigned int data)
{
	cpu_writemem8(address, data & 0xFF);
	cpu_writemem8(address + 1, data >> 8);
}
#endif




u8 subcpu_memspace[0x10000]__attribute__((aligned(32)));

u32 sound_code;
u32 result_code;
u32 pending_command;
int		Z80_VBL_CYCLES;
int		z80_cycles;
u16 z80_bank[4];

static u8 *z80map1, *z80map2, *z80map3, *z80map4;

int cpu_z80_irq_callback(int a)
{
	return 0;
}



//---------------------------------------------------------------------------

void cz80_z80_writeport16(u16 PortNo, u8 data)

{

	switch( PortNo & 0xff)
	{
	case	0x4:
		YM2610_control_port_0_A_w(0,data);
		break;

	case	0x5:
		YM2610_data_port_0_A_w(0,data);
		break;

	case	0x6:
		YM2610_control_port_0_B_w(0,data);
		break;

	case	0x7:
		YM2610_data_port_0_B_w(0,data);
		break;


	case	0x8:
		// NMI enable / acknowledge? (the data written doesn't matter)	
		// Metal Slug Passes this 35, then 0 in sequence. After a mission begins it passes it 1.
		break;
	
	case	0xc:
		result_code = data;
		break;
	
/*
	case	0x18:
		// NMI disable? (the data written doesn't matter) 
		break;
*/
/*
	case	0xc0:
		//cyberlip calls this for some reason, maybe other games?
		break;
*/
/*
	case	0xc1:
		//cyberlip calls this for some reason, maybe other games?
		break;
*/
/*		case	0x0:
		//This is called by Aero Fighters 2 at startup as well as a few others. 
		//They first disable the NMI, call this and pass 80, then enable the NMI again
		break;
*/
	default:
		//printf("Unimplemented Z80 Write Port: %x data: %x\n",PortNo&0xff,data);
		break;
	}
}

//---------------------------------------------------------------------------

u8 cz80_z80_readport16(u16 PortNo)

{
	static int bank[4];
	
	switch( PortNo & 0xff)
	{
	case	0x0:
		pending_command = 0;
		return sound_code;
		break;
	
	case	0x4:
		return YM2610_status_port_0_A_r(0);
		break;
	
	case	0x5:
		return YM2610_read_port_0_r(0);
		break;
	
	case	0x6:
		return YM2610_status_port_0_B_r(0);
		break;
///*
	case 0x08:
		{
		    bank[3] = 0x0800 * ((PortNo >> 8) & 0x7f);
			return 0;
			break;
		}
	case 0x09:
		{
			bank[2] = 0x1000 * ((PortNo >> 8) & 0x3f);
			return 0;
			break;
		}	
	case 0x0a:
		{
			bank[1] = 0x2000 * ((PortNo >> 8) & 0x1f);
			return 0;
			break;
		}
	case 0x0b:
		{
			bank[0] = 0x4000 * ((PortNo >> 8) & 0x0f);
			return 0;
			break;
		}
//*/
	default:
		//printf("Unimplemented Z80 Read Port: %x\n",PortNo&0xff);
		break;
	};	
	return 0;
}


void z80_init(void)
{
		Z80_VBL_CYCLES = 4000000/FPS;
		z80_cycles=Z80_VBL_CYCLES;
	Cz80_Init(&Cz80_struc);
	Cz80_Set_Fetch(&Cz80_struc,0x0000,0xFFFF,(u32)((void *)&subcpu_memspace));
	Cz80_Set_ReadB(&Cz80_struc,(unsigned int (*)(unsigned int))&cpu_readmem8);
	Cz80_Set_WriteB(&Cz80_struc,&cpu_writemem8);
#if CZ80_USE_WORD_HANDLER
	Cz80_Set_ReadW(&Cz80_struc,&cpu_readmem16);
	Cz80_Set_WriteW(&Cz80_struc,&cpu_writemem16);
#endif
	Cz80_Set_INPort(&Cz80_struc,(CZ80_READ *)&cz80_z80_readport16);
	Cz80_Set_OUTPort(&Cz80_struc,(CZ80_WRITE *)&cz80_z80_writeport16);
	Cz80_Set_IRQ_Callback(&Cz80_struc,cpu_z80_irq_callback);
	Cz80_Reset(&Cz80_struc);
	Cz80_Exec(&Cz80_struc,100000);


}
