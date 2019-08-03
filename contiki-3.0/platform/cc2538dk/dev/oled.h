#ifndef __OLED_H
#define __OLED_H	

void OLED_WrDat1(unsigned char dat);	    
void OLED_WrCmd1(unsigned char cmd);
void OLED_Setpos1(unsigned char y,unsigned char column);
void OLED_Fill1(unsigned char dd);	    
void OLED_Display_On1(void);
void OLED_Display_Off1(void);
							   		    
void OLED_Init1(void);
void OLED_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
#endif  
	 



