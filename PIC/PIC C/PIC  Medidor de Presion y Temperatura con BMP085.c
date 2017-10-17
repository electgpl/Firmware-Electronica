#include <16F883.h> 
#use delay(int=4000000)
#use i2c(master,sda=PIN_C4,scl=PIN_C3,FAST,FORCE_HW)
#include <lcd.c>
signed int16 AC1, AC2, AC3, B1, B2, MC, MD, MB;
int16 AC4, AC5, AC6;
float xC3, xC4, xB1, xC5, xC6, xMc, xMd, xX0, xX1;
float xY1, xY2, xP0, xP1, xP2, xS, xX2, xY0; 
 
int8 DescargaByte_BMP085(int8 address){
   int8 datos;
   i2c_Start();
   i2c_write(0xEE);
   i2c_write(address);
   i2c_Start();
   i2c_write(0xEE|0x01);
   datos=i2c_read(0);
   i2c_Stop();
   return(datos);
}
int16 DescargaEntero_BMP085(int8 address){
   int16 ent;
   i2c_Start();
   i2c_write(0xEE);
   i2c_write(address);
   i2c_Start();
   i2c_write(0xEE|0x01);
   ent=make16(i2c_read(),i2c_read(0));
   i2c_Stop();
   return(ent);
}
void inicializa_BMP085(){
   AC1 = DescargaEntero_BMP085(0xAA);
   AC2 = DescargaEntero_BMP085(0xAC);
   AC3 = DescargaEntero_BMP085(0xAE);
   AC4 = DescargaEntero_BMP085(0xB0);
   AC5 = DescargaEntero_BMP085(0xB2);
   AC6 = DescargaEntero_BMP085(0xB4);
   B1  = DescargaEntero_BMP085(0xB6);
   B2  = DescargaEntero_BMP085(0xB8);
   MB  = DescargaEntero_BMP085(0xBA); 
   MC  = DescargaEntero_BMP085(0xBC);
   MD  = DescargaEntero_BMP085(0xBE);
   xC3 = 0.00489*AC3;
   xC4 = 0.0000000306*AC4;
   xC5 = 0.000000191*AC5;
   xC6 = (float)AC6;
   xB1 = 0.0000238*B1; 
   xMc = 0.0812*MC; 
   xMd = (float)MD/160;   
   xX0 = (float)AC1;
   xX1 = 0.0195*AC2;
   xX2 = 0.000763*B2;   
   xY0 = xC4*32768;
   xY1 = xC4*xC3;
   xY2 = xC4*xB1;
}  
int16 DescargaTemp_BMP085(){
   i2c_Start();
   i2c_write(0xEE);
   i2c_write(0xF4);
   i2c_write(0x2E);
   i2c_Stop();
   delay_ms(5);
   return((float)DescargaEntero_BMP085(0xF6));
}
int32 DescargaPres_BMP085(){
   i2c_Start();
   i2c_write(0xEE);
   i2c_write(0xF4);
   i2c_write(0x34+(3<<6));
   i2c_Stop();  
   delay_ms(26);
   return((256*DescargaByte_BMP085(0xF6))+
               DescargaByte_BMP085(0xF7)+
          (DescargaByte_BMP085(0xF8)/256));
}
float LeeTemperatura_BMP085(float tempeAux){
   float Conversor, Temperatura;
   Conversor=xC5*(tempeAux-xC6);
   Temperatura=Conversor+(xMc/(Conversor+xMd));
   xS=Temperatura-25;
   return(Temperatura);
}   
float LeePresion_BMP085(float presAux){
   float auxA, auxB, auxC;
   float Presion;
   auxA=xX2*xS*xS+xX1*xS+xX0;
   auxB=xY2*xS*xS+xY1*xS+xY0;
   auxC=((float)presAux-auxA)/auxB;
   Presion=0.0000045*auxC*auxC+0.994*auxC+2.37;
   Presion+=4.58;
   return(Presion);
}
void main(){
   lcd_init();
   inicializa_BMP085();
   while(true){
      lcd_gotoxy(1,1);
      printf(lcd_putc,"T[c]: %.1g  \n",LeeTemperatura_BMP085(DescargaTemp_BMP085()));
      lcd_gotoxy(1,2);
      printf(lcd_putc,"P[mBar]: %0.2g  \n",LeePresion_BMP085(DescargaPres_BMP085()));
      delay_ms(500); 
   }
}