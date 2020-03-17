/*  gngeo a neogeo emulator
 *  Copyright (C) 2001 Peponas Mathieu
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
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


#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "../neocd.h"
#include "streams.h"

int sound=1;
SDL_AudioSpec desired;

#include "sound.h"
#define MUSIC_VOLUME 56
#define CPU_FPS 60
//#define BUFFER_LEN 16384
//#define BUFFER_LEN 8192
//#define BUFFER_LEN 32768 
//#define BUFFER_LEN 131072 
extern uint16_t play_buffer[BUFFER_LEN/2];
#define NB_SAMPLES 256 /* FAST resolution */
//#define NB_SAMPLES 512 /* BEST resolution */
//#define NB_SAMPLES 1024 /* DEFAULT resolution */
//#define NB_SAMPLES 2048 /* MAX resolution */
void update_sdl_stream(void *userdata, Uint8 * stream, int len)
{
    extern uint16_t play_buffer[];

    	streamupdate(len);

    	memcpy_16bit(stream, (Uint8 *) play_buffer, len);

//	SDL_DC_SetSoundBuffer(play_buffer);


}


int init_sdl_audio(void)
{

    if(SDL_InitSubSystem(SDL_INIT_AUDIO)) {
    	printf("Couldn't Start Sound.\n");
    }
    nb_interlace = 256;
    desired.freq = SAMPLE_RATE;
    desired.samples = NB_SAMPLES;
    

    
#ifdef WORDS_BIGENDIAN
    desired.format = AUDIO_S16MSB;
#else	/* */
    desired.format = AUDIO_S16;
#endif	/* */
    desired.channels = 2;
    desired.callback = update_sdl_stream;
    desired.userdata = NULL;
    SDL_OpenAudio(&desired, NULL);
    
    //init rest of audio
    streams_sh_start();
	YM2610_sh_start();
	SDL_PauseAudio(1);
    
    return 1;
}

void sound_toggle(void) {
//SDL_PauseAudio(1);
	SDL_PauseAudio(sound);
	sound^=1;
}

void sound_enable(void) {
//SDL_PauseAudio(1);
	SDL_PauseAudio(0);
	sound=1;
}

void sound_disable(void) {
	SDL_PauseAudio(1);
	sound=0;
}

void sound_shutdown(void) {
    SDL_PauseAudio(1);
	printf("Sound shutdown 1");
//    SDL_CloseAudio();
    printf("Sound shutdown  2");
    streams_sh_stop();
	printf("Sound shutdown  3");
    YM2610_sh_stop();
	printf("Sound shutdown  4");
}



