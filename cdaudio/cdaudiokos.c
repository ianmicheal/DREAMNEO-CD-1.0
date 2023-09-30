/**************************************
****   CDAUDIO.C  -  CD-DA Player  ****
**************************************/

//-- Include files -----------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include "kos.h"
#include "../neocd.h"
#include "cdaudio.h"


//-- Private Variables -------------------------------------------------------
static int			cdda_min_track;
static int			cdda_max_track;
static int			cdda_disk_length;
static int			cdda_track_end;
static int			cdda_loop_counter;
//static SDL_CD			*cdrom;
static CDROM_TOC	*cdrom;

//-- Public Variables --------------------------------------------------------
int			cdda_first_drive=0;
int			cdda_current_drive=0;
int			cdda_current_track=0;
int			cdda_current_frame=0;
int			cdda_playing=0;
int			cdda_autoloop=0;
int			cdda_volume=0;
int			cdda_disabled=0;

//-- Function Prototypes -----------------------------------------------------
int			cdda_init(void);
int			cdda_play(int);
void			cdda_stop(void);
void			cdda_resume(void);
void			cdda_shutdown(void);
void			cdda_loop_check(void);
int 			cdda_get_disk_info(void);




//----------------------------------------------------------------------------
int	cdda_init(void)
{
	
	cdda_min_track = cdda_max_track = 0;
	cdda_current_track = 0;
	cdda_playing = 0;
	cdda_loop_counter = 0;


	cdrom_init();
	cdrom_read_toc(cdrom, 0);

		cdda_disabled=0;
		printf("CD Audio OK!\n");


	cdda_get_disk_info();
	return	1;
}

//----------------------------------------------------------------------------
int cdda_get_disk_info(void)
{
    if(cdda_disabled) return 1;
	int disc_status, disc_type;
	cdrom_get_status(&disc_status, &disc_type);    
    if( disc_status !=CD_STATUS_OPEN ) {
        cdda_min_track = 1;
        cdda_max_track = TOC_TRACK(cdrom->last);//cdrom->numtracks;
        cdda_disk_length = cdda_max_track;//cdrom->numtracks;
        #ifdef DBUGOUT
        printf("cdda_max_track = %d , cdda_disk_length =%d\n",cdda_max_track,cdda_disk_length);
        #endif
        return 1;
    }
    else
    {
        #ifdef DBUGOUT
        printf("Error: No Disc in drive\n");
        #endif
        cdda_disabled=1;
        return 1;
    }
}


//----------------------------------------------------------------------------
int cdda_play(int track)
{
    if(cdda_disabled) return 1;
    
    if(cdda_playing && cdda_current_track==track) return 1;
	int disc_status, disc_type;
	cdrom_get_status(&disc_status, &disc_type);    
//    if( CD_INDRIVE(SDL_CDStatus(cdrom)) ) {
    if( disc_status !=CD_STATUS_OPEN ) {
    	//SDL_CDPlayTracks(cdrom, track-1, 0, 1, 0);
    	
    	printf("track= %d\n",track);
    	
    	cdrom_cdda_play(track,track+1,0,CDDA_TRACKS);
    	cdda_current_track = track;
    	cdda_loop_counter=0;
    	cdda_track_end=TOC_LBA(cdrom->entry[track - 1]);//(cdrom->track[track-1].length*60)/CD_FPS;//Length in 1/60s of second
    	cdda_playing = 1;
    	return 1;
    } 
    else
    { 
        cdda_disabled = 1;
        return 1;
    }
}

//----------------------------------------------------------------------------
void	cdda_pause(void)
{
	if(cdda_disabled) return;
	//SDL_CDPause(cdrom);
	cdrom_cdda_pause();
	cdda_playing = 0;
}


void	cdda_stop(void)
{
	if(cdda_disabled) return;
	//SDL_CDStop(cdrom);
	cdrom_spin_down();
	cdda_playing = 0;
}

//----------------------------------------------------------------------------
void	cdda_resume(void)
{
	if(cdda_disabled || cdda_playing) return;
	//SDL_CDResume(cdrom);	
	cdrom_cdda_resume();
	cdda_playing = 1;
}

//----------------------------------------------------------------------------
void	cdda_shutdown(void)
{
	cdrom_spin_down();
	if(cdda_disabled) return;
	cdrom_shutdown();
}

//----------------------------------------------------------------------------
void	cdda_loop_check(void)
{
	if(cdda_disabled) return;
	if (cdda_playing==1) {
		cdda_loop_counter++;
		if (cdda_loop_counter>=cdda_track_end) {
			if (cdda_autoloop)
				cdda_play(cdda_current_track);
			else
				cdda_stop();
		}
	}
}

