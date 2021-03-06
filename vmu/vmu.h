
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


/* XPM */
#define logo_vmu_width 48
#define logo_vmu_height 32
static unsigned char EFECERO_xpm[] = {
"++++++++...+++++++++++++++..............++++++++"
"+++++++.+++.+++++++++++++.++++++++++++++.+++++++"
"++++++.+..++.+++++++++++.++++++++++++++++.++++++"
"+++++.+....++.+++++++++.++++++++++++++++++.+++++"
"++++.+......++.+++++++.++++++++++++++++++++.++++"
"+++.+........++.+++++.++++++++++++.+++++++++.+++"
"++.+..........++.+++.++++++++++++...+++++++++.++"
"+.+............++.+.+++++++++++++..+++++++++++.+"
".+..............++.++++++++++++++++++++++++++++."
".+...............++++++++++++..++++++++++++++++."
".+..........++....++++++++++...++++++++++++++++."
".+.........++++...++++++++++...++++++++++++++++."
".+.........++++...+++++++++++.+++++++++++++++++."
".+..........++....++++++++++++++++++++++..+++++."
".+....++..........+++++++++++++++++++++....++++."
".+....++..........+++++++++++++++++++++....++++."
".+................+++++++++++++++++++++....++++."
".+.........................+++++++++++.....++++."
".+..........................++++++++.......++++."
".+..................+........+++++++......+++++."
".+..................+.........++++++.....++++++."
".++.................+..........++++++...+++++++."
"+.++...............++..........++++++++++++++++."
"++.++.............++..........+..+++++++++++++.+"
"+++.++...........++..........+.++.+++++++++++.++"
"++++.++......+++++..........+.++++.+++++++++.+++"
"+++++.++...................+.++++++.+++++++.++++"
"++++++.++.................+.++++++++.+++++.+++++"
"+++++++.++...............+.++++++++++.+++.++++++"
"++++++++.++.............+.++++++++++++.+.+++++++"
"+++++++++.++++++++++++++.++++++++++++++.++++++++"
"++++++++++..............++++++++++++++++++++++++"};

static unsigned char EFECERO_save[] = {
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"+++...++....++.+++.++...++.++.++...+++++++++++++"
"+++.++++.++.++..+..+++.+++..+.++.+++++++++++++++"
"++++.+++....+++...++++.+++.+..++.+.+++++++++++++"
"+++...++.++.++++.++++...++.++.++...++.+.+.++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"};

static unsigned char EFECERO_region[] = {
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++.++.++....++.+++++...+++++.++.++...+++++++++"
"++++.++.++.++.++.+++++.+..++++.++.++.++.++++++++"
"++++....++.++.++.+++++.++.++++.++.++...+++++++++"
"++++.++.++.++.++.+++++.++.++++.++.++.+++++++++++"
"++++.++.++....++....++...+++++....++.+++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++....++....++...+++++++++++++++++"
"++++++++++++++++.+++++.++.++.++.++++++++++++++++"
"++++++++++++++++...+++.++.++...+++++++++++++++++"
"++++++++++++++++.+++++.++.++.+.+++++++++++++++++"
"++++++++++++++++.+++++....++.++.++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++...+++....++....++.....++....++.+++.++++++++"
"++++.++.++.+++++.+++++++.++++.++.++..++.++++++++"
"++++...+++...+++....++++.++++.++.++.+.+.++++++++"
"++++.+.+++.+++++.++.++++.++++.++.++.++..++++++++"
"++++.++.++....++....++.....++....++.+++.++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++....++....++.+++++....++....++.....+++++++++"
"++++.+++++.+++++.+++++.+++++.+++++++.+++++++++++"
"+++++..+++...+++.+++++...+++.+++++++.+++++++++++"
"+++++++.++.+++++.+++++.+++++.+++++++.+++++++++++"
"++++....++....++....++....++....++++.+++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"};
