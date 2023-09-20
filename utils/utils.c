#include "lpc17xx.h"
#include "../GLCD/GLCD.h"
#include <stdio.h>
#include "utils.h"
#include "../timer/timer.h"
#include "../RIT/RIT.h"

uint8_t seconds = 0;
uint8_t hours = 0;
uint8_t minutes = 0;
uint8_t happiness = 4;
uint8_t satiety = 4;
uint8_t element_selected = 0;
uint8_t animation_active = 0;
uint8_t reset = 0;
uint8_t satiety_countdown = 10;
uint8_t happiness_countdown = 5;
uint8_t play_music = 0;
uint8_t sound_on = 0;
double volume_level = 0;
State nigiri_state  = Happy;

void Draw_Overlay_Happy(){
	LCD_Clear(White);
	LCD_DrawEmptyRectangle(1, 1, 238, 318, Black);
	LCD_DrawEmptyRectangle(119, 257, 120, 62, Black);
	LCD_DrawEmptyRectangle(1, 257, 119, 62, Black);
	GUI_Text(35, 65, (uint8_t *) "Happiness", Black, White);
	GUI_Text(145, 65, (uint8_t *) "Satiety", Black, White);
	GUI_Text(40, 280, (uint8_t *) "Snack", Black, White);
	GUI_Text(165, 280, (uint8_t *) "Meal", Black, White);

	return;
}

void Draw_Overlay_Angry(){

	LCD_Clear(White);
	LCD_DrawEmptyRectangle(1, 1, 238, 318, Black);
	LCD_DrawEmptyRectangle(1, 257, 238, 62, Red);	
	GUI_Text(100, 280, (uint8_t *) "Reset", Black, White);
	GUI_Text(35, 65, (uint8_t *) "Happiness", Black, White);
	GUI_Text(145, 65, (uint8_t *) "Satiety", Black, White);
	
	return;
}

void Draw_Image(uint8_t size_x, uint8_t size_y, uint16_t Xpos, uint16_t Ypos, const uint16_t * img) {

	uint16_t TempPixel;
	uint32_t i, j;
	i = 0;
	j = 0;
	
  do
  {
    TempPixel = *img++; 	
		if( i == size_x){
			j++;
			i = 0;
		}
		i++;
	LCD_SetPoint(Xpos + i,Ypos + j, TempPixel);
	}while(j < size_y);
	
	return;
};

void Draw_Battery(uint16_t X_start, uint16_t Y_start, uint8_t charge){
	LCD_DrawRectangle(X_start, Y_start, 29, 20, White);
	LCD_DrawEmptyRectangle(X_start, Y_start, 29, 20, Black);
	LCD_DrawRectangle(X_start + 29 + 1, Y_start + 3 , 4, 14, Black);
	
	switch(charge){
		
		case 0:			
			break;
		
		case 1:
			LCD_DrawRectangle(X_start + 2, Y_start + 2, 5, 16, Red);
			break;
		
		case 2:
			LCD_DrawRectangle(X_start + 2, Y_start + 2, 5, 16, Red);
			LCD_DrawRectangle(X_start + 2 + 5 + 1 + 1, Y_start + 2, 5, 16, Yellow);
			break;
		
		case 3:
			LCD_DrawRectangle(X_start + 2, Y_start + 2, 5, 16, Red);
			LCD_DrawRectangle(X_start + 2 + 5 + 1 + 1, Y_start + 2, 5, 16, Yellow);
			LCD_DrawRectangle(X_start + 2 + 5 + 2 + 5 + 1, Y_start + 2, 5, 16, Green);
			break;
		
		case 4:
			LCD_DrawRectangle(X_start + 2, Y_start + 2, 5, 16, Red);
			LCD_DrawRectangle(X_start + 2 + 5 + 1 + 1, Y_start + 2, 5, 16, Yellow);
			LCD_DrawRectangle(X_start + 2 + 5 + 2 + 5 + 1, Y_start + 2, 5, 16, Green);
			LCD_DrawRectangle(X_start + 2 + 5 + 2 + 5 + 2 + 5 + 1, Y_start + 2, 5, 16, Green);
			break;
		
		default:
			break;
	}
	
	return;
}


void Draw_Volume(uint8_t level){
	switch(level){
		case 0:
			Draw_Image(VOLUME_W, VOLUME_H, 5, 5, volume0);
			break;
		
		case 1:
			Draw_Image(VOLUME_W, VOLUME_H, 5, 5, volume1);
			break;

		case 2:
			Draw_Image(VOLUME_W, VOLUME_H, 5, 5, volume2);
			break;
		
		case 3:
			Draw_Image(VOLUME_W, VOLUME_H, 5, 5, volume3);
			break;
	}
}


void Draw_Age(uint8_t hours, uint8_t minutes, uint8_t seconds){
	char age_String[5] = "";
	const char * original ="Age: %02d:%02d:%02d ";
	sprintf(age_String, original, hours, minutes, seconds);
	GUI_Text(65, 15, (uint8_t *) age_String, Black, White);
	
	return;
}

void reset_nigiri(){
	happiness = 4;
	satiety = 4;
	seconds = 0;
	minutes = 0;
	hours = 0;
	animation_active = 0;
  satiety_countdown = 10;
  happiness_countdown = 5;

	nigiri_state = Happy;
	Draw_Overlay_Happy();
	Draw_Battery(45, 40, happiness);
  Draw_Battery(155, 40, satiety);
  Draw_Age(seconds, minutes, hours);
  Draw_Image(NIGIRI_W, NIGIRI_H, NIGIRI_START_X,NIGIRI_START_Y,Nigiri);
	
	reset_RIT();
	reset_timer(0);
	reset_timer(1);
	
	enable_RIT();
 	enable_timer(0);
	enable_timer(1);
	
	animation_active = 0;
	nigiri_state = Happy;
}

void make_angry(){
	disable_timer(0);
	Draw_Overlay_Angry();
	Draw_Battery(45, 40, happiness);
	Draw_Battery(155, 40, satiety);
	Draw_Image(ANGRY_NIGIRI_W, ANGRY_NIGIRI_H, NIGIRI_START_X,NIGIRI_START_Y,Angry_Nigiri);
	Draw_Age(hours, minutes, seconds);
	play_music = 1;
	animation_active = 1;
	nigiri_state = Angry;
}

void play_sound(uint16_t note, uint32_t duration)
{
	sound_on = 1;
	if(note != 0)
	{
		reset_timer(3);
		init_timer(3, note);
		enable_timer(3);
	}
	reset_timer(2);
	init_timer(2, duration);
	enable_timer(2);
}
