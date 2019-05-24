int periodo1 = 4000;
int periodo2 = 1000;
int periodo3 = 500;
unsigned long TiempoAhora1 = 0;
unsigned long TiempoAhora2 = 0;
unsigned long TiempoAhora3 = 0;
void setup() {
    Serial.begin(9600);
}
void loop() {
    if(millis() > TiempoAhora1 + periodo1){
        TiempoAhora1 = millis();
        Serial.println("tarea1");
    }
    if(millis() > TiempoAhora2 + periodo2){
        TiempoAhora2 = millis();
        Serial.println("tarea2");
    }
    if(millis() > TiempoAhora3 + periodo3){
        TiempoAhora3 = millis();
        Serial.println("tarea3");
    }
}
