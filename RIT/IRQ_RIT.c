/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../utils/utils.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include "../TouchPanel/TouchPanel.h"
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void RIT_IRQHandler (void)
{	
	static int left = 0;
	static int right = 0;
	static int select = 0;
	static int note = 0;
	
	/* ADC management */
	ADC_start_conversion();	
	
		if(play_music) {
			switch(nigiri_state){
				
				case Angry:
					if( (!sound_on) & (note < ANGRY_SONG_L)){
						play_sound(angry_melody[note], quarter);
						note++;
					} else if(note >=  ANGRY_SONG_L){
						note = 0;
						play_music = 0;
					}
					break;
				
				case Eating:
					if((!sound_on) & (note < HAPPY_SONG_L)){
						play_sound(happy_melody[note], quarter);
						note++;
					} else if(note >=  HAPPY_SONG_L){
						note = 0;
						play_music = 0;
					}
					break;
				
				case Cuddle:
					if((!sound_on) & (note < CUDDLE_SONG_L)){
						play_sound(cuddle_melody[note], quarter);
						note++;
					} else if(note >=  CUDDLE_SONG_L){
						note = 0;
						play_music = 0;
					}
					break;
					
				case Happy:
					break;
				
					
			}
		}
	//touchpanel
	if(!(animation_active) & getDisplayPoint(&display, Read_Ads7846(), &matrix )){
		if(display.y > NIGIRI_START_Y & display.y < (NIGIRI_START_Y + NIGIRI_H)){
			if(display.x > NIGIRI_START_X & display.x < (NIGIRI_START_X + NIGIRI_W)) {
				enable_timer(1);
				nigiri_state = Cuddle;
				play_music = 1;
				animation_active = 1;
			}
		}
	}
	else{
		//do nothing if touch returns values out of bounds
	}
	
	if((nigiri_state == Happy) & ((LPC_GPIO1->FIOPIN & (1<<27)) == 0)){	
		/* Joytick LEFT pressed */
		left++;
		switch(left){
			case 1:
					LCD_DrawEmptyRectangle(119, 257, 120, 62, Black);	
					LCD_DrawEmptyRectangle(1, 257, 119, 62, Red);
					element_selected = 1;
			//left ACTION
				break;
			default:
				break;
		}
	} else{
			left=0;
	}	
	
	if((nigiri_state == Happy) & ((LPC_GPIO1->FIOPIN & (1<<28)) == 0)){	
		/* Joytick RIGHT pressed */
		right++;
		switch(right){
			case 1:
					LCD_DrawEmptyRectangle(1, 257, 119, 62, Black);
					LCD_DrawEmptyRectangle(119, 257, 120, 62, Red);	
					element_selected = 2;
			//right ACTION
				break;
			default:
				break;
		}
	} else{
			right=0;
	}	
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick SELECT pressed */
		select++;
		play_sound(b5, half_quarter);
		switch(select){
			case 1:
				if((animation_active == 0) & (element_selected == 1)) {
					Draw_Image(RICEGRAIN_W, RICEGRAIN_H, RICEGRAIN_START_X, RICEGRAIN_START_Y, ricegrain); //draw snack
					enable_timer(1);	//start animation
					animation_active = 1;
					disable_timer(0);

					nigiri_state = Eating;
				} else if((animation_active == 0) & (element_selected == 2)) {
					animation_active = 1;
					Draw_Image(RICEBOWL_W, RICEBOWL_H, RICEBOWL_START_X, RICEBOWL_START_Y, ricebowl);
					enable_timer(1);
					disable_timer(0);

					nigiri_state = Eating;
				} else if (nigiri_state == Angry){
					disable_RIT();
					disable_timer(1);
					reset_nigiri();
					return;
				};
				break;
			default:
				break;
		}

		enable_timer(1);


	} else{
			select=0;
	}	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
