/******************************************* 
**** VIDEO.C - Video Hardware Emulation **** 
*******************************************/ 
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
#include <string.h> 
#include <math.h> 
#include <kos.h> 

#include "video.h" 
#include "../neocd.h" 


//-- Defines ----------------------------------------------------------------- 
#define   LINE_BEGIN   mydword = *((int *)fspr) 
#define LINE_MID   mydword = *((int *)fspr+1) 
#define PIXEL_LAST   col = (mydword&0x0F); if (col) *bm = paldata[col] 
#define PIXEL_R      col = (mydword&0x0F); if (col) *bm = paldata[col]; bm-- 
#define PIXEL_F      col = (mydword&0x0F); if (col) *bm = paldata[col]; bm++ 
#define SHIFT7      mydword >>= 28 
#define SHIFT6      mydword >>= 24 
#define SHIFT5      mydword >>= 20 
#define SHIFT4      mydword >>= 16 
#define SHIFT3      mydword >>= 12 
#define SHIFT2      mydword >>= 8 
#define SHIFT1      mydword >>= 4 


//-- Global Variables -------------------------------------------------------- 
char *  video_vidram __attribute__((aligned(32))); 
//u16 *   neogeo_vidram16; 
unsigned short	*video_paletteram_ng __attribute__((aligned(32)));
unsigned short	video_palette_bank0_ng[4096] __attribute__((aligned(32)));
unsigned short	video_palette_bank1_ng[4096] __attribute__((aligned(32)));
unsigned short	*video_paletteram_pc __attribute__((aligned(32)));
unsigned short	video_palette_bank0_pc[4096] __attribute__((aligned(32)));
unsigned short	video_palette_bank1_pc[4096] __attribute__((aligned(32)));
unsigned short	video_color_lut[32768] __attribute__((aligned(32)));


short      video_modulo; 
unsigned short   video_pointer; 
unsigned short   *video_line_ptr[225];  
unsigned char   video_fix_usage[4096]__attribute__((aligned(32))); 
unsigned char   rom_fix_usage[4096]__attribute__((aligned(32))); 
unsigned char   video_spr_usage[32768]__attribute__((aligned(32)));
unsigned int   fc=0; 

unsigned char   video_shrinky[17]; 
char      full_y_skip[16]={0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; 

unsigned int   neogeo_frame_counter = 0; 
unsigned int   neogeo_frame_counter_speed = 4; 

unsigned int   video_hide_fps=0; 
int      video_scanlines_capable = 1; 
double      gamma_correction = 1.0; 

int      fullscreen_flag=0; 
int      display_mode=1; 
int      snap_no; 
int		fsaa_enabled;
//-- Function Prototypes ----------------------------------------------------- 
int   video_init(void); 
void   video_shutdown(void); 
void   video_draw_line(int); 
void   video_precalc_lut(void); 
void   video_flip_pages(void); 
void   video_draw_spr(unsigned int code, unsigned int color, int flipx, 
         int flipy, int sx, int sy, int zx, int zy); 
void   video_draw_screen1(void); 
void   video_setup(void); 


//---------------------------------------------------------------------------- 
pvr_ptr_t   vidram_tex; /**< Pointer to the PVR memory */
unsigned short   *vidram_buf; 

int  __inline__  video_init(void) 
{ 
    
   //int      y,i; 

   video_precalc_lut(); 

    
video_vidram = malloc(131072);


   vidram_tex = pvr_mem_malloc(512*512*2)-0xA0000000; 
   vidram_buf = (unsigned short *)memalign(32,512*512); 


   pvr_set_bg_color(0.0, 0.0, 0.0); 


	if (video_vidram==NULL) {
		printf("VIDEO: Could not allocate vidram (128k)\n");
		return	0;
	}


	sq_set32((void*)video_palette_bank0_ng, 0, sizeof(video_palette_bank0_ng));
	sq_set32((void*)video_palette_bank1_ng, 0, sizeof(video_palette_bank1_ng));
	sq_set32((void*)video_palette_bank0_pc, 0, sizeof(video_palette_bank0_pc));
	sq_set32((void*)video_palette_bank1_pc, 0, sizeof(video_palette_bank1_pc));
   /*
	memcpy_16bit((void*)video_palette_bank0_ng, 0, sizeof(video_palette_bank0_ng));
	memcpy_16bit((void*)video_palette_bank1_ng, 0, sizeof(video_palette_bank1_ng));
	memcpy_16bit((void*)video_palette_bank0_pc, 0, sizeof(video_palette_bank0_pc));
	memcpy_16bit((void*)video_palette_bank1_pc, 0, sizeof(video_palette_bank1_pc));
	*/
/*	
	   sq_set32(video_palette_bank0_ng, 0, 8192); 
   sq_set32(video_palette_bank1_ng, 0, 8192); 
   sq_set32(video_palette_bank0_pc, 0, 8192); 
   sq_set32(video_palette_bank1_pc, 0, 8192); 
*/
	video_paletteram_ng = video_palette_bank0_ng;
	video_paletteram_pc = video_palette_bank0_pc;
	video_modulo = 1;
	video_pointer = 0;
    video_set_mode(); 


   return 1; 
} 

//---------------------------------------------------------------------------- 
void   video_shutdown(void) 
{ 
   return; 
} 

//---------------------------------------------------------------------------- 
int   video_set_mode(void) 
{ 
   int      y; 

   for(y=0;y<225;y++) 
      video_line_ptr[y] = vidram_buf + y*512; 

   return 1; 
} 


//---------------------------------------------------------------------------- 
void    video_precalc_lut(void) 
{ 
   int   ndx, rr, rg, rb; 
    
   for(rr=0;rr<32;rr++) { 
      for(rg=0;rg<32;rg++) { 
         for(rb=0;rb<32;rb++) { 
            ndx = ((rr&1)<<14)|((rg&1)<<13)|((rb&1)<<12)|((rr&30)<<7) 
               |((rg&30)<<3)|((rb&30)>>1); 
            video_color_lut[ndx] = 
                 ((int)( 31 * pow( (double)rb / 31, 1 / gamma_correction ) )<<0) 
               |((int)( 63 * pow( (double)rg / 31, 1 / gamma_correction ) )<<5) 
               |((int)( 31 * pow( (double)rr / 31, 1 / gamma_correction ) )<<11); 
         } 
      } 
   } 
} 

//float u1=0.0f*(1.0f/512.0f), v1=0.0f*(1.0f/512.0f), u2=320.0f*(1.0f/512.0f), v2=224.0f*(1.0f/512.0f), z1=1, w=640, h=450; 
//---------------------------------------------------------------------------- 
void __inline__ blit() { 
   pvr_poly_hdr_t hdr; 
   pvr_vertex_t vert; 
   pvr_poly_cxt_t cxt; 

   //void *ptr; 

   //int y; 
 pvr_set_bg_color(0.0, 0.0, 0.0); 
   float u1=0.0f*(1.0f/512.0f), v1=0.0f*(1.0f/512.0f), u2=320.0f*(1.0f/512.0f), v2=224.0f*(1.0f/512.0f), x1, y1, z1=1, w=700, h=480; 


   x1 = (640 - w) / 2; 
   y1 = (480 - h) / 2; 


   pvr_wait_ready(); 
   pvr_scene_begin(); 
    
   pvr_list_begin(PVR_LIST_OP_POLY); 
   dcache_flush_range((int)vidram_buf, 512*225*2);
   	
   pvr_txr_load_dma(vidram_buf, vidram_tex, 512*225*2, 1); // dma 
  //pvr_txr_load(vidram_buf, vidram_tex, 512*225*2); 

  /* txr_filter */
 /*
  #define PVR_FILTER_NONE			0	
  #define PVR_FILTER_NEAREST		0
  #define PVR_FILTER_BILINEAR		2
  #define PVR_FILTER_TRILINEAR1		4
  #define PVR_FILTER_TRILINEAR2		6
  */


   pvr_poly_cxt_txr(&cxt, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565|PVR_TXRFMT_NONTWIDDLED, 512, 512, vidram_tex,  PVR_FILTER_NONE); 
   pvr_poly_compile(&hdr, &cxt); 


   pvr_prim(&hdr, sizeof(hdr)); 

   vert.argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);    
   vert.oargb = 0; 
   vert.flags = PVR_CMD_VERTEX; 
    
   vert.x = x1; 
   vert.y = y1; 
   vert.z = z1; 
   vert.u = u1; 
   vert.v = v1; 
   pvr_prim(&vert, sizeof(vert)); 
    
   vert.x = x1+w; 
   vert.y = y1; 
   vert.z = z1; 
   vert.u = u2; 
   vert.v = v1; 
   pvr_prim(&vert, sizeof(vert)); 
    
   vert.x = x1; 
   vert.y = y1+h; 
   vert.z = z1; 
   vert.u = u1; 
   vert.v = v2; 
   pvr_prim(&vert, sizeof(vert)); 
    
   vert.x = x1+w; 
   vert.y = y1+h; 
   vert.z = z1; 
   vert.u = u2; 
   vert.v = v2; 
   vert.flags = PVR_CMD_VERTEX_EOL; 
   pvr_prim(&vert, sizeof(vert)); 

   pvr_list_finish(); 
   pvr_scene_finish(); 

} 


//---------------------------------------------------------------------------- 
void __inline__ video_draw_screen1() 
{ 
   int         sx =0,sy =0,oy =0,my =0,zx = 1, rzy = 1; 
   int         offs,i,count,y; 
   int         tileno,tileatr,t1,t2,t3; 
   u8         fullmode=0; 
   int dday=0,rzx=15,yskip=0; 


 // for (y=0; y<224; y++) 
   //   sq_set16(video_line_ptr[y], video_paletteram_pc[4095], 320*2); 
         // Set LMMODE0 to 32bit moop
  *(volatile uint32_t*)0xa05F6884 = 0x1;
   		u32 palettetemp = (u32)video_paletteram_pc[4095]<<16;
		palettetemp |= video_paletteram_pc[4095];
		sq_set32(vidram_buf, palettetemp, 512*225*2); 
        for (count=0;count<0x300;count+=2) { 
        t3 = *((unsigned short *)( &video_vidram[0x10000 + count] )); 
        t1 = *((unsigned short *)( &video_vidram[0x10400 + count] )); 
  // Set LMMODE0 back to 64 bit moop
  *(volatile uint32_t*)0xa05F6884 = 0x0;
      // If this bit is set this new column is placed next to last one 
      if (t1 & 0x40) { 
         sx += (rzx + 1); 
         if ( sx >= 0x1F0 ) 
            sx -= 0x200; 

         // Get new zoom for this column 
         zx = (t3 >> 8)&0x0F; 

         sy = oy; 
      } else {   // nope it is a new block 
         // Sprite scaling 
         zx = (t3 >> 8)&0x0F; 

         rzy = t3 & 0xff; 
      t2 = *((unsigned short *)( &video_vidram[0x10800 + count] )); 
         sx = (t2 >> 7); 
         if ( sx >= 0x1F0 ) 
            sx -= 0x200; 

         sy = 0x1F0 - (t1 >> 7); 
         if (sy > 0x100) sy -= 0x200; 

         // Number of tiles in this strip 
         my = t1 & 0x3f; 
         if (my == 0x20) {
            fullmode = 1; 
			if(rzy == 0xff)
				while (sy < -16) sy += 2 * (rzy + 1); 
		 }
         else if (my >= 0x21) {
            fullmode = 2;   // most games use 0x21, but 
			while (sy < -16) sy += 2 * (rzy + 1); 
		 }
         else 
            fullmode = 0;   // Alpha Mission II uses 0x3f 

		if(my!=0x21 && rzy!=0xff && my!=0) 
            my=((my*16*256)/(rzy+1) + 15)/16; 

        if(my>0x20) my=0x20; 

         oy = sy; 
      } 

      rzx = zx; 

      // No point doing anything if tile strip is 0 
      if ((my==0)||(sx>311)) 
         continue; 

      // Setup y zoom 
      if(rzy==255) 
         yskip=16; 
      else 
         dday=0;   // =256; NS990105 mslug fix 

      offs = count<<6; 

      // my holds the number of tiles in each vertical multisprite block 
     // for (y=0; y < my ;y++) { 
  
         for (y = my ; y--;) {
         tileno  = *((unsigned short *)(&video_vidram[offs])); 
         offs+=2; 
         tileatr = *((unsigned short *)(&video_vidram[offs])); 
         offs+=2; 

         if (tileatr&0x8) 
            tileno = (tileno&~7)|(neogeo_frame_counter&7); 
         else if (tileatr&0x4) 
            tileno = (tileno&~3)|(neogeo_frame_counter&3); 
              
//         tileno &= 0x7FFF; 
         if (tileno>0x7FFF) 
            continue; 

         if (fullmode == 2 || (fullmode == 1 && rzy == 0xff)) 
         { 
            if (sy >= 248) sy -= 2 * (rzy + 1); 
         } 
         else if (fullmode == 1) 
         { 
            if (y == 0x10) sy -= 2 * (rzy + 1); 
         } 
         else if (sy > 0x110) sy -= 0x200; 

         if(rzy!=255) 
         { 
            yskip=0; 
            video_shrinky[0]=0; 
            for(i=0;i<16;i++) 
            { 
               video_shrinky[i+1]=0; 
               dday-=rzy+1; 
               if(dday<=0) 
               { 
                  dday+=256; 
                  yskip++; 
                  video_shrinky[yskip]++; 
               } 
               else 
                  video_shrinky[yskip]++; 
            } 
         } 

		if (((tileatr>>8)||(tileno!=0))&&(sy<225) && (sx>-8))
         { 
         
            video_draw_spr( 
               tileno, 
               tileatr >> 8, 
               tileatr & 0x01,tileatr & 0x02, 
               sx,sy,rzx,yskip); 
         } 

         sy +=yskip; 
      }  // for y 
   }  // for count 

   if (fc >= neogeo_frame_counter_speed) { 
      neogeo_frame_counter++; 
      fc=6; 
   } 

   fc++; 
  // draw_fix1();
  // draw_fix2(); 
   video_draw_fix(); 

} 



//---------------------------------------------------------------------------- 
void  __inline__ video_draw_screen2() 
{ 
   static int      pass1_start; 
   int         sx =0,sy =0,oy =0,my =0,zx = 1, rzy = 1; 
   int         offs,i,count,y; 
   int         tileno,tileatr,t1,t2,t3; 
   char         fullmode=0; 
   int         ddax=0,dday=0,rzx=15,yskip=0; 

 
   /* clear background  using store ques sh4*/ 
     // Set LMMODE0 to 32bit
  *(volatile uint32_t*)0xa05F6884 = 0x1;
	u32 palettetemp = (u32)video_paletteram_pc[4095]<<16;
		palettetemp |= video_paletteram_pc[4095];
		sq_set32(vidram_buf, palettetemp, 512*225*2);  
  // Set LMMODE0 back to 64 bit
  *(volatile uint32_t*)0xa05F6884 = 0x0;
        sx=0; sy=0; 

   t1 = *((unsigned short *)( &video_vidram[0x10400 + 4] )); 

   if ((t1 & 0x40) == 0) 
   { 
      for (pass1_start=6;pass1_start<0x300;pass1_start+=2) 
      { 
         t1 = *((unsigned short *)( &video_vidram[0x10400 + pass1_start] )); 

         if ((t1 & 0x40) == 0) 
            break; 
      } 
        
      if (pass1_start == 6) 
         pass1_start = 0; 
   } 
   else 
      pass1_start = 0;    

   for (count=pass1_start;count<0x300;count+=2) { 
      t3 = *((unsigned short *)( &video_vidram[0x10000 + count] )); 
      t1 = *((unsigned short *)( &video_vidram[0x10400 + count] )); 
      t2 = *((unsigned short *)( &video_vidram[0x10800 + count] )); 

      // If this bit is set this new column is placed next to last one 
      if (t1 & 0x40) { 
         sx += (rzx + 1); 
         if ( sx >= 0x1F0 ) 
            sx -= 0x200; 

         // Get new zoom for this column 
         zx = (t3 >> 8)&0x0F; 

         sy = oy; 
      } else {   // nope it is a new block 
         // Sprite scaling 
         zx = (t3 >> 8)&0x0F; 

         rzy = t3 & 0xff; 

         sx = (t2 >> 7); 
         if ( sx >= 0x1F0 ) 
            sx -= 0x200; 

         // Number of tiles in this strip 
         my = t1 & 0x3f; 
         if (my == 0x20) 
            fullmode = 1; 
         else if (my >= 0x21) 
            fullmode = 2;   // most games use 0x21, but 
         else 
            fullmode = 0;   // Alpha Mission II uses 0x3f 

         sy = 0x1F0 - (t1 >> 7); 
         if (sy > 0x100) sy -= 0x200; 
          
         if (fullmode == 2 || (fullmode == 1 && rzy == 0xff)) 
         { 
            while (sy < -16) sy += 2 * (rzy + 1); 
         } 
         oy = sy; 

           if(my==0x21) my=0x20; 
         else if(rzy!=0xff && my!=0) 
            my=((my*16*256)/(rzy+1) + 15)/16; 

                  if(my>0x20) my=0x20; 

         ddax=0;   // setup x zoom 
      } 

      rzx = zx; 

      // No point doing anything if tile strip is 0 
      if ((my==0)||(sx>311)) 
         continue; 

      // Setup y zoom 
      if(rzy==255) 
         yskip=16; 
      else 
         dday=0;   // =256; NS990105 mslug fix 

      offs = count<<6; 

      // my holds the number of tiles in each vertical multisprite block 
  //    for (y=0; y < my ;y++) { 
          
               // my holds the number of tiles in each vertical multisprite block 
     // for (y=0; y < my ;y++) { 
  
         for (y = my ; y--;) {
         tileno  = *((unsigned short *)(&video_vidram[offs])); 
         offs+=2; 
         tileatr = *((unsigned short *)(&video_vidram[offs])); 
         offs+=2; 

         if (tileatr&0x8) 
            tileno = (tileno&~7)|(neogeo_frame_counter&7); 
         else if (tileatr&0x4) 
            tileno = (tileno&~3)|(neogeo_frame_counter&3); 
              
//         tileno &= 0x7FFF; 
         if (tileno>0x7FFF) 
            continue; 

         if (fullmode == 2 || (fullmode == 1 && rzy == 0xff)) 
         { 
            if (sy >= 225) sy -= 2 * (rzy + 1); 
         } 
         else if (fullmode == 1) 
         { 
            if (y == 0x10) sy -= 2 * (rzy + 1); 
         } 
         else if (sy > 0x100) sy -= 0x200; 

         if(rzy!=255) 
         { 
            yskip=0; 
            video_shrinky[0]=0; 
            for(i=0;i<16;i++) 
            { 
               video_shrinky[i+1]=0; 
               dday-=rzy+1; 
               if(dday<=0) 
               { 
                  dday+=256; 
                  yskip++; 
                  video_shrinky[yskip]++; 
               } 
               else 
                  video_shrinky[yskip]++; 
            } 
         } 
		if (((tileatr>>8)||(tileno!=0))&&(sy<225) && (sx>-8)) 
        { 
            video_draw_spr( 
               tileno, 
               tileatr >> 8, 
               tileatr & 0x01,tileatr & 0x02, 
               sx,sy,rzx,yskip); 
         } 

         sy +=yskip; 
      }  // for y 
   }  // for count 

   for (count=0;count<pass1_start;count+=2) { 
      t3 = *((unsigned short *)( &video_vidram[0x10000 + count] )); 
      t1 = *((unsigned short *)( &video_vidram[0x10400 + count] )); 
      t2 = *((unsigned short *)( &video_vidram[0x10800 + count] )); 

      // If this bit is set this new column is placed next to last one 
      if (t1 & 0x40) { 
         sx += (rzx + 1); 
         if ( sx >= 0x1F0 ) 
            sx -= 0x200; 

         // Get new zoom for this column 
         zx = (t3 >> 8)&0x0F; 

         sy = oy; 
      } else {   // nope it is a new block 
         // Sprite scaling 
         zx = (t3 >> 8)&0x0F; 

         rzy = t3 & 0xff; 

         sx = (t2 >> 7); 
         if ( sx >= 0x1F0 ) 
            sx -= 0x200; 

         // Number of tiles in this strip 
         my = t1 & 0x3f; 
         if (my == 0x20) 
            fullmode = 1; 
         else if (my >= 0x21) 
            fullmode = 2;   // most games use 0x21, but 
         else 
            fullmode = 0;   // Alpha Mission II uses 0x3f 

         sy = 0x1F0 - (t1 >> 7); 
         if (sy > 0x100) sy -= 0x200; 
          
         if (fullmode == 2 || (fullmode == 1 && rzy == 0xff)) 
         { 
            while (sy < -16) sy += 2 * (rzy + 1); 
         } 
         oy = sy; 

           if(my==0x21) my=0x20; 
         else if(rzy!=0xff && my!=0) 
            my=((my*16*256)/(rzy+1) + 15)/16; 

                  if(my>0x20) my=0x20; 

         ddax=0;   // setup x zoom 
      } 

      rzx = zx; 

      // No point doing anything if tile strip is 0 
      if ((my==0)||(sx>311)) 
         continue; 

      // Setup y zoom 
      if(rzy==255) 
         yskip=16; 
      else 
         dday=0;   // =256; NS990105 mslug fix 

      offs = count<<6; 

      // my holds the number of tiles in each vertical multisprite block 
    //  for (y=0; y < my ;y++) { 
          
               // my holds the number of tiles in each vertical multisprite block 
     // for (y=0; y < my ;y++) { 
  
         for (y = my ; y--;) {
         tileno  = *((unsigned short *)(&video_vidram[offs])); 
         offs+=2; 
         tileatr = *((unsigned short *)(&video_vidram[offs])); 
         offs+=2; 

         if (tileatr&0x8) 
            tileno = (tileno&~7)|(neogeo_frame_counter&7); 
         else if (tileatr&0x4) 
            tileno = (tileno&~3)|(neogeo_frame_counter&3); 
              
//         tileno &= 0x7FFF; 
         if (tileno>0x7FFF) 
            continue; 

         if (fullmode == 2 || (fullmode == 1 && rzy == 0xff)) 
         { 
            if (sy >= 248) sy -= 2 * (rzy + 1); 
         } 
         else if (fullmode == 1) 
         { 
            if (y == 0x10) sy -= 2 * (rzy + 1); 
         } 
         else if (sy > 0x110) sy -= 0x200; 

         if(rzy!=255) 
         { 
            yskip=0; 
            video_shrinky[0]=0; 
            for(i=0;i<16;i++) 
            { 
               video_shrinky[i+1]=0; 
               dday-=rzy+1; 
               if(dday<=0) 
               { 
                  dday+=256; 
                  yskip++; 
                  video_shrinky[yskip]++; 
               } 
               else 
                  video_shrinky[yskip]++; 
            } 
         } 

            if (((tileatr>>8)||(tileno!=0))&&(sy<225) && (sx>-8))
         { 
            video_draw_spr( 
               tileno, 
               tileatr >> 8, 
               tileatr & 0x01,tileatr & 0x02, 
               sx,sy,rzx,yskip); 
         } 

         sy +=yskip; 
      }  // for y 
   }  // for count 

   if (fc >= neogeo_frame_counter_speed) { 
      neogeo_frame_counter++; 
      fc=6; 
   } 

   fc++; 
  // draw_fix1();
  // draw_fix2(); 
    video_draw_fix();

} 

//---------------------------------------------------------------------------- 
//       Without  flip           With Flip 
// 01: X0000000 00000000   00000000 0000000X 
// 02: X0000000 X0000000   0000000X 0000000X 
// 03: X0000X00 00X00000   00000X00 00X0000X 
// 04: X000X000 X000X000   000X000X 000X000X 
// 05: X00X00X0 0X00X000   000X00X0 0X00X00X 
// 06: X0X00X00 X0X00X00   00X00X0X 00X00X0X 
// 07: X0X0X0X0 0X0X0X00   00X0X0X0 0X0X0X0X 
// 08: X0X0X0X0 X0X0X0X0   0X0X0X0X 0X0X0X0X 
// 09: XX0X0X0X X0X0X0X0   0X0X0X0X X0X0X0XX 
// 10: XX0XX0X0 XX0XX0X0   0X0XX0XX 0X0XX0XX 
// 11: XXX0XX0X X0XX0XX0   0XX0XX0X X0XX0XXX 
// 12: XXX0XXX0 XXX0XXX0   0XXX0XXX 0XXX0XXX 
// 13: XXXXX0XX XX0XXXX0   0XXXX0XX XX0XXXXX 
// 14: XXXXXXX0 XXXXXXX0   0XXXXXXX 0XXXXXXX 
// 15: XXXXXXXX XXXXXXX0   0XXXXXXX XXXXXXXX 
// 16: XXXXXXXX XXXXXXXX   XXXXXXXX XXXXXXXX 

//---------------------------------------------------------------------------- 
void   video_draw_spr(unsigned int code, unsigned int color, int flipx, 
         int flipy, int sx, int sy, int zx, int zy) 
{ 
   int                  oy, ey, y, dy; 
   unsigned short         *bm; 
   int                  col; 
   int                  l; 
   int                  mydword; 
   unsigned char         *fspr = 0; 
   char               *l_y_skip; 
   const unsigned short   *paldata; 

   //if (sx <= -8)       return; //moved to the outside of this function.


   if(zy == 16) 
       l_y_skip = full_y_skip; 
   else 
       l_y_skip = video_shrinky; 

   fspr = neogeo_spr_memory; 

   // Mish/AJP - Most clipping is done in main loop 
   oy = sy; 
     ey = sy + zy -1;    // Clip for size of zoomed object 

   if (sy < 0) 
      sy = 0; 
   if (ey >= 225) 
      ey = 223; 

   if (flipy)   // Y flip 
   { 
      dy = -8; 
   //   fspr += (code+1)*128 - 8 - (sy-oy)*8; orgin code from pc source base 
         fspr += ((code+1)<<7) - 8 - ((sy-oy)<<3); // Shift Sh4 saves 1 to 2 cycles maybe? 
   } 
   else      // normal 
   { 
      dy = 8; 
   //   fspr += code*128 + (sy-oy)*8; //orgin code from pc source base 
         fspr += ((code)<<7) + ((sy-oy)<<3); 
   } 

   paldata = &video_paletteram_pc[color*16]; 
    
   if (flipx)   // X flip 
   { 
      l=0; 
          switch(zx) { 
      case   0: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_MID; 
            SHIFT7; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   1: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 1; 
            LINE_BEGIN; 
            SHIFT7; 
            PIXEL_R; 
            LINE_MID; 
            SHIFT7; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   2: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 2; 
            LINE_BEGIN; 
            SHIFT5; 
            PIXEL_R; 
            LINE_MID; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT5; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   3: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 3; 
            LINE_BEGIN; 
            SHIFT3; 
            PIXEL_R; 
            SHIFT4; 
            PIXEL_R; 
            LINE_MID; 
            SHIFT3; 
            PIXEL_R; 
            SHIFT4; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   4: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 4; 
            LINE_BEGIN; 
            SHIFT3; 
            PIXEL_R; 
            SHIFT3; 
            PIXEL_R; 
            LINE_MID; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT3; 
            PIXEL_R; 
            SHIFT3; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   5: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 5; 
            LINE_BEGIN; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT3; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            LINE_MID; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT3; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_LAST; 
            l++; 
         } 
         break;        
      case   6: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 6; 
            LINE_BEGIN; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            LINE_MID; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   7: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 7; 
            LINE_BEGIN; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            LINE_MID; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_LAST; 
            l++; 
         } 
         break;        
      case   8: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 8; 
            LINE_BEGIN; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            LINE_MID; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   9: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 9; 
            LINE_BEGIN; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            LINE_MID; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break;        
      case   10: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 10; 
            LINE_BEGIN; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            LINE_MID; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break;        
      case   11: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 11; 
            LINE_BEGIN; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2;    
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            LINE_MID; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2;    
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break;        
      case   12: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 12; 
            LINE_BEGIN; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            LINE_MID; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT2; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break;        
      case   13: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 13; 
            LINE_BEGIN; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            LINE_MID; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break;        
      case   14: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 14; 
            LINE_BEGIN; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            LINE_MID; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   15: 
         for (y = sy;y <= ey;y++) 
         { 
            fspr += l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx + 15; 
            LINE_BEGIN; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            LINE_MID; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_R; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break;        
      } 
   } 
   else      // normal 
   { 
      l=0; 
          switch(zx) { 
      case   0: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   1: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   2: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT5; 
            PIXEL_F; 
            LINE_MID; 
            SHIFT2; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   3: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT4; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT4; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   4: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT3; 
            PIXEL_F; 
            SHIFT3; 
            PIXEL_F; 
            LINE_MID; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT3; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   5: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT3; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT3; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   6: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            LINE_MID; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   7: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   8: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   9: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   10: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   11: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2;    
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2;    
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   12: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT2; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   13: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   14: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      case   15: 
         for (y = sy ;y <= ey;y++) 
         { 
            fspr+=l_y_skip[l]*dy; 
            bm  = (video_line_ptr[y]) + sx; 
            LINE_BEGIN; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            LINE_MID; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_F; 
            SHIFT1; 
            PIXEL_LAST; 
            l++; 
         } 
         break; 
      } 
   } 

} 

