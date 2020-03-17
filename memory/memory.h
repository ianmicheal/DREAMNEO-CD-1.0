/**** MEMORY DEFINITIONS for NEOCD emulator ****/
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
#ifndef	MEMORY_H
#define MEMORY_H

/*-- Exported Functions ------------------------------------------------------*/

void	initialize_memmap(void);

void	cpu_setOPbase(int);

unsigned int m68k_read_memory_8(unsigned int address);
unsigned int m68k_read_memory_16(unsigned int address);
unsigned int m68k_read_memory_32(unsigned int address);
void m68k_write_memory_8(unsigned int address, unsigned int value);
void m68k_write_memory_16(unsigned int address, unsigned int value);
void m68k_write_memory_32(unsigned int address, unsigned int value);

unsigned int FASTCALL m68k_read_memory_8f(const unsigned int address);
unsigned int FASTCALL m68k_read_memory_16f(const unsigned int address);
unsigned int FASTCALL m68k_read_memory_32f(const unsigned int address);
void FASTCALL m68k_write_memory_8f(const unsigned int address, unsigned int value);
void FASTCALL m68k_write_memory_16f(const unsigned int address, unsigned int value);
void FASTCALL m68k_write_memory_32f(const unsigned int address, unsigned int value);

void    neogeo_sound_irq(int irq);
extern int nb_interlace;
extern int watchdog_counter;
extern int memcard_write;

#endif
