/**************************************
****   INPUT.C  -  Input devices   ****
**************************************/
/*-- Include Files 
---------------------------------------------------------*/
#include "../neocd.h"
#include <kos.h>

/*--------------------------------------------------------------------------*/
#define P1UP     0x00000001
#define P1DOWN   0x00000002
#define P1LEFT   0x00000004
#define P1RIGHT  0x00000008
#define P1A      0x00000010
#define P1B      0x00000020
#define P1C      0x00000040
#define P1D      0x00000080

#define P2UP     0x00000100
#define P2DOWN   0x00000200
#define P2LEFT   0x00000400
#define P2RIGHT  0x00000800
#define P2A      0x00001000
#define P2B      0x00002000
#define P2C      0x00004000
#define P2D      0x00008000

#define P1START  0x00010000
#define P1SEL    0x00020000
#define P2START  0x00040000
#define P2SEL    0x00080000

void processall1(void);
void processall2(void);

cont_state_t* state[2]__attribute__((aligned(32))); 
maple_device_t* dev[2]__attribute__((aligned(32)));
u32 keys   =~0;

/*--------------------------------------------------------------------------*/
  inline void processEvents(void) {
		keys = 0xffffffff;

		if(dev[0] != NULL)
			processall1();
		else
			dev[0] = maple_enum_dev (0,0);

		if(dev[1] != NULL)
			processall2();

		else
			dev[1] = maple_enum_dev (1,0);
}

  inline void input_init(void){
  dev[0] = maple_enum_dev (0, 0);
  dev[1] = maple_enum_dev (1,0);
}
void input_shutdown(void){}
/*--------------------------------------------------------------------------*/
unsigned char read_player1(void) {
    return keys&0xff;
}

/*--------------------------------------------------------------------------*/
unsigned char read_player2(void) {
    return (keys>>8)&0xff;
}

/*--------------------------------------------------------------------------*/
unsigned char read_pl12_startsel(void) {
    return (keys>>16)&0x0f;
}


 void processall1(void){
int statenum = 0;
		state[statenum] = (cont_state_t*) maple_dev_status (dev[statenum]);

		if(state[statenum]->buttons & CONT_A)
			keys -= P1A;
		if(state[statenum]->buttons & CONT_B)
			keys -= P1B;
		if(state[statenum]->buttons & CONT_X)
			keys -= P1C;
		if(state[statenum]->buttons & CONT_Y)
			keys -= P1D;
		
		
		if(((state[statenum]->buttons & CONT_DPAD_UP) && (state[statenum]->buttons & CONT_DPAD_LEFT))|| ( (state[statenum]->joyy <=-64) && (state[statenum]->joyx <=-64))){
			keys -= (P1UP+P1LEFT);
		}
		else if(((state[statenum]->buttons & CONT_DPAD_UP) && (state[statenum]->buttons & CONT_DPAD_RIGHT))|| ( (state[statenum]->joyy <=-64) && (state[statenum]->joyx >=64))){
			keys -= (P1UP+P1RIGHT);
		}
		else if(((state[statenum]->buttons & CONT_DPAD_DOWN) && (state[statenum]->buttons & CONT_DPAD_LEFT))||( (state[statenum]->joyy >=64) && (state[statenum]->joyx <=-64))){
			keys -= (P1DOWN+P1LEFT);
		}
		else if(((state[statenum]->buttons & CONT_DPAD_DOWN) && (state[statenum]->buttons & CONT_DPAD_RIGHT))||( (state[statenum]->joyy >=64) && (state[statenum]->joyx >=64))){
			keys -= (P1DOWN+P1RIGHT);
		}
		else if((state[statenum]->buttons & CONT_DPAD_DOWN)||(state[statenum]->joyy >=64)){
			keys -= P1DOWN;
		}
		else if((state[statenum]->buttons & CONT_DPAD_UP)||(state[statenum]->joyy <=-64)){
			keys -= P1UP;
		}
		else if((state[statenum]->buttons & CONT_DPAD_RIGHT)||(state[statenum]->joyx >=64)){
			keys -= P1RIGHT;
		}
		else if((state[statenum]->buttons & CONT_DPAD_LEFT)||(state[statenum]->joyx <=-64)){
			keys -= P1LEFT;
		}
		
		if(state[statenum]->buttons &CONT_START)
			keys -= P1START;
		if(state[statenum]->rtrig > 96)
			keys -= P1SEL;
		if(state[statenum]->ltrig>110){
			sound_toggle();
			usleep(100);
		}

			if( (state[statenum]->rtrig > 96) && (state[statenum]->ltrig>110))
		neogeo_shutdown();
			usleep(100);
}

 void processall2(void){
int statenum = 1;
		state[statenum] = (cont_state_t*) maple_dev_status (dev[statenum]);

		if(state[statenum]->buttons & CONT_A)
			keys -= P2A;
		if(state[statenum]->buttons & CONT_B)
			keys -= P2B;
		if(state[statenum]->buttons & CONT_X)
			keys -= P2C;
		if(state[statenum]->buttons & CONT_Y)
			keys -= P2D;
		
		
		if(((state[statenum]->buttons & CONT_DPAD_UP) && (state[statenum]->buttons & CONT_DPAD_LEFT))|| ( (state[statenum]->joyy <=-64) && (state[statenum]->joyx <=-64))){
			keys -= (P2UP+P2LEFT);
		}
		else if(((state[statenum]->buttons & CONT_DPAD_UP) && (state[statenum]->buttons & CONT_DPAD_RIGHT))|| ( (state[statenum]->joyy <=-64) && (state[statenum]->joyx >=64))){
			keys -= (P2UP+P2RIGHT);
		}
		else if(((state[statenum]->buttons & CONT_DPAD_DOWN) && (state[statenum]->buttons & CONT_DPAD_LEFT))||( (state[statenum]->joyy >=64) && (state[statenum]->joyx <=-64))){
			keys -= (P2DOWN+P2LEFT);
		}
		else if(((state[statenum]->buttons & CONT_DPAD_DOWN) && (state[statenum]->buttons & CONT_DPAD_RIGHT))||( (state[statenum]->joyy >=64) && (state[statenum]->joyx >=64))){
			keys -= (P2DOWN+P2RIGHT);
		}
		else if((state[statenum]->buttons & CONT_DPAD_DOWN)||(state[statenum]->joyy >=64)){
			keys -= P2DOWN;
		}
		else if((state[statenum]->buttons & CONT_DPAD_UP)||(state[statenum]->joyy <=-64)){
			keys -= P2UP;
		}
		else if((state[statenum]->buttons & CONT_DPAD_RIGHT)||(state[statenum]->joyx >=64)){
			keys -= P2RIGHT;
		}
		else if((state[statenum]->buttons & CONT_DPAD_LEFT)||(state[statenum]->joyx <=-64)){
			keys -= P2LEFT;
		}
		
		if(state[statenum]->buttons &CONT_START)
			keys -= P2START;
		if(state[statenum]->rtrig > 96)
			keys -= P2SEL;
		if(state[statenum]->ltrig>110){
			sound_toggle();
			usleep(100);
		}

		if( (state[statenum]->rtrig > 96) && (state[statenum]->ltrig>110))
		neogeo_shutdown();
			usleep(100);
}

