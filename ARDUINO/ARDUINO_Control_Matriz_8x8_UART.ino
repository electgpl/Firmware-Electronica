#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
 
//Vcc - Vcc
//Gnd - Gnd
//Din - Mosi (Pin 11)
//Cs  - SS (Pin 10)
//Clk - Sck (Pin 13)
 
const int pinCS = 10;
const int numberOfHorizontalDisplays = 8;
const int numberOfVerticalDisplays = 1;
 
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

const int wait = 50; // Velocidad a la que realiza el scroll
 
const int spacer = 1;
const int width = 5 + spacer; // Ancho de la fuente a 5 pixeles
 
void setup(){
   Serial.begin(9600);
   matrix.setIntensity(1); // Ajustar el brillo entre 0 y 15
   matrix.setPosition(0, 0, 0); // El primer display esta en <0, 0>
   matrix.setPosition(1, 1, 0); // El segundo display esta en <1, 0>
   matrix.setPosition(2, 2, 0); // El tercer display esta en <2, 0>
   matrix.setPosition(3, 3, 0); // El cuarto display esta en <3, 0>
   matrix.setPosition(4, 4, 0); // El quinto display esta en <4, 0>
   matrix.setPosition(5, 5, 0); // El sexto display esta en <5, 0>
   matrix.setPosition(6, 6, 0); // El séptimo display esta en <6, 0>
   matrix.setPosition(7, 7, 0); // El octavo display esta en <7, 0>
   matrix.setPosition(8, 8, 0); // El noveno display esta en <8, 0>
   matrix.setRotation(0, 1);    // Posición del display
   matrix.setRotation(1, 1);    // Posición del display
   matrix.setRotation(2, 1);    // Posición del display
   matrix.setRotation(3, 1);    // Posición del display
   matrix.setRotation(4, 1);    // Posición del display
   matrix.setRotation(5, 1);    // Posición del display
   matrix.setRotation(6, 1);    // Posición del display
   matrix.setRotation(7, 1);    // Posición del display
   matrix.setRotation(8, 1);    // Posición del display
}
 
void loop(){
   String cadena = "";
   long int time = millis();
   while(Serial.available()){
      cadena += char(Serial.read());
   }
   for(int i = 0; i < width * cadena.length() + matrix.width() - 1 - spacer; i++){
      matrix.fillScreen(LOW);
      int letter = i / width;
      int x = (matrix.width() - 1) - i % width;
      int y = (matrix.height() - 8) / 2; // Centrar el texto
      while(x + width - spacer >= 0 && letter >= 0){
         if(letter < cadena.length()){
             matrix.drawChar(x, y, cadena[letter], HIGH, LOW, 1);
         }
         letter--;
         x -= width;
      }
      matrix.write(); // Muestra loscaracteres
      delay(wait);
   }
}
