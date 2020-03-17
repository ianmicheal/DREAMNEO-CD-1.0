/*******************************************
**** VIDEO.H - Video Hardware Emulation ****
****            Header File     )        ****
*******************************************/

#ifndef	VIDEO_H
#define VIDEO_H

/*-- Defines ---------------------------------------------------------------*/
#define VIDEO_TEXT	0
#define VIDEO_NORMAL	1
#define	VIDEO_SCANLINES	2

/*-- Global Variables ------------------------------------------------------*/
extern char		*video_vidram;
extern unsigned short	*video_paletteram_ng;
extern unsigned short	video_palette_bank0_ng[4096];
extern unsigned short	video_palette_bank1_ng[4096];
extern unsigned short	*video_paletteram_pc;
extern unsigned short	video_palette_bank0_pc[4096];
extern unsigned short	video_palette_bank1_pc[4096];
extern short		video_modulo;
extern unsigned short	video_pointer;
extern unsigned short	*video_paletteram;
extern unsigned short	*video_paletteram_pc;
extern unsigned short	video_palette_bank0[4096];
extern unsigned short	video_palette_bank1[4096];
extern unsigned short	*video_line_ptr[225];
extern unsigned char	video_fix_usage[4096];
extern unsigned char	rom_fix_usage[4096];
extern unsigned char	video_spr_usage[32768];
extern unsigned char	rom_spr_usage[32768];
extern unsigned short	video_color_lut[32768];
extern int neogeo_prio_mode; 

extern int		video_mode;

extern unsigned int	neogeo_frame_counter;
extern unsigned int	neogeo_frame_counter_speed;

/*-- video.c functions ----------------------------------------------------*/
int  video_init(void);
void video_shutdown(void); 
void video_draw_screen1(void);
void video_draw_screen2(void);
void video_draw_spr(unsigned int code, unsigned int color, int flipx,
			int flipy, int sx, int sy, int zx, int zy);
void video_setup(void);

/*-- draw_fix.c functions -------------------------------------------------*/
void video_draw_fix(void);
 //void draw_fix2(void);
//void fixputs(u16 x, u16 y, const char * string);

void blit(void);

#endif /* VIDEO_H */

