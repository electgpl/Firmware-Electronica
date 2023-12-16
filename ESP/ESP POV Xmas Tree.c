#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#define LED_PIN     D1
#define NUM_LEDS    24
#define NUM_COLS    20
#define BRIGHTNESS  50
#define uDELAY      100
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const uint8_t christmasTreeMap[NUM_COLS][NUM_LEDS] = {
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 1, 1, 1, 5, 1, 1, 1, 1, 2, 1, 5, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0},
   {4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
   {4, 4, 4, 1, 5, 1, 2, 1, 5, 1, 2, 1, 5, 1, 2, 1, 5, 1, 2, 1, 1, 1, 3, 3},
   {4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
   {0, 0, 0, 1, 1, 1, 5, 1, 1, 1, 1, 2, 1, 5, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0},
   {0, 0, 0, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

void rainbowCycle(uint8_t wait){
   uint16_t i, j;
   for(j=0; j<256*5; j++){
      for(i=0; i< strip.numPixels(); i++){
         strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
      delay(wait);
   }
}

uint32_t Wheel(byte WheelPos){
   if(WheelPos < 85){
      return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
   }else if(WheelPos < 170){
      WheelPos -= 85;
      return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
   }else{
      WheelPos -= 170;
      return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
   }
}

void setup(){
   strip.begin();
   strip.show();
   strip.setBrightness(BRIGHTNESS);
   wifi_set_opmode(STATION_MODE);
   wifi_promiscuous_enable(1); 
   rainbowCycle(1);
   delay(1000);
}

void loop(){
   sendBeacon("FELIZ NAVIDAD!!! Ho Ho Ho!");
   for(int row=0; row<NUM_COLS; row++){
      for(int col=0; col<NUM_LEDS; col++){
         switch(christmasTreeMap[row][col]){
         case 1:
            strip.setPixelColor(col, strip.Color(0, 100, 0)); // Green
            break;
         case 2:
            strip.setPixelColor(col, strip.Color(255, 0, 0)); // Red
            break;
         case 3:
            strip.setPixelColor(col, strip.Color(0, 0, 255)); // Blue
            break;
         case 4:
            strip.setPixelColor(col, strip.Color(170, 30, 0)); // Brown
            break;
         case 5:
            strip.setPixelColor(col, strip.Color(255, 100, 0)); // Yellow
            break;
         case 6:
            strip.setPixelColor(col, strip.Color(30, 59, 11)); // White
            break;
         default:
            strip.setPixelColor(col, strip.Color(0, 0, 0)); // Off
            break;
         }
      }
      strip.show();
      delayMicroseconds(uDELAY);
   }
   delayMicroseconds(uDELAY * 4);
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
