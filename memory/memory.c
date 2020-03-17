/********************************************
*   NeoCD Memory Mapping (C version)        *
*********************************************
* Fosters(2001,2003)                        *
********************************************/
/*
######################################################################
#                 Dream neo cd for DreamCast  V 1.0                  #
######################################################################
# including :	Dreamneo cd v1.0 Beta                                #
#  Ported by Ian Micheal Based on   Neogeo cd SDL by fosters         #
#											                         #
#			2004 team and credits									 #
#	#code and co coders quzar black aura troy GPF blue crab		     #
######################################################################
#																	 #
#								   Makefile (c)2020 from Ian Micheal #
######################################################################
*/
//-- Include Files ----------------------------------------------------------- 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <kos.h>
#include "../neocd.h"
#include <SDL/SDL.h>
extern unsigned short    *video_paletteram_ng __attribute__((aligned(32))); 
// vector table
char 		neogeo_game_vectors[0x80]  __attribute__((aligned(32))); 
/*** Globals ***************************************************************************************/
int watchdog_counter=-1;
//int watchdog_counter = 9;
int memcard_write=0;

static int irq2start=1000,irq2control;
static u32 irq2pos_value;


void   initialize_memmap(void);
unsigned int m68k_read_memory_8(unsigned int address);
unsigned int m68k_read_memory_16(unsigned int address);
unsigned int m68k_read_memory_32(unsigned int address);
void m68k_write_memory_8(unsigned int address, unsigned int value);
void m68k_write_memory_16(unsigned int address, unsigned int value);
void m68k_write_memory_32(unsigned int address, unsigned int value);
void write16_200000_2fffff(int offset,int data);
int read16_200000_2fffff(int offset);

/***Helper Functions********************************************************************************/
static int    read_upload(int);
static void   write_upload(int, int);
static void   write_upload_word(int, int);
static void   write_upload_dword(int, int);

static int    cpu_readvidreg(int);
static void   cpu_writevidreg(int, int);
static void   cpu_writeswitch(int, int);
static int    cpu_readcoin(int);

static void   write_memcard(int,int);

void cdda_control(void);

static void   watchdog_reset_w(void);
extern int sound;


/***************************************************************************************************/
void initialize_memmap(void) {    return;}

/***************************************************************************************************/
unsigned int m68k_read_memory_8(const unsigned int off) {
    unsigned int offset = off & 0xffffff;
    if(offset<0x200000)
       return neogeo_prg_memory[offset^1];

    switch(offset>>16)
    {
        case    0x30:    return read_player1();
        case    0x32:    return cpu_readcoin(offset);
        case    0x34:    return read_player2();
        case    0x38:    return read_pl12_startsel();
        case    0x80:    if(offset&0x01) return neogeo_memorycard[(offset&0x3fff)>>1];
                         else return -1;

        /* BIOS ROM */
        case    0xc0:
        case    0xc1:
        case    0xc2:
        case    0xc3:
        case    0xc4:
        case    0xc5:
        case    0xc6:
        case    0xc7:     return neogeo_rom_memory[(offset^1)&0x0fffff];

        /* upload region */
        case    0xe0:
        case    0xe1:
        case    0xe2:
        case    0xe3:    return read_upload(offset&0xfffff);

        default:
            //   m68k_get_reg(NULL,M68K_REG_PC));
                #ifdef DBUGOUT
          printf("m68k_read_memory_8(0x%x) PC=%x\n",offset,0);//m68k_get_reg(NULL,M68K_REG_PC));
          #endif
            break;
    }
    return 0;
}



/***************************************************************************************************/
unsigned int m68k_read_memory_16(const unsigned int off) {
    unsigned int offset = off & 0xffffff;
    unsigned int data;
    if(offset<0x200000)
        return *((u16*)&neogeo_prg_memory[offset]);

    switch(offset>>16)
    {
		case	0x20:	case	0x21:	case	0x22:	case	0x23:	case	0x24:	case	0x25:	case	0x26:
		case	0x27:	case	0x28:pd4990a_control_16_w(0, data);  break;	case	0x29:	case	0x2a:	case	0x2b:	case	0x2c:	case	0x2d:
		case	0x2e:	case	0x2f:	return read16_200000_2fffff(offset); break;/**/

	//	case	0x2f:	//This, in kof99, kof00 and garou is a random number generator which mame calls 'sma_random_r' I will duplicate it for future implementation.
		case	0x32:
		case	0x33:		//I think this should be same as in the read8 set
			break;
        case    0x3c:    
		case	0x3d:	return cpu_readvidreg(offset);
        case    0x40:    return video_paletteram_ng[offset&0x1fff];
		case	0x6a:	//OUTPUT("interesting...this is a write to static ram, or so says MAME");
        case    0x80:    return 0xff00|neogeo_memorycard[(offset&0x3fff)>>1];

        /* BIOS ROM */
        case    0xc0:
        case    0xc1:
        case    0xc2:
        case    0xc3:
        case    0xc4:
        case    0xc5:
        case    0xc6:
        case    0xc7:    return *(u16*)&neogeo_rom_memory[offset&0x0fffff];
		
		case	0xff:	 break;


        default:
                #ifdef DBUGOUT
          printf("m68k_read_memory_16(0x%x) PC=%x\n",offset,0);//m68k_get_reg(NULL,M68K_REG_PPC));
          #endif
            break;
    }
    return 0;
}
/***************************************************************************************************/
unsigned int m68k_read_memory_32(const unsigned int off) {
    unsigned int offset = off & 0xffffff;
    unsigned int data;
    data=m68k_read_memory_16(offset)<<16;
    data|=m68k_read_memory_16(offset+2);
    return data;
}


/***************************************************************************************************/
void m68k_write_memory_8(const unsigned int off, unsigned int data) {
    int temp;
    unsigned int offset = off & 0xffffff;
    data&=0xff;

    if(offset<0x200000) {
        neogeo_prg_memory[offset^1]=(char)data;
        return;
    }

    switch(offset>>16)
    {
        case    0x30:    watchdog_reset_w(); break;
        case    0x32:
		case	0x33://ok yea, im not sure if thats true, but thats what mame 92 says about the mvs ?
			if(sound){
    if(!(offset&0xffff)) {
                sound_code=data&0xff;
                pending_command=1;
                mz80nmi();
                //thd_sleep(20);
                	asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
                z80_cycles-=Z80_VBL_CYCLES/nb_interlace;
                mz80exec(Z80_VBL_CYCLES/nb_interlace);
             
                my_timer();
            }
			}
	    break;
		case	0x38:		//this range is unknown/not use EXCEPT for 0x380050 which is the pd4990a pd4990a_control_16_w
								//turns out in AeroFighters 2, this is called ad nauseum as 0x3800001 with data 0,12,1b in cycle
								if(!(offset&0xffaf))
				pd4990a_control_16_w(0,data); break;
								//Quzar - actually trying to improve this biatch!
						
        case    0x3a:    cpu_writeswitch(offset, data); break;
        case    0x3c:   // printf("MEMEWRITE8: 0x3c"); // according to old
		case	0x3d:	 temp=cpu_readvidreg(offset);
						if(offset&0x01) cpu_writevidreg(offset, (temp&0xff)|(data<<8));
						else cpu_writevidreg(offset, (temp&0xff00)|data);
            break;
        case    0x80:    if(offset&0x01) write_memcard(offset,data); break;


        /* upload */
        case    0xe0:
        case    0xe1:
        case    0xe2:
        case    0xe3:   write_upload(offset&0xfffff,data); break;
		
		/* cdrom */
		case	0xff:	break;

        default:
                #ifdef DBUGOUT
           printf("m68k_write_memory_8(0x%x,0x%x) PC=%x\n",offset,data,0);//m68k_get_reg(NULL,M68K_REG_PC));
           #endif
            break;
    }
}



/***************************************************************************************************/
void m68k_write_memory_16(const unsigned int off, unsigned int data) {
     
    unsigned int offset = off & 0xffffff;
    data&=0xffff;

    if(offset<0x200000) {
        *(u16*)&neogeo_prg_memory[offset]=(u16)data;
        return;
    }

    switch(offset>>16)
    {
		case	0x20:	case	0x21:	case	0x22:	case	0x23:	case	0x24:	case	0x25:	case	0x26:
		case	0x27:	case	0x28:	case	0x29:	case	0x2a:	case	0x2b:	case	0x2c:	case	0x2d:
		case	0x2e:	case	0x2f:	write16_200000_2fffff(offset, data); break;/**/
        case    0x3a:    cpu_writeswitch(offset, data); break;	//on the neo geo arcade this selects the FIX char rom from the board. 0x3a000a - 0x3a000b
        case    0x3c:
		case	0x3d:	cpu_writevidreg(offset, data); break;
        case    0x40:    offset =(offset&0x1fff)>>1;
                         data  &=0x7fff;
                         video_paletteram_ng[offset]=(u16)data;
                         video_paletteram_pc[offset]=video_color_lut[data]; break;
		case	0x60:	//this should be color ram bank0, the one above is color bank1 so maybe thatll help?
        case    0x80:    write_memcard(offset,data); break;

        /* upload */
        case    0xe0:
        case    0xe1:
        case    0xe2:
        case    0xe3:    write_upload_word(offset&0xfffff,data); break;
		
		case	0xec:	//cyberlip grabs this, so does Sam Sho4
		case	0xef:	//This is called in Super Sidekicks 3 0xef3e00 - 0xef49fe
		case	0xfe:	break;	//this might only be called by accident, but who knows.
		case	0xff: break;

        default:
                #ifdef DBUGOUT
            printf("m68k_write_memory_16(0x%x,0x%x) PC=%x\n",offset,data, 0);//data,m68k_get_reg(NULL,M68K_REG_PC));
            #endif
            break;
    }
}



/***************************************************************************************************/
void m68k_write_memory_32(const unsigned int off, unsigned int data) {
      unsigned int offset = off & 0xffffff;
	unsigned int word1=(data>>16)&0xffff;
	unsigned int word2=data&0xffff;
	m68k_write_memory_16(offset,word1);
	m68k_write_memory_16(offset+2,word2);		
}
/***************************************************************************************************/
static int    cpu_readvidreg(int offset)

{
    switch(offset)
    {
        case    0x3c0000:    return *(u16*)&video_vidram[video_pointer<<1]; break;
        case    0x3c0002:    return *(u16*)&video_vidram[video_pointer<<1]; break;
        case    0x3c0004:    return video_modulo; break;
       case    0x3c0006:    return ((((neogeo_frame_counter_speed-1)&0xffff)<<8)|(neogeo_frame_counter&7)|128); break;
	  //  case    0x3c0006:    return  neogeo_frame_counter_speed = (((data >> 8) & 0xff)+1);break; ///  return ((((neogeo_frame_counter_speed-1)&0xffff)<<8)|(neogeo_frame_counter&7)); break;
        case    0x3c0008:    return *(u16*)&video_vidram[video_pointer<<1]; break;
        case    0x3c000a:    return *(u16*)&video_vidram[video_pointer<<1]; break;
/*
			     break;
        case    0x3c0008:    break; // IRQ position
        case    0x3c000a:    break; // IRQ position
        case    0x3c000c:    break; // IRQ acknowledge
*/
        default:
              //  m68k_get_reg(NULL,M68K_REG_PC));
                #ifdef DBUGOUT
         printf("cpu_readvidreg(0x%x) PC=%x\n",offset,0);//m68k_get_reg(NULL,M68K_REG_PC));
         #endif
            return 0;
            break;
    }
}
/***************************************************************************************************/
static void    cpu_writevidreg(int offset, int data)
{
    switch(offset)
    {
        case    0x3c0000:    video_pointer=(u16)data; break;
        case    0x3c0002:    *(u16*)&video_vidram[video_pointer<<1]=(u16)data;
                             video_pointer+=video_modulo; break;
        case    0x3c0004:    video_modulo=(s16)data; break;

        //case    0x3c0006:    neogeo_frame_counter_speed=((data>>8)&0xff)+1; break;
		case    0x3c0006:    neogeo_frame_counter_speed=((data>>8)&0xff); break;//MAME says...
			

        case    0x3c0008:    /* IRQ position 0 call irq2pos*/    break;
        case    0x3c000a:    /* IRQ position 1 call irq2pos*/    break;
        case    0x3c000c:    /* IRQ acknowledge */ break;

        default:
              //  m68k_get_reg(NULL,M68K_REG_PC));
                #ifdef DBUGOUT
        printf("cpu_writevidreg(0x%x,0x%x) PC=%x\n",offset,data,0);//m68k_get_reg(NULL,M68K_REG_PC));
        #endif
            break;
    }
}

/***************************************************************************************************/

static void     neogeo_setpalbank0 (void) {
    video_paletteram_ng=video_palette_bank0_ng;
    video_paletteram_pc=video_palette_bank0_pc;
}


static void     neogeo_setpalbank1 (void) {
    video_paletteram_ng=video_palette_bank1_ng;
    video_paletteram_pc=video_palette_bank1_pc;
}


static void    neogeo_select_bios_vectors (void) {
    memcpy(neogeo_prg_memory, neogeo_rom_memory, 0x80);
}



static void neogeo_select_game_vectors (void) 
{
    memcpy( neogeo_prg_memory, neogeo_game_vectors, 0x80 );
    printf("write game vectors stub\n");
}
static void    cpu_writeswitch(int offset, int data)
{
    switch(offset)
    {
        case 0x3a0000: /* NOP */ break;
        case 0x3a0001: /* NOP */ break;

        case 0x3a0002: neogeo_select_bios_vectors(); break;
        case 0x3a0003: neogeo_select_bios_vectors(); break;

        case 0x3a000e: neogeo_setpalbank1(); break;
        case 0x3a000f: neogeo_setpalbank1(); break;

        case 0x3a0010: /* NOP */ break;
        case 0x3a0011: /* NOP */ break;

        case 0x3a0012: neogeo_select_game_vectors(); break;
        case 0x3a0013: neogeo_select_game_vectors(); break;
		case 0x3a0019:	//happens in aero fighters 2 break;

        case 0x3a001e: neogeo_setpalbank0(); break;
        case 0x3a001f: neogeo_setpalbank0(); break;

        default:
             //   m68k_get_reg(NULL,M68K_REG_PC));
                #ifdef DBUGOUT
       printf("cpu_writeswitch(0x%x,0x%x) PC=%x\n",offset,data,0);//m68k_get_reg(NULL,M68K_REG_PC));
        #endif
            break;
    }
}

/***************************************************************************************************/


void neogeo_sound_irq(int irq)
{
    if (irq) {
             #ifdef DBUGOUT
        printf("neogeo_sound_irq %d\n",irq);
        #endif
        mz80int(0);
    } else {
       mz80ClearPendingInterrupt();
       #ifdef DBUGOUT
       printf("neogeo_sound_irq_end %d\n",irq);
       #endif
    }
}



static int original_cpu_readcoin(int addr)
{

    addr &= 0xFFFF;
	//printf("cpu_readcoin: addr:%x\n",addr);
    if (addr == 0x1) {
        int coinflip = pd4990a_testbit_r();
        int databit = pd4990a_databit_r();
        return 0xff ^ (coinflip << 6) ^ (databit << 7);
    }
    if (addr == 0x0) {
        int res = 0;
        res |= result_code;
        if (!pending_command) {
            res &= 0x7f;
        } else {
            res |= 0x01;
        }
        return res;
    }
    return 0;
}

static int cpu_readcoin(int addr){
	int res;
        int coinflip = pd4990a_testbit_r();
        int databit = pd4990a_databit_r();


	res = 0xff ^ (coinflip << 6) ^ (databit << 7);

	
		res |= result_code;
		if (pending_command) res &= 0x7f;

	else
		res |= 0x01;

	return res;
}

static void watchdog_reset_w (void)
{
    if (watchdog_counter == -1) printf("Watchdog Armed!\n");
    watchdog_counter=9 * 60;  /* 3s * 60 fps */
}


static int read_upload(int offset) {
      
       printf("read_upload(0x%X)\n",offset); fflush(stdout);
       
    int zone = m68k_read_memory_8(0x10FEDA);
    int bank = m68k_read_memory_8(0x10FEDB);

    /* read_upload is disabled for now.*/
	/* fixes Metal Slug */

printf("read_upload in zone %x offset: %x\n",zone,offset);

   
	return -1;

    switch (zone) {
        case 0x00: /* 68000 */
            return neogeo_prg_memory[offset^1];	//it seems safe to keep this enabled, but without more testing, i wont take the risk it breaks things
		case 0x01: /* Z80 */
		//	 return subcpu_memspace[offset>>1];
			 return subcpu_memspace[(offset>>1)&0xFFFF];
			return -1;
        default:
                #ifdef DBUGOUT
          printf("read_upload unimplemented zone %x offset: %x\n",zone,offset);
          #endif
           return -1; 
    }
	
}


static void write_upload(int offset, int data) {
       #ifdef DBUGOUT
       printf("write_upload(0x%X,0x%X)\n",offset,data); fflush(stdout);
       #endif
    int zone = m68k_read_memory_8(0x10FEDA);
   int bank = m68k_read_memory_8(0x10FEDB); 

    switch (zone) {
        case 0x00: /* 68000 */
            neogeo_prg_memory[offset^1]=(char)data;
            break;
        case 0x01: /* Z80 */
          //  subcpu_memspace[offset>>1]=(char)data;
            subcpu_memspace[(offset>>1)&0xFFFF]=(char)data;
			//OUTPUT("write_upload in zone %x offset: %x data: %x\n",zone,offset>>1,data);
            break;
        case 0x11: /* FIX */
            neogeo_fix_memory[offset>>1]=(char)data;
            break;
        default:
                #ifdef DBUGOUT
         printf("write_upload unimplemented zone %x\n",zone); 
         #endif
			break;
    }
}


 void write_upload_word(int offset, int data) {
       #ifdef DBUGOUT
       printf("write_upload_word(0x%X,0x%X)\n",offset,data); fflush(stdout);
       #endif
    int zone = m68k_read_memory_8(0x10FEDA);
    int bank = m68k_read_memory_8(0x10FEDB);
    int offset2;
    char *dest;
    char sprbuffer[4];
	
  	data&=0xffff;

    switch (zone) {
        case 0x12: /* SPR */

            offset2=offset & ~0x02;

            offset2+=(bank<<20);

            if((offset2&0x7f)<64)
               offset2=(offset2&0xfff80)+((offset2&0x7f)<<1)+4;
            else
               offset2=(offset2&0xfff80)+((offset2&0x7f)<<1)-128;

            dest=&neogeo_spr_memory[offset2];

            if (offset & 0x02) {
               /* second word */
			   *(u16*)(&dest[2])=(u16)data;
			   /* reformat sprite data */
               swab(dest, sprbuffer, sizeof(sprbuffer));
               extract8(sprbuffer, dest);
			} else {
			   /* first word */
			   *(u16*)(&dest[0])=(u16)data;
            }
            break;

        case 0x13: /* Z80 */
            subcpu_memspace[offset>>1]=(char)data;
            break;
        case 0x14: /* PCM */
            neogeo_pcm_memory[(offset>>1)+(bank<<19)]=(char)data;
            break;
        default:
                #ifdef DBUGOUT
         printf("write_upload_word unimplemented zone %x\n",zone); 
         #endif
			break;
    }
}


static void write_upload_dword(int offset, int data) {
       #ifdef DBUGOUT
       printf("write_upload_dword(0x%X,0x%X)\n",offset,data); fflush(stdout);
       #endif
    int zone = m68k_read_memory_8(0x10FEDA);
    int bank = m68k_read_memory_8(0x10FEDB);
    char *dest;
    char sprbuffer[4];

    switch (zone) {
        case 0x12: /* SPR */
            offset+=(bank<<20);

            if((offset&0x7f)<64)
               offset=(offset&0xfff80)+((offset&0x7f)<<1)+4;
            else
               offset=(offset&0xfff80)+((offset&0x7f)<<1)-128;

            dest=&neogeo_spr_memory[offset];
            swab((char*)&data, sprbuffer, sizeof(sprbuffer));
            extract8(sprbuffer, dest);

            break;

        default:
                #ifdef DBUGOUT
          printf("write_upload_dword unimplemented zone %x\n",zone);
          #endif 
          break;

    }
}


static void write_memcard(int offset, int data) {
    data&=0xff;
    neogeo_memorycard[(offset&0x3fff)>>1]=(char)data; 

    /* signal that memory card has been written */
    memcard_write=3; 
}

int neogeo_rng = 0x2345;
static int sma_random_r(void){	//returns the last value of neogeo_rng, and modifies the current one.

int old = neogeo_rng;

	int newbit = (
			(neogeo_rng >> 2) ^
			(neogeo_rng >> 3) ^
			(neogeo_rng >> 5) ^
			(neogeo_rng >> 6) ^
			(neogeo_rng >> 7) ^
			(neogeo_rng >>11) ^
			(neogeo_rng >>12) ^
			(neogeo_rng >>15)) & 1;

	neogeo_rng = ((neogeo_rng << 1) | newbit) & 0xffff;

	return old;
}


static int prot_data; //used for the protection thingers
void write16_200000_2fffff(int offset,int data){
	//supposedly for fatal fury 2
	switch (offset)
	{
	case 0x255552 :	 /* data == 0x5555; read back from 55550, ffff0, 00000, ff000 */
		prot_data = 0xff00ff00;
		break;

	case 0x256782 :	 /* data == 0x1234; read back from 36000 *or* 36004 */
		prot_data = 0xf05a3601;
		break;

	case 0x242812 :	 /* data == 0x1824; read back from 36008 *or* 3600c */
		prot_data = 0x81422418;
		break;

	case 0x255550 :
	case 0x2ffff0 :
	case 0x2ff000 :
	case 0x236000 :
	case 0x236004 :
	case 0x236008 :
	case 0x23600c :
		prot_data <<= 8;
		break;

	default:
//printf("unknown protection write at pc %06x, offset %08x, data %02x\n",offset,data);
		break;
	}
}

int read16_200000_2fffff(int offset)
{
	u16 res = (prot_data >> 24) & 0xff;

	switch (offset)
	{
	case 0x55550 >> 1:
	case 0xffff0 >> 1:
	case 0x00000 >> 1:
	case 0xff000 >> 1:
	case 0x36000 >> 1:
	case 0x36008 >> 1:
		return res;

	case 0x36004 >> 1:
	case 0x3600c >> 1:
		return ((res & 0xf0) >> 4) | ((res & 0x0f) << 4);

	default:
//logerror("unknown protection read at pc %06x, offset %08x\n",cpu_get_pc(),offset<<1);
		return 0;
	}
}

