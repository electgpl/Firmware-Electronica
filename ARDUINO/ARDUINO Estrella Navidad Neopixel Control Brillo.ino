#include <Adafruit_NeoPixel.h>

// Configuración de los LEDs
#define PIN 2             // Pin donde se conecta el Data de los NeoPixel
#define NUM_LEDS 50       // Número de LEDs NeoPixel
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// Variables de control
int currentEffect = 0;    // Índice del efecto actual
unsigned long lastChange = 0;
const int effectDuration = 5000; // Duración de cada efecto en milisegundos
int brightness = 32;     // Brillo de los LEDs (0-255)

// Escala un color por el brillo actual
uint32_t scaleBrightness(uint32_t color) {
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  r = (r * brightness) / 255;
  g = (g * brightness) / 255;
  b = (b * brightness) / 255;
  return strip.Color(r, g, b);
}

void setup() {
  strip.begin();
  strip.show(); // Inicializa todos los LEDs apagados
}

void loop() {
  // Cambia el efecto automáticamente después de "effectDuration"
  if (millis() - lastChange > effectDuration) {
    currentEffect = (currentEffect + 1) % 20; // Cambia al siguiente efecto
    lastChange = millis();
  }

  // Ejecuta el efecto actual
  switch (currentEffect) {
    case 0: colorWipe(scaleBrightness(strip.Color(255, 0, 0)), 50); break; // Rojo
    case 1: colorWipe(scaleBrightness(strip.Color(0, 255, 0)), 50); break; // Verde
    case 2: colorWipe(scaleBrightness(strip.Color(0, 0, 255)), 50); break; // Azul
    case 3: theaterChase(scaleBrightness(strip.Color(255, 255, 0)), 100); break; // Amarillo chispeante
    case 4: rainbowCycle(10); break; // Ciclo de arco iris
    case 5: twinkle(5); break; // Titileo aleatorio 
    case 6: sparkles(scaleBrightness(strip.Color(255, 255, 255)), 50); break; // Chispas blancas  
    case 7: runningLights(scaleBrightness(strip.Color(0, 255, 255)), 50); break; // Luces corriendo
    case 8: fadeInOut(scaleBrightness(strip.Color(255, 0, 0)), 20); break; // Rojo suave
    case 9: fireEffect(); break; // Efecto de fuego
    case 10: alternateColors(scaleBrightness(strip.Color(255, 0, 0)), scaleBrightness(strip.Color(0, 255, 0)), 500); break; // Alterna rojo y verde
    case 11: candyCane(); break; // Bastón de caramelo
    case 12: chasingRainbow(30); break; // Arco iris persiguiéndose
    case 13: twinkleCluster(scaleBrightness(strip.Color(255, 255, 255)), 20); break; // Grupos de destellos
    case 14: gradientWave(15); break; // Onda de degradado
    case 15: snowSparkles(30); break; // Nieve chispeante
    case 16: comet(scaleBrightness(strip.Color(255, 100, 100)), 20); break; // Cometa navideño
    case 17: colorPulse(scaleBrightness(strip.Color(0, 255, 0)), 50); break; // Pulso verde
    case 18: rainbowWipe(30); break; // Barrido arco iris
    case 19: snowfall(scaleBrightness(strip.Color(255, 255, 255)), 80); break; // Caída de nieve
  }
}

// ----------- Funciones de efectos -----------

// Llena los LEDs uno por uno con un color
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

// Efecto de luces que se encienden y apagan
void theaterChase(uint32_t color, int wait) {
  for (int j = 0; j < 10; j++) {
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i += 3) {
        strip.setPixelColor(i + q, color);
      }
      strip.show();
      delay(wait);
      for (int i = 0; i < strip.numPixels(); i += 3) {
        strip.setPixelColor(i + q, 0);
      }
    }
  }
}

// Ciclo de arco iris
void rainbowCycle(int wait) {
  for (int j = 0; j < 256 * 5; j++) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Alterna dos colores
void alternateColors(uint32_t color1, uint32_t color2, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, i % 2 == 0 ? color1 : color2);
  }
  strip.show();
  delay(wait);
}

void twinkle(int count) {
    for (int i = 0; i < count; i++) {
        int pixel = random(strip.numPixels());
        strip.setPixelColor(pixel, strip.Color(brightness, brightness, brightness)); // Color blanco
        strip.show();
        delay(100);
        strip.setPixelColor(pixel, 0); // Apagar el pixel
    }
}

void sparkles(uint32_t color, int delayMs) {
    for (int i = 0; i < strip.numPixels(); i++) {
        if (random(10) > 7) { // Ajusta la probabilidad de chispa
            strip.setPixelColor(i, color);
        } else {
            strip.setPixelColor(i, 0); // Apagar
        }
    }
    strip.show();
    delay(delayMs);
}

void runningLights(uint32_t color, int waveDelay) {
  int position = 0;
  for (int i = 0; i < strip.numPixels() * 2; i++) {
    position++; // Incrementa posición
    for (int j = 0; j < strip.numPixels(); j++) {
      // Calcula el factor de brillo
      float brightnessFactor = (sin(j + position) * 127 + 128) / 255.0;

      // Extrae componentes RGB del color original
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;

      // Escala cada componente por el factor
      uint8_t scaledR = r * brightnessFactor;
      uint8_t scaledG = g * brightnessFactor;
      uint8_t scaledB = b * brightnessFactor;

      // Reconstruye el color ajustado
      uint32_t scaledColor = strip.Color(scaledR, scaledG, scaledB);

      // Aplica el color ajustado
      strip.setPixelColor(j, scaleBrightness(scaledColor));
    }
    strip.show();
    delay(waveDelay);
  }
}

// Efecto de fade in-out
void fadeInOut(uint32_t color, int wait) {
  for (int brightness = 0; brightness < 256; brightness++) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, colorDim(color, brightness));
    }
    strip.show();
    delay(wait);
  }
  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, colorDim(color, brightness));
    }
    strip.show();
    delay(wait);
  }
}

// Efecto de fuego
void fireEffect() {
  for (int i = 0; i < strip.numPixels(); i++) {
    int flicker = random(0, 55);

    // Calcula los valores de color con la reducción del "flicker"
    int red = max(0, 255 - flicker);
    int green = max(0, 165 - flicker / 2);

    // Aplica escala de brillo directamente
    uint32_t scaledColor = strip.Color(red, green, 0);

    // Usa `scaleBrightness` si es necesario
    strip.setPixelColor(i, scaleBrightness(scaledColor));
  }
  strip.show();
  delay(50);
}

void snowSparkles(int delayMs) {
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 255, 255)); // Blanco nieve
        strip.show();
        delay(random(10, delayMs)); // Chispeo aleatorio
        strip.setPixelColor(i, 0);
    }
}

// Efecto bastón de caramelo
void candyCane() {
  for (int i = 0; i < strip.numPixels(); i++) {
    uint32_t color;
    if ((i % 6) < 3) {
      // Rojo
      color = strip.Color(255, 0, 0);
    } else {
      // Blanco
      color = strip.Color(255, 255, 255);
    }
    // Ajustar el brillo antes de aplicar el color
    strip.setPixelColor(i, scaleBrightness(color));
  }
  strip.show();
  delay(500);
}

// Arco iris en movimiento
void chasingRainbow(int wait) {
  for (int j = 0; j < 256; j++) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Pulso de color
void colorPulse(uint32_t color, int wait) {
  for (int brightness = 0; brightness < 256; brightness++) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, colorDim(color, brightness));
    }
    strip.show();
    delay(wait);
  }
  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, colorDim(color, brightness));
    }
    strip.show();
    delay(wait);
  }
}

// Caída de nieve
void snowfall(uint32_t color, int wait) {
  strip.clear();
  int pixel = random(strip.numPixels());
  strip.setPixelColor(pixel, color);
  strip.show();
  delay(wait);
}

// Grupos de destellos
void twinkleCluster(uint32_t color, int count) {
  strip.clear();
  for (int i = 0; i < count; i++) {
    int pixel = random(strip.numPixels());
    strip.setPixelColor(pixel, color);
    strip.show();
    delay(100);
    strip.setPixelColor(pixel, 0);
  }
}

// Simula un cometa
void comet(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.clear();
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

// Onda de degradado
void gradientWave(int wait) {
  for (int j = 0; j < 256; j++) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Barrido de arco iris
void rainbowWipe(int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, wheel(i * 256 / strip.numPixels()));
    strip.show();
    delay(wait);
  }
}

// Funciones auxiliares
uint32_t wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color((255 - WheelPos * 3) * brightness / 255, 0, (WheelPos * 3) * brightness / 255);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, (WheelPos * 3) * brightness / 255, (255 - WheelPos * 3) * brightness / 255);
  } else {
    WheelPos -= 170;
    return strip.Color((WheelPos * 3) * brightness / 255, (255 - WheelPos * 3) * brightness / 255, 0);
  }
}

uint32_t colorDim(uint32_t color, int brightness) {
  uint8_t r = (uint8_t)(color >> 16);
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)color;
  return strip.Color(r * brightness / 255, g * brightness / 255, b * brightness / 255);
}