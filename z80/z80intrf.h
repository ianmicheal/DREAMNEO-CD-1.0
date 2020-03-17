#ifndef	Z80INTRF_H
#define Z80INRTF_H

#include "z80/cz80.h"
extern cz80_struc Cz80_struc;

#define mz80int(VEC) Cz80_Set_IRQ(&Cz80_struc, VEC)
#define mz80ClearPendingInterrupt() Cz80_Clear_IRQ(&Cz80_struc)

#define mz80exec(CIC) Cz80_Exec(&Cz80_struc,CIC)
#define mz80nmi() Cz80_Set_NMI(&Cz80_struc)

void cz80_z80_writeport16(u16 PortNo, u8 data);
u8 cz80_z80_readport16(u16 PortNo);



void z80_init(void);


extern u8 subcpu_memspace[0x10000];
//#define subcpu_memspace mame_z80mem

extern u32 sound_code;
extern u32 result_code;
extern u32 pending_command;
extern int z80_cycles;
extern int Z80_VBL_CYCLES;
extern u16 z80_bank[4];

#endif /* Z80INTRF_H */

