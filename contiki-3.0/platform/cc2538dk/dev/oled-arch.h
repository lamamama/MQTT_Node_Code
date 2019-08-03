#ifndef __OLED_ARCH_H
#define __OLED_ARCH_H	

void OLED_WrDat(unsigned char dat);	    
void OLED_WrCmd(unsigned char cmd);
void OLED_Setpos(unsigned char y,unsigned char column);
void OLED_Fill(unsigned char dd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
							   		    
void OLED_Init(void);

#endif  
	 



