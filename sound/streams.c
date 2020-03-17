/***************************************************************************

  streams.c

  Handle general purpose audio streams

***************************************************************************/
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
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include "../neocd.h"

extern int frame;


uint16_t play_buffer[BUFFER_LEN];
static Sint16 *tmp_sound_buf[16]=
{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
static uint16_t left_right_buffer[BUFFER_LEN*2];
static uint16_t *left_buffer=&left_right_buffer[0];
static uint16_t *right_buffer=&left_right_buffer[BUFFER_LEN];

int SamplePan[] = { 0, 255, 128, 0, 255 };


static int stream_joined_channels[MIXER_MAX_CHANNELS];

static int stream_vol[MIXER_MAX_CHANNELS];

struct {
    Sint16 *buffer;
    int param;
    void (*callback) (int param, Sint16 ** buffer, int length);
} stream[MIXER_MAX_CHANNELS];

void mixer_set_volume(int channel,int volume) {
    stream_vol[channel]=volume;
}

 __inline__  int streams_sh_start(void)
{
    int i;
    for (i = MIXER_MAX_CHANNELS; i--;) {
   // for (i = 0; i < MIXER_MAX_CHANNELS; i++) {
	stream_joined_channels[i] = 1;
	stream[i].buffer = 0;
    }
    return 0;
}

#define MAX_AUDIOVAL ((1<<(16-1))-1)
#define MIN_AUDIOVAL -(1<<(16-1))

static __inline__ void mixaudio(Sint16 *dst, const Sint16 *src, Uint32 len)
{
	register unsigned l=len;
	while ( l-- ) {
		register int dst_sample=(*src++)+(*dst);
		if (dst_sample>MAX_AUDIOVAL)
			dst_sample=MAX_AUDIOVAL;
		else if (dst_sample<MIN_AUDIOVAL)
			dst_sample=MIN_AUDIOVAL;
		*dst++=dst_sample;
	}
}

void streams_sh_stop(void)
{
    int i;
    for (i = MIXER_MAX_CHANNELS; i--;) {
   // for (i = 0; i < MIXER_MAX_CHANNELS; i++) {
	free(stream[i].buffer);
	stream[i].buffer = 0;
    }
}

void  __inline__ streamupdate(int len)
{
    /*static int current_pos;*/
    int channel;
    int buflen = len >> 2;

    if (buflen<=0)
	return;

    /* update all the output buffers */
    bzero(left_buffer, len);
    bzero(right_buffer, len);

    for (channel = 0; channel < MIXER_MAX_CHANNELS;
	 channel += stream_joined_channels[channel]) {
	if (stream[channel].buffer) {

	    if (stream_joined_channels[channel] > 1) {
		    {
		    	register int i;
		    	register Sint16 **buf=tmp_sound_buf;
		     	register int max=stream_joined_channels[channel];
		    	for (i = 0; i < max; i++)
				*buf++ = stream[channel + i].buffer;
		    }

		    (*stream[channel].callback) (stream[channel].param,
						 tmp_sound_buf, buflen);
	    }
	}
    }

    for (channel = 0; channel < MIXER_MAX_CHANNELS;
	 channel += stream_joined_channels[channel]) {

	if (stream[channel].buffer) {
	    register int i;
	    register uint16_t *bl=left_buffer;
	    register uint16_t *br=right_buffer;
	    register int max=stream_joined_channels[channel];
	    for (i = 0; i < max; i++) {

		if (SamplePan[channel + i] <= 128)
		    mixaudio((Sint16 *)bl, (Sint16 *)stream[channel + i].buffer, buflen);
		if (SamplePan[channel + i] >= 128)
		    mixaudio((Sint16 *)br, (Sint16 *)stream[channel + i].buffer, buflen);

	    }
	}
    }
#ifndef DREAMCAST
    SDL_LockAudio();
#endif
    {
    	register uint16_t *pl = play_buffer;
	register uint16_t *bl = left_buffer;
	register uint16_t *br = right_buffer;
	register int i;
	register int max = len >> 2;
    	for (i = 0; i < max; ++i) {
		*pl++ = *bl++;
		*pl++ = *br++;
    	}
    }
#ifndef DREAMCAST
    SDL_UnlockAudio();
#endif
}

int stream_init_multi(int channels, int param,
		      void (*callback) (int param, Sint16 ** buffer,
					int length))
{
    static int channel, i;

    stream_joined_channels[channel] = channels;
    for (i = channels; i--;) {
  //  for (i = 0; i < channels; i++) {

	if ((stream[channel + i].buffer =
	     malloc(sizeof(Sint16) * BUFFER_LEN)) == 0)
	    return -1;
    }

    stream[channel].param = param;
    stream[channel].callback = callback;
    channel += channels;
    return channel - channels;
}
