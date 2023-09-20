/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "adc.h"
#include "../utils/utils.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

void ADC_IRQHandler(void) {
  	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
  if(AD_current != AD_last){
			if(AD_current <= 0x0050) {
				Draw_Volume(0);
				volume_level = 0;
			}else {
				if((AD_current*3/0xFFF) == 0) {
					volume_level = 0.4;
					Draw_Volume(1);
			} else if((AD_current*3/0xFFF) == 1) {
					volume_level = 0.7;
					Draw_Volume(2);
			} else if((AD_current*3/0xFFF) == 2) {
					volume_level = 1;
					Draw_Volume(3);
			AD_last = AD_current;
			}
		}
	}		
}
