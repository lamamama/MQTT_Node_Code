#ifndef __DHT11_H
#define __DHT11_H

extern void Delay_ms(unsigned int xms);	
extern void Start_DHT11(void);              
extern void DHT11_Inint(void);               

extern unsigned char temp[2]; 
extern unsigned char humidity[2];
extern unsigned char Hum_dec,Hum_uni,Tem_dec,Tem_uni,Tem_data_H,RH_data_H;

#endif
