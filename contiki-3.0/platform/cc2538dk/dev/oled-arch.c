/*
 * Copyright (c) 2012-2013, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "clock.h"
#include "reg.h" 
#include "dev/oled-arch.h"
#include "dev/gpio.h"

/* CLK = CLocK */
#define SPI_CLK_PORT  GPIO_A_BASE
#define SPI_CLK_BIT   (1 << 2)
#define SPI_CLK       SPI_CLK_PORT, SPI_CLK_BIT

/* CS = Chip Select */
#define SPI_CS_PORT   GPIO_D_BASE
#define SPI_CS_BIT    (1 << 5)
#define SPI_CS        SPI_CS_PORT, SPI_CS_BIT  

/* RST = Reset*/
#define SPI_RST_PORT GPIO_B_BASE
#define SPI_RST_BIT  (1 << 5)
#define SPI_RST      SPI_RST_PORT, SPI_RST_BIT

/* DC = D/C */
#define SPI_DC_PORT GPIO_A_BASE
#define SPI_DC_BIT  (1 << 4)
#define SPI_DC      SPI_DC_PORT, SPI_DC_BIT

/* SDIN =  */
#define SPI_SDIN_PORT GPIO_A_BASE
#define SPI_SDIN_BIT  (1 << 5)
#define SPI_SDIN      SPI_SDIN_PORT, SPI_SDIN_BIT  

/* Delay in us */
#define DELAY 10

/*---------------------------------------------------------------------------*/
static void
gpio_set(int port, int bit)
{
  REG((port | GPIO_DATA) + (bit << 2)) = bit;
}
/*---------------------------------------------------------------------------*/
static void
gpio_reset(int port, int bit)
{
  REG((port | GPIO_DATA) + (bit << 2)) = 0;
}
/*---------------------------------------------------------------------------*/
static int
gpio_get(int port, int bit)
{
  return REG((port | GPIO_DATA) + (bit << 2));
}
/*---------------------------------------------------------------------------*/
static void
delay(void)
{
    clock_delay_usec(DELAY);
}

/*---------------------------------------------------------------------------*/
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
void OLED_WrDat(unsigned char dat)
{
	unsigned char i=8;
	gpio_reset(SPI_CS);
	gpio_set(SPI_DC);
	gpio_reset(SPI_CLK);
	
	for(i=0;i<8;i++)
	{
		if(dat&0x80)
		{
			gpio_set(SPI_SDIN);
		}
		else
		{
			gpio_reset(SPI_SDIN);
		}
		gpio_set(SPI_CLK);   
		gpio_reset(SPI_CLK); 
		dat<<=1;    
	}
	gpio_set(SPI_CS);
}
/*---------------------------------------------------------------------------*/
//向SSD1306写入一个字节。
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WrCmd(unsigned char cmd)
{
	unsigned char i=8;	
	gpio_reset(SPI_CS);
	gpio_reset(SPI_DC);
	gpio_reset(SPI_CLK);
	for(i=0;i<8;i++)
	{
		if(cmd&0x80)
			gpio_set(SPI_SDIN);
		else
			gpio_reset(SPI_SDIN);
		gpio_set(SPI_CLK);   
		gpio_reset(SPI_CLK); 
		cmd<<=1;   
	} 	
	gpio_set(SPI_CS);
}
/*---------------------------------------------------------------------------*/
//********************设置显示屏行，列坐标**************************
void OLED_Setpos(unsigned char y, unsigned char column)
{
  y=y%8;
  column=column%128;

  OLED_WrCmd(0xb0+y);             //设置行页地址(0~7)
  //设置列地址的低地址· 0x00+(0-15)
  OLED_WrCmd(column&0x0f);
  //设置列地址的高地址· 0x10+(0-8)
  OLED_WrCmd(((column&0xf0)>>4)|0x10);
}
/*---------------------------------------------------------------------------*/
void OLED_Fill(unsigned char dd)
{
  unsigned char y,x;
  for(y=0;y<8;y++)
  {
    OLED_Setpos(y,0);
    for(x=0;x<128;x++)
    {
      OLED_WrDat(dd);
    }
  }
}
/*---------------------------------------------------------------------------*/
//开启OLED显示    
void OLED_Display_On(void)
{
  OLED_WrCmd(0X8D);  //SET DCDC命令
  OLED_WrCmd(0X14);  //DCDC ON
  OLED_WrCmd(0XAF);  //DISPLAY ON
}
/*---------------------------------------------------------------------------*/
//关闭OLED显示     
void OLED_Display_Off(void)
{
  OLED_WrCmd(0X8D);  //SET DCDC命令
  OLED_WrCmd(0X10);  //DCDC OFF
  OLED_WrCmd(0XAE);  //DISPLAY OFF
}
/*---------------------------------------------------------------------------*/
//初始化SSD1306              
void OLED_Init(void)
{   
 
  GPIO_SET_OUTPUT(SPI_CS_PORT,SPI_CS_BIT);
  GPIO_SET_OUTPUT(SPI_CLK_PORT,SPI_CLK_BIT);
  GPIO_SET_OUTPUT(SPI_DC_PORT,SPI_DC_BIT);
  GPIO_SET_OUTPUT(SPI_RST_PORT,SPI_RST_BIT);
  GPIO_SET_OUTPUT(SPI_SDIN_PORT,SPI_SDIN_BIT);
  
  gpio_reset(SPI_RST);                
  delay();
  gpio_set(SPI_RST);
  
  gpio_reset(SPI_CS); 

  OLED_Display_Off();

  OLED_WrCmd(0xae);//--turn off oled panel
  OLED_WrCmd(0x00);//---set low column address
  OLED_WrCmd(0x10);//---set high column address
  OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WrCmd(0x81);//--set contrast control register
  OLED_WrCmd(0xcf); // Set SEG Output Current Brightness
  OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     
  OLED_WrCmd(0xc8);//Set COM/Row Scan Direction  
  OLED_WrCmd(0xa6);//--set normal display
  OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
  OLED_WrCmd(0x3f);//--1/64 duty
  OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WrCmd(0x00);//-not offset
  OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
  OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_WrCmd(0xd9);//--set pre-charge period
  OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WrCmd(0xda);//--set com pins hardware configuration
  OLED_WrCmd(0x12);
  OLED_WrCmd(0xdb);//--set vcomh
  OLED_WrCmd(0x40);//Set VCOM Deselect Level
} 
/*---------------------------------------------------------------------------*/


