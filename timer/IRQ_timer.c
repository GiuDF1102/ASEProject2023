/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../utils/utils.h"
#include "../RIT/RIT.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

uint16_t SinTable1[45] =   
{
	512, 583, 653, 720, 783, 841, 892, 936, 972, 998, 1016, 
	1023, 1021, 1008, 986, 955, 915, 867, 812, 752, 687,
	618, 547, 476, 405, 337, 271, 211, 156, 108, 68, 37,
	15, 2, 0, 7, 25, 51, 87, 131, 182, 241, 303, 371, 441,
};

uint16_t SinTable2[45] =                       
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};
void TIMER0_IRQHandler (void)
{
	seconds++;																					//update seconds counter
	
	if(!animation_active) {
		satiety_countdown--;
		happiness_countdown--;
	}							

	if(seconds == 60) { 																
		minutes++;																				
		seconds = 0;																			
	}
	
	if(minutes == 60) {
		hours++;
		minutes = 0;
	}
	
	if(hours == 99) {
		hours = 0;
		minutes = 0;
		seconds = 0;
	}
	
	Draw_Age(hours, minutes, seconds);		

	if(!happiness_countdown){														//Every 5 seconds
		if((happiness > 1) & !animation_active){
			happiness_countdown = 5;//if happiness is not zero and if animation is not active
			Draw_Battery(45, 40, --happiness);							//decrement happiness
		}else if(happiness == 1){
			happiness = 0; //else only if happiness is zero then make tamagotchi angry
			make_angry();
			LPC_TIM0->IR = 1;			/* clear interrupt flag */
			return;
		}
	}
	
	if(!satiety_countdown) {															//Every 10 seconds
		if((satiety > 1) & !animation_active){
			satiety_countdown	= 10;		//if satiety is not zero and if animation is not active
			Draw_Battery(155, 40, --satiety);									//decrement satiety
		}else if(satiety == 1){															//else only if satiety is zero then make tamagotchi angry
			Draw_Overlay_Angry();
			satiety = 0;
			make_angry();
			LPC_TIM0->IR = 1;			/* clear interrupt flag */
			return;
		}
	}
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}



void TIMER1_IRQHandler (void)
{
	static uint8_t anim_state = 1;
	static uint8_t go_back = 0;
	static uint8_t angry_on = 0;
	static uint8_t count_cuddle_anim = 0;
	
	switch(nigiri_state) {
		
		case Happy:
			 if(seconds%2 & !animation_active) {
				LCD_DrawRectangle(NIGIRI_START_X, NIGIRI_START_Y, NIGIRI_W, NIGIRI_H, White);
			  Draw_Image(NIGIRI_W, NIGIRI_H, NIGIRI_START_X,NIGIRI_START_Y+1,Nigiri);	
			} else if(!(seconds%2) & !animation_active){				
				LCD_DrawRectangle(NIGIRI_START_X, NIGIRI_START_Y+1, NIGIRI_W, NIGIRI_H, White);
			  Draw_Image(NIGIRI_W, NIGIRI_H, NIGIRI_START_X,NIGIRI_START_Y,Nigiri);	
			}
			break;
		
		//EATING ANIMATION
		case Eating:
			if(!go_back){ //going towards food/snack			
				if(anim_state == 1){
					LCD_DrawRectangle(NIGIRI_START_X, NIGIRI_START_Y, LOVING_NIGIRI_W, LOVING_NIGIRI_H+1, White);
			    Draw_Image(LOVING_NIGIRI_W, LOVING_NIGIRI_H, NIGIRI_START_X-10,NIGIRI_START_Y,Loving_Nigiri);
					anim_state = 2;
				} else if(anim_state == 2){
					LCD_DrawRectangle(NIGIRI_START_X-10, NIGIRI_START_Y, LOVING_NIGIRI_W, LOVING_NIGIRI_H+1, White);
					Draw_Image(LOVING_NIGIRI_W, LOVING_NIGIRI_H, NIGIRI_START_X-10-10,NIGIRI_START_Y,Loving_Nigiri);
					anim_state = 3;
				} else if(anim_state == 3){
					LCD_DrawRectangle(NIGIRI_START_X-10-10, NIGIRI_START_Y, LOVING_NIGIRI_W, LOVING_NIGIRI_H+1, White);			
					Draw_Image(LOVING_NIGIRI_W, LOVING_NIGIRI_H, NIGIRI_START_X-10-10-10,NIGIRI_START_Y,Loving_Nigiri);
					anim_state = 4;
				} else if(anim_state == 4){
					Draw_Image(HEART_W, HEART_H, NIGIRI_START_X-10-10-10-HEART_W, NIGIRI_START_Y-HEART_H, heart);
					if(element_selected == 1) {
						LCD_DrawRectangle(RICEGRAIN_START_X, RICEGRAIN_START_Y, RICEGRAIN_W, RICEGRAIN_H, White);
						happiness_countdown = HAPPINESS_COUNTDOWN_FULL;
						if(happiness < 4) {
							happiness_countdown = 5;
							happiness++;
							Draw_Battery(45, 40, happiness);
						}
					} else if(element_selected == 2) {
						LCD_DrawRectangle(RICEBOWL_START_X, RICEBOWL_START_Y, RICEBOWL_W, RICEBOWL_H, White);
						satiety_countdown = SATIETY_COUNTDOWN_FULL;
						if(satiety < 4) {
							satiety_countdown = 10;
							satiety++;
						  Draw_Battery(155, 40, satiety);
						}
					}
					anim_state = 3;
					go_back = 1;
					play_music = 1;
				}
			} else { //going back
				if(anim_state == 3){
					LCD_DrawRectangle(NIGIRI_START_X-10-10-10-HEART_W, NIGIRI_START_Y-HEART_H, HEART_W, HEART_H, White);
					LCD_DrawRectangle(NIGIRI_START_X-10-10-10, NIGIRI_START_Y, LOVING_NIGIRI_W, LOVING_NIGIRI_H+1, White);
			    Draw_Image(LOVING_NIGIRI_W, LOVING_NIGIRI_H, NIGIRI_START_X-10-10,NIGIRI_START_Y,Loving_Nigiri);
					anim_state = 2;
				} else if(anim_state == 2){
					LCD_DrawRectangle(NIGIRI_START_X-10-10, NIGIRI_START_Y, LOVING_NIGIRI_W, LOVING_NIGIRI_H+1, White);
					Draw_Image(LOVING_NIGIRI_W, LOVING_NIGIRI_H, NIGIRI_START_X-10,NIGIRI_START_Y,Loving_Nigiri);
					anim_state = 1;
				} else if(anim_state == 1){
					LCD_DrawRectangle(NIGIRI_START_X-10, NIGIRI_START_Y, LOVING_NIGIRI_W, LOVING_NIGIRI_H+1, White);			
					Draw_Image(NIGIRI_W, NIGIRI_H, NIGIRI_START_X,NIGIRI_START_Y,Nigiri);
					go_back = 0;

					enable_timer(0);
					enable_RIT();
					animation_active = 0;
					nigiri_state = Happy;
				}
			}
			break;
		
		//ANGRY ANIMATION
		case Angry:
			if(!angry_on){
				Draw_Image(ANGRY_SIGN_W, ANGRY_SIGN_H, NIGIRI_START_X-ANGRY_SIGN_W, NIGIRI_START_Y-ANGRY_SIGN_H, angry_sign);
				angry_on = 1;
			}else{
				LCD_DrawRectangle(NIGIRI_START_X-ANGRY_SIGN_W, NIGIRI_START_Y-ANGRY_SIGN_H, ANGRY_SIGN_W, ANGRY_SIGN_H, White);
				angry_on = 0;
			}
			
			break;
			
		//CUDDLE ANIMATION
		case Cuddle:
			if(count_cuddle_anim <=3) {
				if(!(count_cuddle_anim%2)){ //0 2
					Draw_Image(HEART_W, HEART_H, NIGIRI_START_X-HEART_W, NIGIRI_START_Y-HEART_H, heart);
				} else { //1 3
					LCD_DrawRectangle(NIGIRI_START_X-HEART_W, NIGIRI_START_Y-HEART_H, HEART_W, HEART_H, White);
				}
			count_cuddle_anim++;
			} else { //4
					happiness_countdown = HAPPINESS_COUNTDOWN_FULL;
					if(happiness < 4) {
						happiness++;
						Draw_Battery(45, 40, happiness);
					}

					enable_timer(2);
					count_cuddle_anim = 0;
					animation_active = 0;
					nigiri_state = Happy;
					
			}
			
			break;

	}
	
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void)
{	
	sound_on = 0;
  disable_timer(3);
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER3_IRQHandler (void)
{
	static int ticks=0;
	/* DAC management */	
	static int currentValue;
	
	currentValue = SinTable2[ticks];
	currentValue -= 410;
	currentValue /= 1;
	currentValue += 410;
	LPC_DAC->DACR = (uint32_t)(volume_level*currentValue)<<6;
	ticks++;
//	LPC_DAC->DACR = (((uint32_t)(volume_level*SinTable[ticks]))<<6)/2;

	if(ticks==45) ticks=0;
	
  LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
