
/*
######################################################################
#                 Dream neo cd for DreamCast  V 1.0                  #
######################################################################
# including :	Dreamneo cd v1.0 Beta                                #
#  Ported by Ian Micheal Based on   Neogeo cd SDL by fosters         #
#			2003 Fosters Based on  NeoCD/SDL					     #
#			2004 team and credits									 #
#	#code and co coders quzar black aura troy GPF blue crab		     #
######################################################################
#																	 #
#								   Makefile (c)2020 from Ian Micheal #
######################################################################
*/

#ifndef NEOCD_H
#define NEOCD_H
//#define BUFFER_LEN 131072 
#define BUFFER_LEN 16384
//#define BUFFER_LEN 8192
#define SAMPLE_RATE    11025
#define MIXER_MAX_CHANNELS 8
//#define SAMPLE_RATE    8000
#define REFRESHTIME (1000/60)+1
#include <kos.h>
//#define OUTPUT(...) 
#include "types.h"
#include "cdaudio/cdaudio.h"
#include "mem/memfuncs.h"
#include "cdrom/cdrom.h"
#include "memory/memory.h"
#include "video/video.h"
#include "input/input.h"
#include "z80/z80intrf.h"
#include "sound/sound.h"
#include "sound/streams.h"
#include "sound/2610intf.h"
#include "sound/timer.h"
#include "pd4990a.h"



#include "c68k/c68k.h"


/*-- Version, date & time to display on startup ----------------------------*/
#define VERSION1 "DREAM NEO CD is based on NeoCD/SDL 0.3.1 and GnGeo 0.6.5 And Neoraine"
#define VERSION2 "Compiled on: "__DATE__" "__TIME__
#define PORTBY "DREAM NEO CD Is port by Ian micheal"


/*-- globals ---------------------------------------------------------------*/
extern char	*neogeo_rom_memory __attribute__((aligned(32)));
extern char	*neogeo_prg_memory __attribute__((aligned(32)));
extern char	*neogeo_fix_memory __attribute__((aligned(32)));
extern char	*neogeo_spr_memory __attribute__((aligned(32)));
extern char	*neogeo_pcm_memory __attribute__((aligned(32)));

extern unsigned char neogeo_memorycard[8192]__attribute__((aligned(32)));
extern char 	neogeo_game_vectors[128]  __attribute__((aligned(32)));
//extern char	*OP_ROM;

extern int      neogeo_ipl_done;
extern int FPS;
extern u32 neocd_time;

extern void swab (const void* src1, const void* src2, int isize);
extern u8 *R24[0x100],		// Memory Addresses for UAE020 Read
*W24[0x100],		// Memory Addresses for UAE020 Read
*ROM, 		// Pointer for ROM memory
*RAM, 		// Pointer for RAM memory
*GFX, 		// Pointer for Graphics memory
*SMP, 		// Pointer for Sample memory
*EEPROM,			// Pointer for EEPROM memory
*Z80ROM;			// Pointer for Z80ROM memory
#endif /* NEOCD_H */



