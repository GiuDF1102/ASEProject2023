/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "utils/utils.h"
#include "joystick/joystick.h"
#include "TouchPanel/TouchPanel.h"
#include "adc/adc.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
	//INIT		
  SystemInit();

  LCD_Initialization();
	joystick_init();
	TP_Init();
	TouchPanel_Calibrate();
	init_RIT(0x004C4B40);											//50 msec
	init_timer(0, 0x0017D7840); 						  //1s -> 0x17D7840
	init_timer(1, 0x000BEBC20);								//amination timer initialization = 500ms

	//TAMAGOTCHI INIT
	Draw_Overlay_Happy();
	Draw_Battery(45, 40, happiness);
  Draw_Battery(155, 40, satiety);
  Draw_Age(seconds, minutes, hours);
  Draw_Image(NIGIRI_W, NIGIRI_H, NIGIRI_START_X,NIGIRI_START_Y,Nigiri);
	
	enable_RIT();
 	enable_timer(0);
	enable_timer(1);
	init_timer(3, 0);
	init_timer(2,0);
	
	ADC_init();	
	ADC_start_conversion();	

	LPC_SC->PCON |= 0x1;
	LPC_SC->PCON &= ~(0x2);	
	
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
  while (1)	
  {
		__ASM("wfi");

  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
