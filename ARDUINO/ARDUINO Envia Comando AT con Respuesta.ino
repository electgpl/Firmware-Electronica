sendATcommand("AT+CIPMODE=0", "OK", 1000);

uint8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout){
   uint8_t x=0,  answer=0;
   char response[100];
   unsigned long previous;
   memset(response, '\0', 100);                                 // Initialisa String
   delay(100);
   while( Serial.available() > 0)
	  Serial.read();                                              // Limpia el buffer de Entrada
   Serial.println(ATcommand);                                   // Envia el comando AT
   x = 0;
   previous = millis();
   do{                                                          // Espera la respeusta
      if(Serial.available() != 0){                              // Carga el Vector con el dato leido en UART
         response[x] = Serial.read();
         x++;
         if(strstr(response, expected_answer) != NULL){         // Espera a la respuesta del comando enviado
            answer = 1;
         }
      }
   }while((answer == 0) && ((millis() - previous) < timeout));  // Sale por respuesta correcta o timeout
   return answer;
}

