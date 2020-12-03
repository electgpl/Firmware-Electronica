#include <Adafruit_NeoPixel.h>

#define PIN 2
#define STRIPSIZE 50
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPSIZE, PIN, NEO_GRB + NEO_KHZ800);

uint8_t myColors[][3] = {{232, 100, 255},
                         {200, 200,  20},
                         { 30, 200, 200},
                         {237, 159, 176},
                         {255,   0,   0},
                         {108,  87, 229},
                         {162, 211, 172},
                        };

#define FAVCOLORS sizeof(myColors)

void setup(){
   strip.begin();
   strip.setBrightness(50);
}

void loop(){ 
   colorWipe(strip.Color(128, 0, 0), 50);
   colorWipe(strip.Color(0, 128, 0), 50);
   colorWipe(strip.Color(0, 0, 128), 50);
   colorXmas(100);
   flashRandom(2, 200);
   Strobe(128, 128, 128, 10, 30, 100);
   Strobe(128, 128, 128, 10, 30, 100);
   Strobe(128, 128, 128, 10, 30, 100);
   theaterChaseRainbow(50);
   whiteOverRainbow(75, 5);
   theaterChase(strip.Color(127, 127, 127), 50);
   theaterChase(strip.Color(127,   0,   0), 50);
   theaterChase(strip.Color(  0,   0, 127), 50);
   rainbow(50);
   flashRandom(10, 50);
   flashRandom(6, 100);
   flashRandom(2, 200);
   colorWipe(strip.Color(128, 0, 0), 40);
   colorWipe(strip.Color(0, 128, 0), 40);   
   theaterChaseRainbow(50);
   whiteOverRainbow(75, 5);
   rainbowFade2White(3, 3, 1);
   Strobe(0, 128, 0, 20, 30, 100);
   Strobe(128, 0, 0, 20, 30, 100);
   theaterChaseRainbow(50);
   theaterChaseRainbow(50);
   colorXmas(200);
   colorWave(5);
   colorXmas(100);
   Strobe(128, 0, 0, 20, 30, 100);
   Strobe(0, 128, 0, 20, 30, 100);
   rainbow(15);
   colorXmas(100);
   rainbowCycle(10);
   colorXmas(200);
   Strobe(128, 128, 128, 10, 30, 100);
   colorXmas(100);
   flashRandom(3, 100);
   Strobe(128, 128, 128, 10, 30, 100);
   colorXmas(50);
   colorWave(30);                               
}

void flashRandom(int wait, uint8_t howmany){
   for(uint16_t i=0; i<howmany; i++){
      int c = random(FAVCOLORS);
      int red = myColors[c][0];
      int green = myColors[c][1];
      int blue = myColors[c][2]; 
      int j = random(strip.numPixels());
      for(int x=0; x < 5; x++){
         int r = red * (x+1); r /= 5;
         int g = green * (x+1); g /= 5;
         int b = blue * (x+1); b /= 5;
         strip.setPixelColor(j, strip.Color(r, g, b));
         strip.show();
         delay(wait);
      }
      for(int x=5; x >= 0; x--){
         int r = red * x; r /= 5;
         int g = green * x; g /= 5;
         int b = blue * x; b /= 5; 
         strip.setPixelColor(j, strip.Color(r, g, b));
         strip.show();
         delay(wait);
      }
   }
}

void whiteOverRainbow(int whiteSpeed, int whiteLength){
   if(whiteLength >= strip.numPixels()) 
      whiteLength = strip.numPixels() - 1;
   int      head          = whiteLength - 1;
   int      tail          = 0;
   int      loops         = 3;
   int      loopNum       = 0;
   uint32_t lastTime      = millis();
   uint32_t firstPixelHue = 0;
   for(;;){
      for(int i=0; i<strip.numPixels(); i++){
         if(((i >= tail) && (i <= head)) || ((tail > head) && ((i >= tail) || (i <= head)))){
            strip.setPixelColor(i, strip.Color(0, 0, 0, 255));
         }else{
            int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
            strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
         }
      }
      strip.show();
      firstPixelHue += 40;
      if((millis() - lastTime) > whiteSpeed){
         if(++head >= strip.numPixels()){
            head = 0;
            if(++loopNum >= loops) 
               return;
         }
         if(++tail >= strip.numPixels()){
            tail = 0;
         }
         lastTime = millis();
      }
   }
}

/*
void pulseWhite(uint8_t wait){
   for(int j=0; j<256; j++){
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
      strip.show();
      delay(wait);
   }
   for(int j=255; j>=0; j--){
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
      strip.show();
      delay(wait);
   }
}*/

void rainbowFade2White(int wait, int rainbowLoops, int whiteLoops){
   int fadeVal=0, fadeMax=100;
   for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536; firstPixelHue += 256){
      for(int i=0; i<strip.numPixels(); i++){ 
         uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
         strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255, 255 * fadeVal / fadeMax)));
      }
      strip.show();
      delay(wait);
      if(firstPixelHue < 65536){
         if(fadeVal < fadeMax) fadeVal++;
      }else if(firstPixelHue >= ((rainbowLoops-1) * 65536)){ 
         if(fadeVal > 0) fadeVal--;
      }else{
         fadeVal = fadeMax;
      }
   }
   for(int k=0; k<whiteLoops; k++){
      for(int j=0; j<256; j++){
         strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
         strip.show();
      }
      delay(1000);
      for(int j=255; j>=0; j--){
         strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
         strip.show();
      }
   }
   delay(500);
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

void theaterChase(uint32_t color, int wait){
   for(int a=0; a<10; a++){
      for(int b=0; b<3; b++){
         strip.clear();
         for(int c=b; c<strip.numPixels(); c += 3){
            strip.setPixelColor(c, color);
         }
         strip.show();
         delay(wait);
      }
   }
}

void theaterChaseRainbow(int wait){
   int firstPixelHue = 0;
   for(int a=0; a<30; a++){
      for(int b=0; b<3; b++){
         strip.clear();
         for(int c=b; c<strip.numPixels(); c += 3){
            int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
            uint32_t color = strip.gamma32(strip.ColorHSV(hue));
            strip.setPixelColor(c, color);
         }
         strip.show();
         delay(wait);
         firstPixelHue += 65536 / 90;
      }
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
