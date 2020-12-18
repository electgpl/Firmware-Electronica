// Configurar Pin de salida (por defecto 2)
// Configurar Cantidad de LEDs (por defecto 50)
// Configurar Brillo global de los leds (por defecto 50)
// Dentro del loop todas las funciones de los leds, se pueden modificar, cambiar de lugar en la secuencia, eliminar o comentar.

#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel

#define PIN 2                                    //Pin de salida para los leds
#define STRIPSIZE 50                             //Cantidad de leds
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
   strip.setBrightness(50);                      //Brillo de los leds
}

void loop(){                                     //Aqui sucede la magia de navidad
   colorWipe(strip.Color(128, 0, 0), 50);        //Barrido de color ROJO
   colorWipe(strip.Color(0, 128, 0), 50);        //Barrido de color VERDE
   colorWipe(strip.Color(0, 0, 128), 50);        //Barrido de color AZUL
   colorXmas(25);                                //Barrido de color VERDE y ROJO
   flashRandom(2, 200);                          //Flash en pixel individual color random
   Strobe(128, 128, 128, 10, 30, 100);           //Estroboscopio en todos los leds color BLANCO
   Strobe(128, 128, 128, 10, 30, 100);           //Estroboscopio en todos los leds color BLANCO
   Strobe(128, 128, 128, 10, 30, 100);           //Estroboscopio en todos los leds color BLANCO
   theaterChaseRainbow(50);                      //Spin de barrido de colores
   theaterChase(strip.Color(127, 127, 127), 50); //Spin de barrido Blanco
   theaterChase(strip.Color(127,   0,   0), 50); //Spin de barrido Rojo
   theaterChase(strip.Color(  0,   0, 127), 50); //Spin de barrido Azul
   rainbow(20);                                  //Fundido de colores
   flashRandom(10, 50);                          //Flash en pixel individual color random
   flashRandom(6, 100);                          //Flash en pixel individual color random
   flashRandom(2, 200);                          //Flash en pixel individual color random
   colorWipe(strip.Color(128, 0, 0), 40);        //Barrido de color ROJO
   colorWipe(strip.Color(0, 128, 0), 40);        //Barrido de color VERDE   
   theaterChaseRainbow(50);                      //Spin de barrido de colores
   rainbowFade2White(3, 3, 1);                   //Spin con Fade de colores y Stop
   Strobe(0, 128, 0, 20, 30, 100);               //Estroboscopio en todos los leds color Verde
   Strobe(128, 0, 0, 20, 30, 100);               //Estroboscopio en todos los leds color Rojo
   theaterChaseRainbow(50);                      //Spin de barrido de colores
   colorXmas(50);                                //Barrido de color VERDE y ROJO
   colorWave(5);                                 //Onda de colores RGB Potente
   colorXmas(50);                                //Barrido de color VERDE y ROJO
   Strobe(128, 0, 0, 20, 30, 100);               //Estroboscopio en todos los leds color Rojo
   Strobe(0, 128, 0, 20, 30, 100);               //Estroboscopio en todos los leds color Verde
   rainbow(15);                                  //Fundido de colores
   colorXmas(50);                                //Barrido de color VERDE y ROJO
   rainbowCycle(10);                             //Spin de color RGB Lento
   colorXmas(50);                                //Barrido de color VERDE y ROJO
   Strobe(128, 128, 128, 10, 30, 100);           //Estroboscopio en todos los leds color BLANCO
   colorXmas(50);                                //Barrido de color VERDE y ROJO
   flashRandom(3, 100);                          //Flash en pixel individual color random
   Strobe(128, 128, 128, 10, 30, 100);           //Estroboscopio en todos los leds color BLANCO
   colorXmas(50);                                //Barrido de color VERDE y ROJO
   colorWave(20);                                //Onda de colores RGB  
   rainbowFade2White(3, 3, 1);                   //Spin con Fade de colores y Stop   
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
   strip.setPixelColor(20, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(21, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(22, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(23, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(24, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(25, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(26, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(27, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(28, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(29, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(30, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(31, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(32, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(33, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(34, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(35, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(36, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(37, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(38, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(39, strip.Color(0, 64, 0)); strip.show(); delay(wait);   
   strip.setPixelColor(40, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(41, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(42, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(43, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(44, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(45, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(46, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(47, strip.Color(0, 64, 0)); strip.show(); delay(wait);
   strip.setPixelColor(48, strip.Color(64, 0, 0)); strip.show(); delay(wait);
   strip.setPixelColor(49, strip.Color(0, 64, 0)); strip.show(); delay(wait); 
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
