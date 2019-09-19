#include <ESP8266WiFi.h>
void setup() {
   delay(500);
   wifi_set_opmode(STATION_MODE);
   wifi_promiscuous_enable(1); 
}
void loop(){
   //MAXIMO 32 Caracteres
   //..........13245678901234567890123456789012
   sendBeacon(" - PONGA SU ANUNCIO AQUI !!! -  ");
}
void sendBeacon(char* ssid){
   byte channel = random(1,12); 
   wifi_set_channel(channel);
   uint8_t packet[128] = { 0x80, 0x00, //Frame Control 
                           0x00, 0x00, //Duration
                   /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //Destination address 
                   /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //Source address - overwritten later
                   /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //BSSID - overwritten to the same as the source address
                   /*22*/  0xc0, 0x6c, //Seq-ctl
                   //Frame body starts here
                   /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, //timestamp - the number of microseconds the AP has been active
                   /*32*/  0xFF, 0x00, //Beacon interval
                   /*34*/  0x01, 0x04, //Capability info
                   /* SSID */
                   /*36*/  0x00};
   int ssidLen = strlen(ssid);
   packet[37] = ssidLen;
   for(int i = 0; i < ssidLen; i++){
      packet[38+i] = ssid[i];
   }
   uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                           0x03, 0x01, 0x04 /*DSSS (Current Channel)*/ };
   for(int i = 0; i < 12; i++){
      packet[38 + ssidLen + i] = postSSID[i];
   }
   packet[50 + ssidLen] = channel;
   // Randomize SRC MAC
   packet[10] = packet[16] = random(256);
   packet[11] = packet[17] = random(256);
   packet[12] = packet[18] = random(256);
   packet[13] = packet[19] = random(256);
   packet[14] = packet[20] = random(256);
   packet[15] = packet[21] = random(256);
   int packetSize = 51 + ssidLen;
   wifi_send_pkt_freedom(packet, packetSize, 0);
   wifi_send_pkt_freedom(packet, packetSize, 0);
   wifi_send_pkt_freedom(packet, packetSize, 0);
   delay(1);
}
