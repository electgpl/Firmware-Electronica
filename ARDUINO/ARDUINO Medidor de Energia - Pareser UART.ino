#include <SoftwareSerial.h>
SoftwareSerial mySerial(5, 6); // RX, TX

uint8_t readNBytes[8] = {0xA5, 0x08, 0x41, 0x00, 0x02, 0x4E, 0x20, 0x5E};
uint8_t respuesta[35];
int i = 0;
int veces = 0;
int dosBytes = 0;
int cuatroBytes = 0;

void setup(){
   mySerial.begin(9600);
   Serial.begin(9600);
   mySerial.write(readNBytes, 8);
}

void loop(){
   i = 0;
   delay(100);
   while(mySerial.available()){
      respuesta[i] = mySerial.read();
      i++;
   }
   if(veces < 1){
      Serial.print("ACK:                "); Serial.println(respuesta[0]);
      Serial.print("NoB:                "); Serial.println(respuesta[1]);
      Serial.print("Sys Status:         "); dosBytes = respuesta[2]  | respuesta[3] << 8; Serial.println(dosBytes,HEX);
      Serial.print("Sys Version:        "); dosBytes = respuesta[4]  | respuesta[5] << 8; Serial.println(dosBytes,HEX);
      Serial.print("Voltage RMS:        "); dosBytes = respuesta[6]  | respuesta[7] << 8; Serial.println(dosBytes);
      Serial.print("Line Frequency:     "); dosBytes = respuesta[8]  | respuesta[9] << 8; Serial.println(dosBytes);
      Serial.print("Thermistor Voltage: "); dosBytes = respuesta[10] | respuesta[11] << 8; Serial.println(dosBytes);
      Serial.print("Power Factor:       "); dosBytes = respuesta[12] | respuesta[13] << 8; Serial.println(dosBytes);
      Serial.print("Current RMS:        "); dosBytes = respuesta[16] | respuesta[17] << 8; 
      cuatroBytes = dosBytes << 16 | respuesta[14] | respuesta[15] << 8; Serial.println(cuatroBytes);
      Serial.print("Active Power:       "); dosBytes = respuesta[20] | respuesta[21] << 8;
      cuatroBytes = dosBytes << 16 | respuesta[18] | respuesta[19] << 8; Serial.println(cuatroBytes);
      Serial.print("Reactive Power:     "); dosBytes = respuesta[24] | respuesta[25] << 8;
      cuatroBytes = dosBytes << 16 | respuesta[22] | respuesta[23] << 8; Serial.println(cuatroBytes);
      Serial.print("Apparent Power:     "); dosBytes = respuesta[28] | respuesta[29] << 8;
      cuatroBytes = dosBytes << 16 | respuesta[26] | respuesta[27] << 8; Serial.println(cuatroBytes);
      Serial.print("32 al pedo:         "); dosBytes = respuesta[32] | respuesta[33] << 8;
      cuatroBytes = dosBytes << 16 | respuesta[30] | respuesta[31] << 8; Serial.println(cuatroBytes,HEX);
      Serial.print("Checksum:           "); Serial.println(respuesta[34],HEX);
      veces++;
   }
}
