#include <Adafruit_NeoPixel.h>

#define PIN D2
#define STRIPSIZE 20
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPSIZE, PIN, NEO_GRB + NEO_KHZ800);

void setup(){
   strip.begin();
   strip.setBrightness(20);
}

void loop(){
   colorWipe(strip.Color(128, 0, 0), 100);
   colorWipe(strip.Color(0, 128, 0), 100);
   colorWipe(strip.Color(0, 0, 128), 100);
   colorXmas(200);
   Strobe(0, 128, 0, 20, 30, 100);
   Strobe(128, 0, 0, 20, 30, 100);
   colorXmas(200);
   colorWave(75);
   colorXmas(100);
   Strobe(128, 0, 0, 20, 30, 100);
   Strobe(0, 128, 0, 20, 30, 100);
   rainbow(15);
   colorXmas(100);
   rainbowCycle(15);
   colorXmas(200);
   Strobe(128, 128, 128, 10, 30, 100);
   colorXmas(100);
   Strobe(128, 128, 128, 10, 30, 100);
   colorXmas(50);
   colorWave(30);
}

void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause){
   for(int j=0; j<StrobeCount; j++){
      for(int i=0; i<strip.numPixels(); i++){
         strip.setPixelColor(i, strip.Color(red, green, blue));
      }
      strip.show();
      delay(FlashDelay);
      for(int i=0; i<strip.numPixels(); i++){
         strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
      delay(FlashDelay);
   }
   delay(EndPause);
}

void colorXmas(uint8_t wait){
   strip.setPixelColor(0, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(1, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(2, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(3, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(4, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(5, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(6, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(7, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(8, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(9, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(10, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(11, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(12, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(13, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(14, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(15, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(16, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(17, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(18, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(19, strip.Color(0, 64, 0)); strip.show(); delay(wait);
}
 
void colorWipe(uint32_t c, uint8_t wait){
   for(uint16_t i=0; i<strip.numPixels(); i++){
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
   }
}

void rainbow(uint8_t wait){
   uint16_t i, j;
   for(j=0; j<256; j++){
      for(i=0; i<strip.numPixels(); i++){
         strip.setPixelColor(i, Wheel((i+j) & 255));
      }
      strip.show();
      delay(wait);
   }
}

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

void colorWave(uint8_t wait){
   int i, j, stripsize, cycle;
   float ang, rsin, gsin, bsin, offset;
   static int tick = 0;
   stripsize = strip.numPixels();
   cycle = stripsize * 25;
   while (++tick % cycle){
      offset = map2PI(tick);
      for (i = 0; i < stripsize; i++){
         ang = map2PI(i) - offset;
         rsin = sin(ang);
         gsin = sin(2.0 * ang / 3.0 + map2PI(int(stripsize/6)));
         bsin = sin(4.0 * ang / 5.0 + map2PI(int(stripsize/3)));
         strip.setPixelColor(i, strip.Color(trigScale(rsin), trigScale(gsin), trigScale(bsin)));
      }
      strip.show();
      delay(wait);
   }
}

byte trigScale(float val){
   val += 1.0;
   val *= 127.0;
   return int(val) & 255;
}
 
float map2PI(int i){
   return PI*2.0*float(i) / float(strip.numPixels());
}
