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


#include <kos.h>
#include "vmu.h"

/* defines VMU LCD resolution */
int vmu_lcd_update(void) {
vmu_set_icon(EFECERO_xpm);
uint8 logo_bits[48*32/8];
	int x, y;

	for (y=0; y<32; y++) {
		for (x=0; x<48/8; x++) {
			logo_bits[y*48/8 + x] = EFECERO_xpm[(31-y)*48/8 + ((48/8 - 1) - x)];
		}
	}
}

/* other VMU code can be done here */

int vmu_lcd_save(void) {
vmu_set_icon(EFECERO_save);
uint8 logo_bits[48*32/8];
	int x, y;

	for (y=0; y<32; y++) {
		for (x=0; x<48/8; x++) {
			logo_bits[y*48/8 + x] = EFECERO_save[(31-y)*48/8 + ((48/8 - 1) - x)];
		}
	}
}



int vmu_lcd_region(void) {
vmu_set_icon(EFECERO_xpm);
uint8 logo_bits[48*32/8];
	int x, y;

	for (y=0; y<32; y++) {
		for (x=0; x<48/8; x++) {
			logo_bits[y*48/8 + x] = EFECERO_region[(31-y)*48/8 + ((48/8 - 1) - x)];
		}
	}
}
