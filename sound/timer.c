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

#include <stdlib.h>
#include "timer.h"
#include <stdio.h>

//double neogeo_timer_count;
double  timer_counta=0.0;
static double timer_inc=1.0;
static timer_struct *timer_list;
static int initted=0;
#define MAX_TIMER 3
timer_struct timers[MAX_TIMER];
int nb_interlace=256;
//int nb_timer=0;

 double timer_get_time(void) {
    return  timer_counta;
}

timer_struct *insert_timer(double duration, int param, void (*func) (int))
{
    int i;
    for (i = 0; i < MAX_TIMER; i++) {
        
	if (timers[i].del_it) {
	    timers[i].time =  timer_counta + duration;
	    timers[i].param = param;
	    timers[i].func = func;
	    //printf("Insert_timer %d duration=%f param=%d\n",i,timers[i].time,timers[i].param);
	    timers[i].del_it = 0;
	    return &timers[i];
	}
    }
    printf("YM2610: No timer free!\n");
    return NULL;		/* No timer free */
}
void free_all_timer(void) {
	register int i;
	register timer_struct *t=(timer_struct *)&timers[0];
	for (i = 0; i < MAX_TIMER; i++, t++)
		t->del_it=1;
	initted=1;
}

void del_timer(timer_struct * ts)
{
    ts->del_it = 1;
}
void timer_callback_2610(int param);

//void del_timer(timer_struct * ts)
//{
//    ts->del_it = 1;
//}

//static float inc;
/*
void my_timer(void)
{
    static int init = 1;
    int i;

    if (init) {
//	timer_init_save_state();
	init = 0;
	if (0) {
	    timer_inc = (double) (0.01066) / nb_interlace;
			  //(conf.sound ? (double) nb_interlace : 1.0);
	} else {
	    timer_inc =
		(double) (0.01066) / nb_interlace;
	   // (conf.sound ? (double) nb_interlace : 1.0);
	}
	for (i = 0; i < MAX_TIMER; i++)
	    timers[i].del_it = 1;
    }
       timer_counta += timer_inc;		/* 16ms/20ms par frame */
   // neogeo_timer_count += inc;		/* 16ms par frame */
/*
    for (i = 0; i < MAX_TIMER; i++) {
	if ( timer_counta >= timers[i].time && timers[i].del_it == 0) {
	   // printf("Timer_expire %d duration=%f param=%d\n",i,timers[i].time,timers[i].param);
	    if (timers[i].func) timers[i].func(timers[i].param);
	    timers[i].del_it = 1;
	}
    }
}

*/
void my_timer(void)
{
    register int i;
    register timer_struct *t=(timer_struct *)&timers[0];
    timer_counta  +=   timer_inc;		/* 16ms par frame */
    timer_inc =(double) (0.014) / nb_interlace;
     double cnt=timer_counta ;
    for (i = 0; i < MAX_TIMER; i++,t++) {
	if (cnt >= t->time && t->del_it == 0) {
	    timer_callback_2610(t->param);
	    t->del_it = 1;
	}
    }
}
