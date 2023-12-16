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

void setup(){
   strip.begin();
   strip.show();
   strip.setBrightness(BRIGHTNESS);
   delay(1000);
}

void loop(){
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
