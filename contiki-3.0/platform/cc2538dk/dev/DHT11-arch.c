#include "dev/gpio.h"
#include "clock.h"

#define DHT11_DATA_PORT   GPIO_B_BASE
#define DHT11_DATA_BIT    (1 << 0)
#define DHT11_DATA        DHT11_DATA_PORT, DHT11_DATA_BIT  

void Delay_us(void);
void Delay_ms(unsigned int Time);
void Start_DHT11(void);
void DHT11_Inint(void);


unsigned char DHT11_FLAG,DHT11_temp;
unsigned char Hum_dec,Hum_uni,Tem_dec,Tem_uni=4;
unsigned char Tem_data_H,Tem_data_L,RH_data_H,RH_data_L,DHT11_checkdata;
unsigned char Tem_data_H_temp,Tem_data_L_temp,RH_data_H_temp,RH_data_L_temp,DHT11_checkdata_temp;
unsigned char DHT11_comdata;

static void
gpio_set1(int port, int bit)
{
  REG((port | GPIO_DATA) + (bit << 2)) = bit;
}
/*---------------------------------------------------------------------------*/
static void
gpio_reset1(int port, int bit)
{
  REG((port | GPIO_DATA) + (bit << 2)) = 0;
}
/*---------------------------------------------------------------------------*/
static int
gpio_get1(int port, int bit)
{
  return REG((port | GPIO_DATA) + (bit << 2));
}
static void
Delay_us_dht11(unsigned char DELAY)
{
    clock_delay_usec(DELAY);
}
/*void Delay_us(void) 
{
    MicroWait(1);   
}

void Delay_10us(void) 
{
   MicroWait(10);
}*/

void Delay_ms_dht11(unsigned int Time)//n msÑÓÊ±
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     Delay_us_dht11(10);
  }
}

//
void Start_DHT11(void)    // ÎÂÊªÐŽÈë
{     
    unsigned char i;         
    for(i=0;i<8;i++)    
    {
        DHT11_FLAG=2; 
        while((!gpio_get1(DHT11_DATA))&&DHT11_FLAG++);
        Delay_us_dht11(10);
	Delay_us_dht11(10);
	Delay_us_dht11(10);
        DHT11_temp=0;
        if(gpio_get1(DHT11_DATA))DHT11_temp=1;
        DHT11_FLAG=2;
        while((gpio_get1(DHT11_DATA))&&DHT11_FLAG++);   
        if(DHT11_FLAG==1)break;    
        DHT11_comdata<<=1;
        DHT11_comdata|=DHT11_temp; 
    }    
}

void DHT11_Inint(void)   
{
    gpio_reset1(DHT11_DATA);
    Delay_ms_dht11(10);  //>18MS
    Delay_ms_dht11(10); 
    Delay_ms_dht11(10); 
    gpio_set1(DHT11_DATA);
    GPIO_SET_INPUT(DHT11_DATA_PORT,DHT11_DATA_BIT);//INPUT
    Delay_us_dht11(10);
    Delay_us_dht11(10);
    Delay_us_dht11(10);
    Delay_us_dht11(5);
    if(!gpio_get1(DHT11_DATA))
    {
        DHT11_FLAG=2; 
        while((!gpio_get1(DHT11_DATA))&&DHT11_FLAG++);
        DHT11_FLAG=2;
        while((gpio_get1(DHT11_DATA))&&DHT11_FLAG++); 
        Start_DHT11();
        Delay_us_dht11(10);
        RH_data_H_temp=DHT11_comdata;
        Start_DHT11();
        Delay_us_dht11(10);
        RH_data_L_temp=DHT11_comdata;
        Start_DHT11();
	Delay_us_dht11(10);
        Tem_data_H_temp=DHT11_comdata;
        Start_DHT11();
	Delay_us_dht11(10);
        Tem_data_L_temp=DHT11_comdata;
        Start_DHT11();
	Delay_us_dht11(10);
        DHT11_checkdata_temp=DHT11_comdata;
        gpio_set1(DHT11_DATA);
	Delay_us_dht11(30);
        DHT11_temp=(Tem_data_H_temp+Tem_data_L_temp+RH_data_H_temp+RH_data_L_temp);
        if(DHT11_temp==DHT11_checkdata_temp)
        {
            RH_data_H=RH_data_H_temp;
            RH_data_L=RH_data_L_temp;
            Tem_data_H=Tem_data_H_temp;
            Tem_data_L=Tem_data_L_temp;
            DHT11_checkdata=DHT11_checkdata_temp;
        }
        Tem_dec=Tem_data_H/10; 
        Tem_uni=Tem_data_H%10;
        
        Hum_dec=RH_data_H/10; 
        Hum_uni=RH_data_H%10;        
    } 
    else //
    {
        Tem_dec=0; 
        Tem_uni=0;
        
        Hum_dec=0; 
        Hum_uni=0;  
    } 
    
    GPIO_SET_OUTPUT(DHT11_DATA_PORT,DHT11_DATA_BIT); //OUTPUT
    Delay_us_dht11(25);
}

