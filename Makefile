

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



#mbigtable -mnomacsave -O2 -fomit-frame-pointer -falign-loops -fstrict-aliasing -fno-gcse  -funroll-all-loops -funroll-loops  -DFRAME_POINTERS -fno-omit-frame-pointer -falign-functions=32 -falign-labels=32 -falign-loops=32 -falign-jumps=32
# -fbranch-probabilities -fdata-sections -DDBUGOUT -DFM_INTERNAL_TIMER -Wall -Wall -O1 -Q -fno-gcse -s -g -finline-limit=1000 -nostartfiles -v  -Wall 

KOS_LOCAL_CFLAGS=-I$(KOS_BASE)/addons/include/SDL -I.  -Iz80 -DNDEBUG -DDREAMCAST -DSDL -DLOWERCASEFILES -DUSE_PD4990A -DINLINE=__inline__ -D__LITTLE_ENDIAN__ -Ttext=0x8c010000  -nostdlib  



TARGET = v2.elf
OBJS= memory/memory.o
PROBJS = c68k/c68k.o c68k/c68kexec.o z80/z80.o  


OBJS += mem/memset.o mem/memcpy.o mem/memmove.o  sound/2610intf.o  z80/z80intrf.o  sound/sound.o dreamneo.o vmu/vmu.o cdrom/cdrom.o  cdaudio/cdaudiokos.o  sound/streams.o sound/ay8910.o sound/fm.o  sound/timer.o sound/ymdeltat.o  video/videopvr.o video/dmafix.o  swab.o input/input.o pd4990adc.o  


all: NeogeoSDL.bin  rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules
#c68k Confirmed working compiler Options
KOS_CFLAGS = -ml -m4-single-only   -Os  -finline-functions   -finline-limit=1000   -foptimize-register-move  
#KOS_CFLAGS = -ml -m4-single-only  -Wall -Os  -fdefer-pop -foptimize-sibling-calls -fcse-follow-jumps -fcse-skip-blocks -fexpensive-optimizations -fthread-jumps -fstrength-reduce -fpeephole -fforce-mem -ffunction-cse -finline -fdelete-null-pointer-checks -fschedule-insns2 -fsched-interblock -fsched-spec -fbranch-count-reg -freorder-blocks -fcommon -fgnu-linker -frerun-cse-after-loop -frerun-loop-opt
# -fgcse -frerun-cse-after-loop -frerun-loop-opt
# -fdelete-null-pointer-checks -fschedule-insns2 -fsched-interblock
# -fsched-spec -fbranch-count-reg -freorder-blocks -fcommon -fgnu-linker
# -fregmove -foptimize-register-move -fargument-alias -fstrict-aliasing
#-fident -fpeephole2 -fguess-branch-probability -fmath-errno
 
#KOS_CFLAGS =   -ml -m4-single-only  -Wall -Os -fno-gcse  -fstrict-aliasing  -fno-omit-frame-pointer -foptimize-register-move  -frerun-cse-after-loop -frerun-loop-opt -fargument-noalias-global  -fexpensive-optimizations 
#KOS_CFLAGS =   -ml -m4-single-only  -O2 -funroll-all-loops -fno-omit-frame-pointer -w
#KOS_CFLAGS =  -ml -m4-single-only -Wl   -O9 -fno-builtin -fargument-noalias-global  -fwritable-strings  -ffast-math -fno-omit-frame-pointer -mbigtable -mnomacsave -fschedule-insns2 -fexpensive-optimizations -fomit-frame-pointer -falign-loops -fstrict-aliasing -fno-gcse -ffast-math  -fargument-noalias-global
#KOS_CFLAGS =   -ml -m4-single-only  -Os -funroll-loops -finline-functions -freorder-blocks -funsigned-char  -fno-gcse -fno-optimize-sibling-calls -fno-omit-frame-pointer  -fsingle-precision-constant -fargument-noalias-global -falign-functions=32 -falign-labels=32 -falign-loops=32 -falign-jumps=32 -freduce-all-givs
#KOS_CFLAGS = -ml -m4-single-only -O3 -fomit-frame-pointer -falign-loops -fstrict-aliasing -fno-gcse -fno-omit-frame-pointer -ffast-math -falign-functions=32 -falign-labels=32 -falign-loops=32 -falign-jumps=32
#KOS_CFLAGS =   -ml -m4-single-only -O3  -fno-rtti -fno-exceptions -mdalign -fargument-noalias-global  -fwritable-strings -funroll-all-loops  -fno-gcse  -ffast-math -fno-omit-frame-pointer  -fno-optimize-sibling-calls -freduce-all-givs	-falign-functions=32 -falign-labels=32 -falign-loops=32 -falign-jumps=32

clean:
	-rm -f $(TARGET) $(OBJS) romdisk.* v2.bin 

rm-elf:
	-rm -f $(TARGET) romdisk.*

$(TARGET): $(OBJS) $(PROBJS) romdisk.o
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(PROBJS) romdisk.o -L$(KOS_BASE)/lib -lgcc -lSDL -lpng -lz  -lm -lkallisti -lc -lgcc

romdisk.img:
	$(KOS_GENROMFS) -f romdisk.img -d romdisk -v

romdisk.o: romdisk.img
	$(KOS_BASE)/utils/bin2o/bin2o romdisk.img romdisk romdisk.o

NeogeoSDL.bin: $(TARGET)
	$(KOS_OBJCOPY) -O binary -R .stack $(TARGET) v2.bin
	




