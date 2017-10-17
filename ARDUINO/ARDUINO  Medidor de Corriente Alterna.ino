while(true){
   val=0;
   maxvalue=511;
   minvalue=511;
   for(int i=0; i<2000; i++){
      val=analogRead(analogPin);    // lee el valor de corriente del pin
      if(val>maxvalue) 
         maxvalue=val;
      if(val<minvalue) 
         minvalue=val;
   }
   val=((maxvalue-minvalue)*0.03)/1.4142;
   Serial.print("Irms= " );
   Serial.print(val-0.16);  // imprime el valor leído con una calibración de -0.16
   Serial.print(" [A] ");
   delay(1000);
                           //coloca el cursor en la columna 0, la línea 1
                           // (Nota: la línea 1 es la segunda fila, ya que el recuento comienza con 0):
   lcd.setCursor(0, 1);
                           // imprimir el número de segundos desde reinicio:
   lcd.print(val-0.16);
   lcd.print(" A.");
   delay(250);
}