#include <16F883.h>
#fuses HS,NOWDT,NOPROTECT,PUT,NOLVP,NOMCLR,INTRC
#use delay(clock=20000000)
#bit FlagDesbordaTimer1 = 0x0C.0
#include <LCD.C>
void main(){
   lcd_init();
   int Ciclo1byte, Ciclos;
   int32 Frecuencia;
   long ParteAlta, ParteBaja;
   while(true){
      Ciclo1byte = 0;
      Ciclos = 0;
      ParteAlta = 0;
      FlagDesbordaTimer1 = 0;
      set_timer1(0);
      setup_timer_1(T1_EXTERNAL|T1_DIV_BY_1);
      while(Ciclos!=0xFF) {
         Ciclo1byte = 0;
         while(Ciclo1byte!=0xFF) {
            if(FlagDesbordaTimer1){
               FlagDesbordaTimer1 = 0; 
               ParteAlta++;
            }
            else{
               delay_cycles(5);
            }
            delay_cycles(62);
            Ciclo1byte++;
         }
      delay_cycles(216);
      Ciclos++;
      }
      delay_cycles(211);
      setup_timer_1(T1_DISABLED);
      if(FlagDesbordaTimer1){
         ParteAlta++;
      }   
      ParteBaja  = get_timer1();
      Frecuencia = make32(ParteAlta,ParteBaja);
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Frecuencimetro"); 
      lcd_gotoxy(1,2);
      printf(lcd_putc,"%LU Hz",Frecuencia); 
   }
}