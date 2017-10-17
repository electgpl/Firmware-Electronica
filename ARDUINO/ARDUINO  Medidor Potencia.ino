// -- variables and pins definition ------------------
const int analogInPin = A0;         // analog input pin
int retardo = 2;                    // (tiempo (s.) entre visionados)
float lectura, ff, pKW, iA, vV, vS, S_Ratio;
// -- initialize serial comm & parameters ------------
void setup() {
   Serial.begin(9600);
   S_Ratio = 36.5;                  // Sensor/ratio (mV/mA ) : 36.5
   vV = 230;                        // valor de tension a computar
   ff = 5;                          // freq. factor / (50Hz -> 5 / 60Hz -> 4.15)
}
// -- smooth read routine ----------------------------
float smoothread(float fc){         // fc (factor corrector)
   int ni = 35;                     // n. de iteraciones => smooth
                                    // (ni) => rango 10 a 50 mejor promedio [smoothing]
   float retorno = 0.0;
   for(int x = 0; x< ni; x++){
      do{                           // espero paso por cero
         delayMicroseconds(100);
      }while(analogRead(0) != 0);
      delay (ff); // espera centro de ciclo
      delay (10); // estabilizacion CAD
      retorno = retorno +(analogRead(0)*fc);
   }
   return retorno / ni;
}
// -- main loop --------------------------------------
void loop(){
   lectura = smoothread (1) / 1.41; // lectura (rms)
   vS = (lectura * 0.0048);         // valor de C.A.D.
   iA = (lectura * S_Ratio)/1000;   // Intensidad (A)
   pKW = (vV * iA)/1000;            // Potencia (kW)
   Serial.print("\n- Tension predefinida [V] --> " );
   Serial.print(vV,0);
   Serial.print("\n- Lectura del sensor [V] --> " );
   Serial.print(vS,3);
   Serial.print("\n- Intensidad calculada [A] --> " );
   Serial.print(iA,3);
   Serial.print("\n- Potencia calculada [kW] --> " );
   Serial.print(pKW,3);
   delay(retardo * 1000);
}