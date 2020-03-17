/******************************************
**** Fixed Text Layer Drawing Routines ****
******************************************/
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "video.h"
#include "../neocd.h"
static __inline__ void draw_font(unsigned short *br, unsigned short *paldata, unsigned *gfxdata);
void draw_fix(u16 code, u16 colour, u16 sx, u16 sy,u16 * palette,  char * fix_memory);
//#define draw_fix draw_fix_asm

/* Draw Single FIX character */
__inline__ void draw_fix(u16 code, u16 colour, u16 sx, u16 sy,u16 * palette,  char * fix_memory)
{
	u8 y;
	register u32 mydword;
	u32 * fix=(u32*)&(fix_memory[code<<5]);
	u16 * dest;
	u16 * paldata=&palette[colour];
	u16 col;

	for(y=0;y<8;y++)
	{
		dest    = video_line_ptr[sy+y]+sx;
		mydword  = *fix++;
		
		col = (mydword>> 0)&0x0f; if (col) dest[0] = paldata[col];
		col = (mydword>> 4)&0x0f; if (col) dest[1] = paldata[col];
		col = (mydword>> 8)&0x0f; if (col) dest[2] = paldata[col];
		col = (mydword>>12)&0x0f; if (col) dest[3] = paldata[col];
		col = (mydword>>16)&0x0f; if (col) dest[4] = paldata[col];
		col = (mydword>>20)&0x0f; if (col) dest[5] = paldata[col];
		col = (mydword>>24)&0x0f; if (col) dest[6] = paldata[col];
		col = (mydword>>28)&0x0f; if (col) dest[7] = paldata[col];
	}
}
//replacement?
  __inline__ void draw_fix2(u16 code, u16 colour, u16 sx, u16 sy,u16 * palette, char * fix_memory){
	u16 * br1[64];
	u16 * br = (u16 *)br1;
	unsigned short * paldata=&palette[colour];
	u32 * fix=(u32*)&(fix_memory[code<<5]);

	draw_font((unsigned short *) br, paldata, fix);
	
	u16 * dest;
	u8 y;
	for(y=0;y<8;y++)
	{
		dest = video_line_ptr[sy+y]+sx;
		if(br[0]) dest[0] = br[0];
		if(br[1]) dest[1] = br[1];
		if(br[2]) dest[2] = br[2];
		if(br[3]) dest[3] = br[3];
		if(br[4]) dest[4] = br[4];
		if(br[5]) dest[5] = br[5];
		if(br[6]) dest[6] = br[6];
		if(br[7]) dest[7] = br[7];
		br+=8;
	}

}
static __inline__ void draw_font(unsigned short *br, unsigned short *paldata, unsigned *gfxdata)
{
    int y;
    for(y=0;y<8;y++) {
	register unsigned int myword = gfxdata[0];
	br[0]=paldata[(myword)&0xf];
	br[1]=paldata[(myword>>4)&0xf];
	br[2]=paldata[(myword>>8)&0xf];
	br[3]=paldata[(myword>>12)&0xf];
	br[4]=paldata[(myword>>16)&0xf];
	br[5]=paldata[(myword>>20)&0xf];
	br[6]=paldata[(myword>>24)&0xf];
	br[7]=paldata[(myword>>28)&0xf];
	
	br+=8;
	gfxdata++;
    }
}







/* Draw entire Character Foreground */
void video_draw_fix(void)
{
	u16 x, y;
	u16 code, colour;
	u16 * fixarea=(u16 *)&video_vidram[0xe004];
 // for( y=28; y--; ) { 
	for (y=0; y < 28; y++)
	{
		for (x = 0; x < 40; x++)
		{
			code = fixarea[x << 5];

			colour = (code&0xf000)>>8;
			code  &= 0xfff;

			if(video_fix_usage[code])
				draw_fix(code,colour,(x<<3),(y<<3),video_paletteram_pc, neogeo_fix_memory);
		}
		fixarea++;
	}

}

