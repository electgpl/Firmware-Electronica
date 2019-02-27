#define INTERVAL_TASK1 5000
#define INTERVAL_TASK2 7000
#define INTERVAL_TASK3 11000
#define INTERVAL_TASK4 13000
 
unsigned long time_1 = 0;
unsigned long time_2 = 0;
unsigned long time_3 = 0;
unsigned long time_4 = 0;
 
void setup(){
   Serial.begin(9600);
}
 
void loop(){
   if(millis() > time_1 + INTERVAL_TASK1){
      time_1 = millis();
      Serial.print("SE EJECUTA TAREA 1 CADA: ");
      Serial.println(INTERVAL_TASK1);
   }
   if(millis() > time_2 + INTERVAL_TASK2){
      time_2 = millis();
      Serial.print("SE EJECUTA TAREA 2 CADA: ");
      Serial.println(INTERVAL_TASK2);
   }
   if(millis() > time_3 + INTERVAL_TASK3){
      time_3 = millis();
      Serial.print("SE EJECUTA TAREA 3 CADA: ");
      Serial.println(INTERVAL_TASK3);
   }
   if(millis() > time_4 + INTERVAL_TASK4){
      time_4 = millis();
      Serial.print("SE EJECUTA TAREA 4 CADA: ");
      Serial.println(INTERVAL_TASK4);
   }
}
