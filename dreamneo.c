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
#include <unistd.h> 
#include <kos.h> 
#include <png/png.h> 
#include <zlib/zlib.h> 
#include <arch/timer.h>
#include <arch/arch.h>
#include <string.h> 

#include "neocd.h" 


#define REGION_JAPAN  0 
#define REGION_USA    1 
#define REGION_EUROPE 2 

#define REGION1 REGION_JAPAN 
#define REGION2 REGION_USA 
#define REGION3 REGION_EUROPE 
/*** M68K Illegal Op Exceptions ***/
#define NEOGEO_END_UPLOAD 	0xFABF
#define CDROM_LOAD_FILES		0xFAC0
#define NEOGEO_EXIT_CDPLAYER	0xFAC4
#define NEOGEO_PROGRESS_SHOW 0xFAC5
#define NEOGEO_START_UPLOAD	0xFAC6
#define NEOGEO_IPL			0xFAC7
#define NEOGEO_IPL_END		0xFAC8
#define NEOGEO_TRACE		0xFACE

/* romdisk */ 
extern uint8 romdisk[]; 
KOS_INIT_ROMDISK(romdisk); 

//-- Private variables ------------------------------------------------------- 

static int      cdda_was_playing = 1; 

//-- Global Variables -------------------------------------------------------- 
char         *neogeo_rom_memory = NULL; 
char         *neogeo_prg_memory = NULL; 
char         *neogeo_fix_memory = NULL; 
char         *neogeo_spr_memory = NULL; 
char         *neogeo_pcm_memory = NULL; 
unsigned char startup_bin;
unsigned char      neogeo_memorycard[8192]__attribute__((aligned(32))); 
int         neogeo_prio_mode = 0; 
int         neogeo_ipl_done = 0; 
//char         neogeo_region=REGION2; // usa 
char         neogeo_region=REGION1;  // japan
unsigned char      config_game_name[128]__attribute__((aligned(32))); 

int FPS = 59.185606; /* Correct fps from dev kit IM */ 
u32      neocd_time; /* next time marker */ 
extern int sound;
int	neogeo_frameskip= 0;
int unsigned vsync_freq = 60; 
 
static int frameskip_counter = 0;

volatile int frame_counter = 0;
int fps; 
static int neogeo_frameskip_count=0;
static unsigned lastime, mytime =0, elapsed, mycounter; 
 






//-- 68K Core related stuff -------------------------------------------------- 
//char            *OP_ROM; 

//-- Function Prototypes ----------------------------------------------------- 
void   neogeo_init(void); 
void   neogeo_reset(void); 
void   neogeo_hreset(void); 
void   neogeo_shutdown(void); 
void   MC68000_Cause_Interrupt(int); 
void   neogeo_exception(void); 
void   neogeo_run(void); 
void   draw_main(void); 
void   neogeo_quit(void); 
void   not_implemented(void); 
void   neogeo_machine_settings(void); 
void   neogeo_debug_mode(void); 
void   neogeo_cdda_check(void); 
void   neogeo_cdda_control(void); 
void   neogeo_do_cdda( int command, int trck_number_bcd); 
void   neogeo_read_gamename(void); 

file_t f; 
char * fixtmp; 
/* textures */ 
pvr_ptr_t back_tex; 
/* init background */ 
void back_init() 
{ 
    back_tex = pvr_mem_malloc(512*512*2); 
    png_to_texture("/cd/main.png", back_tex, PNG_NO_ALPHA); 
} 

/* init load_bios */ 
   void load_bios() 
   { 
   FILE         *fp; 
   bfont_draw_str(vram_s+(10)*640+(10), 640,480, "DREAMNEO CD STARTUP NOW LOADING... Please wait"); 
   printf("PRG (2MB) ... ");
   SDL_Delay(1*1000);
   neogeo_prg_memory = memalign(32,2097152);
   if (neogeo_prg_memory ==NULL) { 
   bfont_draw_str(vram_s+(40)*640+(10), 640,480,"PRG (2MB) ... Failed !");
   printf("failed !"); 
   SDL_Delay(1*1000);
   return ; 
   } 
   
  // bfont_draw_str(vram_s+(40)*640+(10), 640,480,"PRG (2MB) ...  OK");
   printf("SPR (4MB) ... "); 
   SDL_Delay(1*1000);
   neogeo_spr_memory = memalign(32, 4194304); 
   if (neogeo_spr_memory ==NULL) { 
   bfont_draw_str(vram_s+(40)*640+(10), 640,480,"SPR ... Failed !");
   printf("failed !");
   SDL_Delay(1*1000); 
   return ; 
   } 
  // bfont_draw_str(vram_s+(88)*640+(10), 640,480,"SPR (4MB) ...  OK"); 
   printf("PCM (1Mb) ... ");
   SDL_Delay(1*1000); 
   neogeo_pcm_memory = memalign(32, 1048576); 
   if (neogeo_pcm_memory  ==NULL) { 
   bfont_draw_str(vram_s+(88)*640+(10), 640,480,"PCM (1Mb) ... Failed !"); 
   printf("failed !");
   SDL_Delay(1*1000); 
   return ; 
   } 
 //  bfont_draw_str(vram_s+(130)*640+(10), 640,480,"PCM (1Mb) ... OK"); 
/* Wait 5 seconds */
   printf("Waiting 1 seconds\n");
 //  bfont_draw_str(vram_s+(200)*640+(40),640,480,"Waiting 5 seconds ... OK"); 
   SDL_Delay(1*1000);
   
   
/* Initialize Memory Mapping */
   initialize_memmap(); 
 
   neogeo_rom_memory = (char *)calloc(1, 524288);
   if (neogeo_rom_memory==NULL) { 
   bfont_draw_str(vram_s+(230)*640+(10), 640,480, "ROM (512kb) ... Failed !");
   printf("ROM (512kb) ... Failed !"); 
      return ; 
   } 
   
   bfont_draw_str(vram_s+(40)*640+(10), 640,480, "ROM (512kb) ... OK");
   printf("ROM (512kb) ... OK "); 
   SDL_Delay(1*1000);
   vid_clear(0,0,0);
   neogeo_fix_memory = (char *)calloc(1, 131072); 
   if (neogeo_fix_memory==NULL) { 
   printf("FIX (128kb) ... Failed !"); 
   bfont_draw_str(vram_s+(40)*640+(10), 640,480,"FIX (128kb) ... Failed !");
      return ; 
   }  
 //  bfont_draw_str(vram_s+(88)*640+(10), 640,480,"FIX (128kb) ... OK");
   SDL_Delay(1*1000);
   f = fs_open("/rd/neocd.bin", O_RDONLY); 
   if(!f) {
   f = fs_open("/cd/neocd.bin", O_RDONLY); 
   if (!f) { 
   printf("Fatal Error: Could not load NEOCD.BIN\n");
   bfont_draw_str(vram_s+(400)*640+(0),640,1,"Could not load NEOCD.BIN"); 
   return; 
   } 
   }
    bfont_draw_str(vram_s+(400)*640+(0),640,1,"LOADED NEOCD.BIN ... !"); 
   fs_read(f, neogeo_rom_memory, 524288); 
   fs_close(f); 
    vid_clear(0,0,0);
 
           /* convert bios FIX data */ 
   char *fixtmp=(char *)calloc(1, 131072);
   memcpy(fixtmp,&neogeo_rom_memory[524288],131072);
   fix_conv((unsigned char *)fixtmp,(unsigned char *)&neogeo_rom_memory[524288],131072,(unsigned char *)rom_fix_usage);
   
  // swab(neogeo_rom_memory,524288, neogeo_rom_memory, 131072);
  // free(fixtmp);
  // fixtmp=NULL;
   // Load startup RAM 
   fp = fopen("/rd/startup.bin", "rb"); 
  fread(neogeo_prg_memory + 0x10F300, 1, 3328, fp); 
   fclose(fp); 
   swab(neogeo_prg_memory + 0x10F300, neogeo_prg_memory + 0x10F300, 3328); 
   //swab(3328, neogeo_prg_memory + 0x10f300, 0xd00);

	/*** Patch BIOS load files with/without image to use the BIOS routines to decode
           the actual filenames needed. This makes more sense to me and safeguards against
	 unknown filetypes. ***/
 //   *((short*)(neogeo_rom_memory+0xDBFC)) = 0x4E71;   
  //  *((short*)(neogeo_rom_memory+0xDBFE)) = 0x4E71;  
  //  *((short*)(neogeo_rom_memory+0xDC24)) = 0x4E71;
  //  *((short*)(neogeo_rom_memory+0xDC26)) = 0x4E71;
  // *((short*)(neogeo_rom_memory+0xDD42)) = CDROM_LOAD_FILES;
  //  *((short*)(neogeo_rom_memory+0xDD44)) = 0x4E71;
  //  *((short*)(neogeo_rom_memory+0xDD46)) = 0x4E71;
    
    

	/*** Hook for loading animations ***/
//	*((short*)(neogeo_rom_memory+0xDE48)) = NEOGEO_PROGRESS_SHOW;
  //  *((short*)(neogeo_rom_memory+0xDE4A)) = 0x4E71;
  //  *((short*)(neogeo_rom_memory+0xDE4C)) = 0x4E71;
 //   *((short*)(neogeo_rom_memory+0xDE4E)) = 0x4E71;
  //  *((short*)(neogeo_rom_memory+0xDE72)) = 0x4E71;
  //  *((short*)(neogeo_rom_memory+0xDE74)) = 0x4E71;
   
   
     /*** CD player ***/ 
  //*((uint16*)(neogeo_rom_memory+0x120)) = 0x55E; 
	/*** Reuse FABF for end of file loading cleanups ***/
//  *((short*)(neogeo_rom_memory+0xDC54)) = NEOGEO_END_UPLOAD;
	/*** Patch start of loading with image ***/
//  *((short*)(neogeo_rom_memory+0xDB80)) = NEOGEO_START_UPLOAD;

	/*** Hook for loading animations ***/
	*((short*)(neogeo_rom_memory+0xDE48)) = NEOGEO_PROGRESS_SHOW;
    *((short*)(neogeo_rom_memory+0xDE4A)) = 0x4E71;
    *((short*)(neogeo_rom_memory+0xDE4C)) = 0x4E71;
    *((short*)(neogeo_rom_memory+0xDE4E)) = 0x4E71;
    *((short*)(neogeo_rom_memory+0xDE72)) = 0x4E71;
    *((short*)(neogeo_rom_memory+0xDE74)) = 0x4E71;

   /*** Patch BIOS load files w/ now loading message ***/ 
   *((short*)(neogeo_rom_memory+0x552)) = 0xFABF; 
   *((short*)(neogeo_rom_memory+0x554)) = 0x4E75; 
   /*** Patch BIOS load files w/ out now loading ***/ 
   *((short*)(neogeo_rom_memory+0x564)) = 0xFAC0; 
   *((short*)(neogeo_rom_memory+0x566)) = 0x4E75; 
   	/*** Hook for loading animations ***/
	*((short*)(neogeo_rom_memory+0xDE48)) = NEOGEO_PROGRESS_SHOW;
   /*** Patch BIOS CDROM Check ***/ 
   *((short*)(neogeo_rom_memory+0xB040)) = 0x4E71; 
   *((short*)(neogeo_rom_memory+0xB042)) = 0x4E71; 
   
   	/*** Patch BIOS upload command ***/
//	*((short*)(neogeo_rom_memory+0x546)) = 0xFAC1;
//	*((short*)(neogeo_rom_memory+0x548)) = 0x4E75;

     /*** Patch BIOS upload command  2***/
   *((short*)(neogeo_rom_memory+0xDB80)) = NEOGEO_START_UPLOAD;
   *((short*)(neogeo_rom_memory+0xDB80)) = NEOGEO_START_UPLOAD;
   /*** Patch BIOS CDDA check ***/ 
   *((short*)(neogeo_rom_memory+0x56A)) = 0xFAC3; 
   *((short*)(neogeo_rom_memory+0x56C)) = 0x4E75; 

   /*** Full reset, please ***/ 
   *((short*)(neogeo_rom_memory+0xA87A)) = 0x4239; 
   *((short*)(neogeo_rom_memory+0xA87C)) = 0x0010; 
   *((short*)(neogeo_rom_memory+0xA87E)) = 0xFDAE; 
   *((short*)(neogeo_rom_memory+0x120)) = 0x55E; 
   /*** Trap exceptions ***/ 
   *((short*)(neogeo_rom_memory+0xA5B6)) = 0x4AFC; 
   /*** System checks ***/
   *((short*)(neogeo_rom_memory+0xAD36)) = 5;
   /*** CD player ***/ 
   *((short*)(neogeo_rom_memory+0x120)) = 0x55E; 
      /*** CD player  Exit***/ 
   *((short*)(neogeo_rom_memory+0x120)) = NEOGEO_EXIT_CDPLAYER; 
      // Check BIOS validity 
   if (*((uint16*)(neogeo_rom_memory+0xA822)) != 0x4BF9) 
   { 
      printf("Fatal Error: Invalid BIOS file."); 
	  bfont_draw_str(vram_s+(200)*640+(40),640,1,"BIOS Invalid Patching failed no fix"); 
	  SDL_Delay(5*1000);
      return ; 
    } 
    bfont_draw_str(vram_s+(88)*640+(10), 640,480,"BIOS PATCHING LOADED ... ok");
    SDL_Delay(1*1000);
   } 


/* draw background */ 
void draw_back(void) 
{ 
    pvr_poly_cxt_t cxt; 
    pvr_poly_hdr_t hdr; 
    pvr_vertex_t vert; 

    pvr_poly_cxt_txr(&cxt, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565, 512, 512, back_tex,PVR_FILTER_NEAREST); 
    pvr_poly_compile(&hdr, &cxt); 
    pvr_prim(&hdr, sizeof(hdr)); 

    vert.argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);    
    vert.oargb = 0; 
    vert.flags = PVR_CMD_VERTEX; 
    
    vert.x = 1; 
    vert.y = 1; 
    vert.z = 1; 
    vert.u = 0.0; 
    vert.v = 0.0; 
    pvr_prim(&vert, sizeof(vert)); 
    
    vert.x = 640; 
    vert.y = 1; 
    vert.z = 1; 
    vert.u = 1.0; 
    vert.v = 0.0; 
    pvr_prim(&vert, sizeof(vert)); 
    
    vert.x = 1; 
    vert.y = 480; 
    vert.z = 1; 
    vert.u = 0.0; 
    vert.v = 1.0; 
    pvr_prim(&vert, sizeof(vert)); 
    
    vert.x = 640; 
    vert.y = 480; 
    vert.z = 1; 
    vert.u = 1.0; 
    vert.v = 1.0; 
    vert.flags = PVR_CMD_VERTEX_EOL; 
    pvr_prim(&vert, sizeof(vert)); 
} 

/* draw one frame */ 
void draw_frame1(void) 
{ 
    pvr_wait_ready(); 
    pvr_scene_begin(); 
    
    pvr_list_begin(PVR_LIST_OP_POLY); 
    draw_back(); 
    pvr_list_finish(); 

    pvr_list_begin(PVR_LIST_TR_POLY); 

    
    pvr_list_finish(); 
    pvr_scene_finish(); 


} 
//---------------------------------------------------------------------------- 
int   main(int argc, char* argv[]) 
{ 
      
   // Displays version number, date and time
	printf(VERSION1);
	printf("%d.%d\n",VERSION2);
	printf(PORTBY);
	
    FILE         *fp; 
    int            result; 
    int done = 0;
    /* init kos  */ 
    pvr_init_defaults(); 
    /* init background */ 
    back_init(); 
	 /* Loads VMU LCD icon */
    vmu_lcd_update();


   /* init bios function  */ 
   load_bios(); 
   draw_frame1();   

   bfont_draw_str(vram_s+(220)*640+(0),640,1,"    Press Start");
  // wait for door to be opened.
  int disc_status, disc_type;
  cdrom_get_status(&disc_status, &disc_type);
  usleep(100);
  draw_frame1(); 

   /* keep drawing frames until start is pressed */
    while(!done) {
        MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)
            if (st->buttons & CONT_START)
                done = 1;
        MAPLE_FOREACH_END()
        draw_frame1();
/* wait for a CD */ 
    while(cdrom_init1() ) { 
		cdrom_get_status(&disc_status,&disc_type);
       usleep(90);   
    }  
}
    //Free the splash screen
    pvr_mem_free(back_tex); 
   // Allocate needed memory buffers 
   bfont_draw_str(vram_s+(10)*640+(10), 640,480, "...Loading..."); 
   // Register exit procedure 
 // atexit(neogeo_shutdown); 
file_t vmu; 
vmu = fs_open("/vmu/a1/memcard.bin",O_RDONLY); 
if (vmu) { 
fs_seek(vmu, 640, SEEK_SET); 
fs_read(vmu, neogeo_memorycard,8192); 
fs_close(vmu); 
} else { 
fp = fopen("/rd/memcard.bin", "rb"); 

fread(neogeo_memorycard, 1, 8192, fp); 
fclose(fp); 
} 
	
   // Video init 
   video_init();
   pvr_dma_init();   
  // Initialise input
   printf("Initialize input...\n");
   input_init();
   // Initialize CD-ROM 
   cdda_current_drive=cdrom_current_drive; 
   // Sound init 
   init_sdl_audio(); 
   // Initialize CD-ROM
   printf("Initialize cdrom...\n");
   cdda_init();
		
   // Initialize everything
	neogeo_init();
			
	pd4990a_init();
			
	neogeo_run();


   return 0; 
} 

FASTCALL void	neogeo_init(void)
{

    C68k_Init(&C68K, 0);

    C68k_Set_ReadB(&C68K, m68k_read_memory_8);
    C68k_Set_ReadW(&C68K, m68k_read_memory_16);

    C68k_Set_WriteB(&C68K, m68k_write_memory_8);
    C68k_Set_WriteW(&C68K, m68k_write_memory_16);
    
    C68k_Set_Fetch(&C68K, 0x000000, 0x1FFFFF, (u32) neogeo_prg_memory);
    C68k_Set_Fetch(&C68K, 0xC00000, 0xC7FFFF, (u32) neogeo_rom_memory);
    
    C68k_Reset(&C68K);

}
   
 

//---------------------------------------------------------------------------- 
void   neogeo_hreset(void) 
{ 

   FILE * fp; 

   /* read game name */ 
   neogeo_read_gamename(); 


   
   vmu_lcd_region();
    vmu_lcd_update();
   
   

C68k_Reset(&C68K);
C68k_Set_PC(&C68K, 0xc0a822);
C68k_Set_SR(&C68K, 0x2700);
C68k_Set_AReg(&C68K, 7, 0x10F300);
C68k_Set_MSP(&C68K, 0x10F300);
C68k_Set_USP(&C68K, 0x10F300);

    
   m68k_write_memory_32(0x10F6EE, m68k_read_memory_32(0x68L)); // $68 *must* be copied at 10F6EE 

   if (m68k_read_memory_8(0x107)&0x7E) 
   { 
      if (m68k_read_memory_16(0x13A)) 
      { 
         m68k_write_memory_32(0x10F6EA, (m68k_read_memory_16(0x13A)<<1) + 0xE00000); 
      } 
      else 
      { 
         m68k_write_memory_32(0x10F6EA, 0); 
         m68k_write_memory_8(0x00013B, 0x01); 
      } 
   } 
   else 
      m68k_write_memory_32(0x10F6EA, 0xE1FDF0); 

   //check for holding start button
cont_cond_t regionswitch1; 
cont_get_cond(maple_first_controller(), &regionswitch1);
if(!(regionswitch1.buttons & CONT_START)){
   /* Set System Region */ 
    cont_cond_t regionswitch; 
   int regionloopend=1; 
   
   
   vid_clear(0,0,0);
   bfont_draw_str(vram_s+(10)*640+(10), 640,480, "Region Select by Quzar"); 
   bfont_draw_str(vram_s+(40)*640+(10), 640,480, "X for Japan");
   bfont_draw_str(vram_s+(64)*640+(10), 640,480, "Y for USA");   
   bfont_draw_str(vram_s+(88)*640+(10), 640,480, "B for Europe");

   bfont_draw_str(vram_s+(130)*640+(10), 640,480, "Current Region:"); 
   bfont_draw_str(vram_s+(130)*640+(190), 640,480, "USA"); 
   bfont_draw_str(vram_s+(200)*640+(40),640,480,"Hit A to continue..."); 

   while(regionloopend){ 
     
       
  cont_get_cond(maple_first_controller(), &regionswitch); 
      if(!(regionswitch.buttons & CONT_X))   { 
		  neogeo_region=REGION1; 
		  bfont_draw_str(vram_s+(130)*640+(190), 640,480, "      ");
		  bfont_draw_str(vram_s+(130)*640+(190), 640,480, "Japan"); 
	  }
	  if(!(regionswitch.buttons & CONT_Y))   { 
		  neogeo_region=REGION2;
		  bfont_draw_str(vram_s+(130)*640+(190), 640,480, "      ");
		  bfont_draw_str(vram_s+(130)*640+(190), 640,480, "USA"); 
	  }
	  if(!(regionswitch.buttons & CONT_B))   { 
		  neogeo_region=REGION3;
		  bfont_draw_str(vram_s+(130)*640+(190), 640,480, "      ");
		  bfont_draw_str(vram_s+(130)*640+(190), 640,480, "Europe"); 
	  }
       
if(!(regionswitch.buttons & CONT_A))   { 
   regionloopend=0; 
} 
   } 
//vid_empty();  
}
       

   m68k_write_memory_8(0x10FD83,neogeo_region); 

   cdda_current_track = 0; 
   cdda_get_disk_info(); 

   z80_init(); 
  // Cz80_Reset();
}    

//---------------------------------------------------------------------------- 
void   neogeo_reset(void) 
{ 

C68k_Reset(&C68K);
C68k_Set_PC(&C68K, 0x122);
C68k_Set_SR(&C68K, 0x2700);
C68k_Set_AReg(&C68K, 7, 0x10F300);
C68k_Set_MSP(&C68K, 0x10F300);
C68k_Set_USP(&C68K, 0x10F300);


   m68k_write_memory_8(0x10FD80, 0x82); 
   m68k_write_memory_8(0x10FDAF, 0x01); 
   m68k_write_memory_8(0x10FEE1, 0x0A); 
   m68k_write_memory_8(0x10F675, 0x01); 
   m68k_write_memory_8(0x10FEBF, 0x00); 
   m68k_write_memory_32(0x10FDB6, 0); 
   m68k_write_memory_32(0x10FDBA, 0); 
   
  	m68k_write_memory_32(0x120002, 0xffffffff);
	m68k_write_memory_32(0x11c808, 0xc0c760);	// load screen setup function
	m68k_write_memory_32(0x11c80c, 0xc0c814);	// load screen progress function
	m68k_write_memory_32(0x11c810, 0xc190e2);	// load screen default anime data
   /* System Region */ 
   m68k_write_memory_8(0x10FD83,neogeo_region); 

   cdda_current_track = 0; 
   z80_init(); 
} 

void   neogeo_save(void) 
{ 
SDL_PauseAudio(1);
       


uint16 palette[16] = { 
0xF07B, 0xFFC4, 0xF000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 
}; 

uint16 bitmap[256] = { 
0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 
0x3233, 0x3322, 0x2222, 0x2233, 0x3333, 0x2233, 0x2223, 0x3323, 
0x3233, 0x2333, 0x3323, 0x3332, 0x3323, 0x3332, 0x2333, 0x3332, 
0x3233, 0x2333, 0x2322, 0x3332, 0x2223, 0x3332, 0x2333, 0x3332, 
0x3233, 0x2333, 0x3323, 0x3332, 0x2223, 0x3332, 0x2333, 0x3332, 
0x3233, 0x2333, 0x2222, 0x2233, 0x3333, 0x2233, 0x2223, 0x3323, 
0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 
0x3333, 0x3333, 0x3233, 0x3222, 0x3322, 0x3323, 0x3333, 0x3333, 
0x3333, 0x3333, 0x2333, 0x3233, 0x2333, 0x3323, 0x3333, 0x3333, 
0x3333, 0x3333, 0x3233, 0x3223, 0x2333, 0x3323, 0x3333, 0x3333, 
0x3333, 0x3333, 0x3333, 0x3232, 0x2333, 0x3323, 0x3333, 0x3333, 
0x3333, 0x3333, 0x2233, 0x3223, 0x3322, 0x2322, 0x3333, 0x3333, 
0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 
0x3333, 0x2233, 0x3333, 0x3333, 0x2232, 0x2222, 0x3323, 0x3333, 
0x3333, 0x1132, 0x3323, 0x3333, 0x2222, 0x2222, 0x3322, 0x3333, 
0x3333, 0x1121, 0x3312, 0x3233, 0x2222, 0x2222, 0x2322, 0x3333, 
0x3233, 0x1111, 0x2311, 0x2233, 0x2222, 0x0220, 0x2222, 0x3333, 
0x2133, 0x1111, 0x1211, 0x2232, 0x2222, 0x0220, 0x2222, 0x3323, 
0x2133, 0x1111, 0x1111, 0x2222, 0x0220, 0x2222, 0x2222, 0x3322, 
0x2133, 0x1111, 0x1122, 0x2222, 0x0020, 0x2222, 0x2222, 0x3322, 
0x2133, 0x1111, 0x1122, 0x2222, 0x0022, 0x2222, 0x2222, 0x3322, 
0x2133, 0x1111, 0x1111, 0x2222, 0x2222, 0x2222, 0x0220, 0x3322, 
0x2133, 0x2111, 0x1111, 0x2222, 0x2222, 0x2222, 0x0220, 0x3322, 
0x2133, 0x2111, 0x1111, 0x1111, 0x2212, 0x2222, 0x0200, 0x3322, 
0x2133, 0x1111, 0x1111, 0x1112, 0x2211, 0x2022, 0x2200, 0x3322, 
0x2133, 0x1111, 0x1111, 0x1112, 0x1211, 0x2022, 0x2202, 0x3322, 
0x3233, 0x1111, 0x1111, 0x1122, 0x1111, 0x2222, 0x2222, 0x3322, 
0x3333, 0x1121, 0x1211, 0x1121, 0x1211, 0x2232, 0x2222, 0x3323, 
0x3333, 0x1132, 0x2212, 0x1111, 0x2311, 0x2233, 0x2222, 0x3333, 
0x3333, 0x2133, 0x1111, 0x1111, 0x3312, 0x3233, 0x2322, 0x3333, 
0x3333, 0x3233, 0x2222, 0x2222, 0x3323, 0x3333, 0x3322, 0x3333, 
0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333 
}; 

int i; 
file_t f; 
//FILE   *fp; 
uint8 buffer[4]; 

f = fs_open("/vmu/a1/memcard.bin", O_WRONLY); 
if (!f) return; 

//start of the header 
fs_write(f, "DREAM NEO CD V1.0    ", 16);         //vmdesc 
fs_write(f, "Memory Card                     ", 32);   //dcdesc 
fs_write(f, "DREAM NEO CD V1.0        ", 16);         //appname 

buffer[0]=1; 
buffer[1]=0; 
fs_write(f, buffer, 2);               //icons num 
fs_write(f, buffer, 2);               //anim speed 

buffer[0] = 0; 
fs_write(f, buffer, 2);               //eyecatch 
fs_write(f, buffer, 2);               //crc 

buffer[0]=0; 
buffer[1]=0; 
buffer[2]=0; 
buffer[3]=0; 
fs_write(f, buffer, 4);               //data length 

buffer[0] = 0; 
for (i=0; i<20; i+=2) 
fs_write(f, buffer, 2);            //reserved 
for (i=0; i<16; i++) { 
   buffer[0] = palette[i] % 256; 
   buffer[1] = palette[i] / 256; 
   fs_write(f, buffer, 2);            //icons pal 
} 
for (i=0; i<256; i++) { 
   buffer[0] = bitmap[i] % 256;    
   buffer[1] = bitmap[i] / 256; 
   fs_write(f, buffer, 2);            //icons bitmap 
} 
fs_write(f, neogeo_memorycard,  8192); 

fs_close(f); 
SDL_PauseAudio(0);
} 





//---------------------------------------------------------------------------- 
void   neogeo_shutdown(void) 
{ 
 //  pvr_shutdown();
 // pvr_mem_available(); 

   printf("NEOGEO: System Shutdown.\n"); 
    
   printf("NEOGEO: System Shutdown.\n"); 
    


   free(neogeo_prg_memory); 
   printf("neogeo_prg_memory: System Shutdown.\n");
   free(neogeo_rom_memory); 
   printf("neogeo_rom_memory: System Shutdown.\n");
   free(neogeo_spr_memory); 
   printf("neogeo_spr_memory: System Shutdown.\n");
   free(neogeo_fix_memory); 
   printf("neogeo_fix_memory: System Shutdown.\n");
   free(neogeo_pcm_memory); 
   printf("neogeo_pcm_memory: System Shutdown.\n");
   SDL_Delay(1500);
   SDL_Quit();
   	//vid_empty();
	pvr_mem_reset();
   void *subelf; 
   int length;	
    //   iso_ioctl(0,NULL,0);
    length = fs_load("/cd/menu.bin", &subelf); // Reload mod menu from Quake menu with quit  Ian micheal 2019
    arch_exec(subelf, length);
     bfont_draw_str(vram_s+(10)*640+(10), 640,480, "...Loading..menu..."); 
	Sys_Printf ("========Thanks for playing========\n");
    
//   SDL_Quit(); 
 
   return; 
} 

//---------------------------------------------------------------------------- 
void   neogeo_exception(void) 
{ 
 
       	/*** Trap exceptions ***/
	*((short*)(neogeo_rom_memory+0xA5B6)) = 0x4AFC;

  // exit(0); 
}    
////---------------------------------------------------------------------------- 
double Sys_FloatTime (void)
{
	uint32 sec, msecs;
	double timer_count;
	timer_ms_gettime(&sec, &msecs);
//	timer_count = sec + (msecs / 1000.0);
	timer_count = sec + (msecs / 0x10000000);
//	msecs = (uint32)(used * 1000.0 / timer_ms_countdown);
	return timer_count;
}

#define T0_COUNT ((u32 *)(0x10000000))
#define T0_MODE ((u32 *)(0x10000010))
#define T_MODE_CLKS_M 3
#define T_MODE_CUE_M 128
unsigned timer_gettime(void) {
  return (*T0_COUNT) & 0xFFFF;
}
////---------------------------------------------------------------------------- 
 void   neogeo_run(void) 
{ 
   int   i; 
    int frameskip =2;//2//3
//	int now = 0;
	int m68k_cycles = 12000000/FPS;
	int time = 0;
	char frameblit = 1;
	
	
	printf("START EMULATION...\n");
   // If IPL.TXT not loaded, load it ! 
   if (!neogeo_ipl_done) 
   { 
      // Display Title 
      cdrom_load_title(); 
		
      // Process IPL.TXT 
      if (!cdrom_process_ipl()) { 
         printf("Error: Error while processing IPL.TXT.\n"); 
         return; 
      } 
       		// copy game vectors
		memcpy(neogeo_game_vectors, neogeo_prg_memory, 0x80 );	
      // Reset everything 
      neogeo_ipl_done = 1; 
      neogeo_hreset(); 
   } 
		
   // Resume cdda if it was playing 
 //  if ((cdda_current_track != 0)&&(cdda_was_playing)) 
  //    cdda_resume(); 
  
//	sound_enable();

	for(;;)
   {    
 		// Time management
		double lastime = mytime;
	double	 mytime  = Sys_FloatTime();  
		double elapsed = mytime-lastime;
		 if (elapsed>0xffff)
		   elapsed = elapsed-0xffff0000; 
		 mycounter += elapsed;
		 if(mycounter>=vsync_freq)
		 {
		    fps=frame_counter;
		    // estimated framerate with frameskip
		     if (frameskip)
		    	fps<<=1;
		    if (fps>60)
		      fps = 60;
		           frameskip++;
            frameskip=frameskip%2;   //5000000; ///3ms //4
            frameskip=frameskip%2;
		    frame_counter=0;
		    mycounter=0; 
		    frame_counter=0;
		    mycounter=0;   
		 }  
   //    */
       //Execute Z80 timeslice (one VBL) 
	   if(sound){
		 mz80int(0); 
		  for( i=nb_interlace; i--; ) {  // Loop optimized old //  for (i=0; i<nb_interlace; i++) { 
            if (z80_cycles>0) { 
               mz80exec(z80_cycles); 
               z80_cycles=0; 
               my_timer(); 
            } 
            z80_cycles += Z80_VBL_CYCLES/nb_interlace; 
         } 
	   }

		// One-vbl timeslice 
	   C68k_Exec(&C68K, m68k_cycles);
	   C68k_Set_IRQ(&C68K, 2);	
       /* update pd4990a */ 
       pd4990a_addretrace(); 
	   /* check the watchdog */ 
       if (watchdog_counter > 0) {       
       if (--watchdog_counter == 0) { 
       printf("reset caused by the watchdog\n"); 
       vid_clear(0,0,0);
       bfont_draw_str(vram_s+(10)*640+(10), 640,480, "reset caused by the watchdog"); 
       neogeo_reset(); 
       } 
       } 

if (++frameskip_counter >frameskip){
// Call display routine 
		if (neogeo_prio_mode)
			video_draw_screen2();
		else
			video_draw_screen1();
blit();   
frameblit ^= 1;
frameskip_counter = 0;	
}  
	/* check for memcard writes */
	if(memcard_write==0)
		 {
		   memcard_write--;
		   if (memcard_write > 0) {
	
		   vmu_lcd_save(); 
		   neogeo_save();
           vmu_lcd_update();
           
			   }
		}
      /* Update keys and Joystick */ 
      processEvents(); 
 // Check if there are pending commands for CDDA 
neogeo_cdda_check(); 
cdda_loop_check(); 
// Save current state and return to menu 
cdda_was_playing = cdda_playing; 
}
// Stop CDDA
if (cdda_playing)
cdda_stop();
return; 
}
//---------------------------------------------------------------------------- 
// This is a really dirty hack to make SAMURAI SPIRITS RPG work 
void   neogeo_prio_switch(void) 
{ 

   if (C68k_Get_DReg(&C68K, 7) == 0xFFFF)
      return; 
    
   if (C68k_Get_DReg(&C68K, 7) == 9 &&
       C68k_Get_AReg(&C68K, 3) == 0x10DED9 &&
      (C68k_Get_AReg(&C68K, 2) == 0x1081d0 ||
      (C68k_Get_AReg(&C68K, 2)&0xFFF000) == 0x102000)) {
      neogeo_prio_mode = 0; 
      return; 
   } 
    
   if (C68k_Get_DReg(&C68K, 7) == 8 &&
       C68k_Get_AReg(&C68K, 3) == 0x10DEC7 &&
      C68k_Get_AReg(&C68K, 2) == 0x102900) {
      neogeo_prio_mode = 0; 
      return; 
   } 
    
   if (C68k_Get_AReg(&C68K, 7) == 0x10F29C)
   { 
      if ((C68k_Get_DReg(&C68K, 4)&0x4010) == 0x4010)
      { 
         neogeo_prio_mode = 0; 
         return; 
      } 
       
      neogeo_prio_mode = 1; 
   } 
   else 
   { 
      if (C68k_Get_AReg(&C68K, 3) == 0x5140)
      { 
         neogeo_prio_mode = 1; 
         return; 
      } 

      if ( (C68k_Get_AReg(&C68K,3)&~0xF) == (C68k_Get_AReg(&C68K, 4)&~0xF) )
         neogeo_prio_mode = 1; 
      else 
         neogeo_prio_mode = 0; 
   } 


	 
} 

void   neogeo_exit(void) 
{ 
   puts("NEOGEO: Exit requested by software..."); 
  // exit(0); 
} 



//---------------------------------------------------------------------------- 
void   neogeo_quit(void) 
{ 
     // exit(0); 
} 

//---------------------------------------------------------------------------- 
void  neogeo_cdda_check(void) 
{ 
   int      Offset; 
    
   Offset = m68k_read_memory_32(0x10F6EA); 
   if (Offset < 0xE00000)   // Invalid addr 
      return; 

   Offset -= 0xE00000; 
   Offset >>= 1; 
  //  neogeo_do_cdda(mame_z80mem[Offset&0xFFFF], mame_z80mem[(Offset+1)&0xFFFF]);
  // neogeo_do_cdda(subcpu_memspace[Offset&0xFFFF], subcpu_memspace[(Offset+1)&0xFFFF]);
    neogeo_do_cdda(subcpu_memspace[Offset], subcpu_memspace[Offset+1]); 
} 

//---------------------------------------------------------------------------- 
void neogeo_cdda_control(void) 
{ 

   neogeo_do_cdda( (C68k_Get_DReg(&C68K, 0)>>8)&0xFF, C68k_Get_DReg(&C68K, 0)&0xFF );

} 


//---------------------------------------------------------------------------- 
void   neogeo_do_cdda( int command, int track_number_bcd) 
{ 
   int      track_number; 
   int      offset; 

   if ((command == 0)&&(track_number_bcd == 0)) 
      return; 

   m68k_write_memory_8(0x10F64B, track_number_bcd); 
   m68k_write_memory_8(0x10F6F8, track_number_bcd); 
   m68k_write_memory_8(0x10F6F7, command); 
   m68k_write_memory_8(0x10F6F6, command); 

   offset = m68k_read_memory_32(0x10F6EA); 

   if (offset) 
   { 
      offset -= 0xE00000; 
      offset >>= 1; 

      m68k_write_memory_8(0x10F678, 1); 
	//mame_z80mem[offset&0xFFFF] = 0;
		//	mame_z80mem[(offset+1)&0xFFFF] = 0;
      subcpu_memspace[offset&0xFFFF] = 0;
   subcpu_memspace[(offset+1)&0xFFFF] = 0;
   } 

   switch( command ) 
   { 
      case   0: 
      case   1: 
      case   5: 
      case   4: 
      case   3: 
      case   7: 
         track_number = ((track_number_bcd>>4)*10) + (track_number_bcd&0x0F); 
         if ((track_number == 0)&&(!cdda_playing)) 
         { 
    
            cdda_resume(); 
         } 
         else if ((track_number>1)&&(track_number<99)) 
         { 
         //   sound_mute(); 
            cdda_play(track_number); 
            cdda_autoloop = !(command&1); 
         } 
         break; 
      case   6: 
      case   2: 
         if (cdda_playing) 
         { 
    
            cdda_pause(); 
         } 
         break; 
   } 
} 
//---------------------------------------------------------------------------- 
void neogeo_read_gamename(void) 
{ 

   unsigned char   *Ptr; 
   int            temp; 

   Ptr = neogeo_prg_memory + m68k_read_memory_32(0x11A); 
   swab(Ptr, config_game_name, 80); 

   for(temp=0;temp<80;temp++) { 
      if (!isprint(config_game_name[temp])) { 
         config_game_name[temp]=0; 
         break; 
      } 
   } 
}





