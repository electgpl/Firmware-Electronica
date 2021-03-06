#include <Wire.h>
#include <IR_Thermometer_Sensor_MLX90614.h> //https://github.com/DFRobot/IR-Thermometer-Sensor-MLX90614/tree/master/Arduino%20Library/IR_Thermometer_Sensor_MLX90614
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
IR_Thermometer_Sensor_MLX90614 MLX90614 = IR_Thermometer_Sensor_MLX90614();

void setup(){
   Serial.begin(9600);
   MLX90614.begin();  
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.setTextColor(SSD1306_WHITE); 
   display.clearDisplay();
   display.setCursor(0,0);
   display.setTextSize(2);
   display.println("Termometro");
   display.println("Infrarrojo");
   display.println("Eelectgpl");
   display.write(2);
   display.display();
   delay(2000); 
}

void loop(){
   Serial.print("Ambient = "); Serial.print(MLX90614.GetAmbientTemp_Celsius());    Serial.println(" *C");
   Serial.print("Object  = "); Serial.print(MLX90614.GetObjectTemp_Celsius());     Serial.println(" *C");
   Serial.print("Ambient = "); Serial.print(MLX90614.GetAmbientTemp_Fahrenheit()); Serial.println(" *F");
   Serial.print("Object  = "); Serial.print(MLX90614.GetObjectTemp_Fahrenheit());  Serial.println(" *F");
   Serial.println();
   display.clearDisplay();
   display.setTextSize(2);
   display.setCursor(0,0);
   display.print(MLX90614.GetAmbientTemp_Celsius(),1); 
   display.setTextSize(2);
   display.setCursor(100,0);
   display.print((char)247);
   display.println("C");
   display.drawLine(0, 21, 127, 21, WHITE); 
   display.setTextSize(4);
   display.setCursor(31,30); 
   display.print(MLX90614.GetObjectTemp_Celsius(),1); 
   display.display();   
   delay(500);
}
