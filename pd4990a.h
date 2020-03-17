/*
 *	Header file for the PD4990A Serial I/O calendar & clock.
 */
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

#ifndef _PD4990A_H
#define _PD4990A_H

struct pd4990a_s
{
	int seconds;
	int minutes;
	int hours;
	int days;
	int month;
	int year;
	int weekday;
};

extern struct pd4990a_s pd4990a;

#ifdef USE_PD4990A
void pd4990a_addretrace (void);
void pd4990a_init(void);
int pd4990a_testbit_r (void);
int pd4990a_databit_r (void);
void pd4990a_control_w (unsigned,unsigned);
extern void pd4990a_control_16_w (int,int);
void pd4990a_increment_day(void);
void pd4990a_increment_month(void);

#else

#define pd4990a_init()
#define pd4990a_testbit_r() (0)
#define pd4990a_databit_r() (0)
#define pd4990a_addretrace() 

#endif

#endif /* _PD4990A_H */

