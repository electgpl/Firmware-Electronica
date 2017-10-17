// Define Pins used to control AD9850 DDS
const int FQ_UD=10;
const int SDAT=11;
const int SCLK=9;
const int RESET=12;


double Fstart_MHz = 1;  // Start Frequency for sweep
double Fstop_MHz = 10;  // Stop Frequency for sweep
double current_freq_MHz; // Temp variable used during sweep
long serial_input_number; // Used to build number from serial stream
int num_steps = 100; // Number of steps to use in the sweep
char incoming_char; // Character read from serial stream


// the setup routine runs once when you press reset:
void setup() {
  // Configiure DDS control pins for digital output
  pinMode(FQ_UD,OUTPUT);
  pinMode(SCLK,OUTPUT);
  pinMode(SDAT,OUTPUT);
  pinMode(RESET,OUTPUT);
  
  // Configure LED pin for digital output
  pinMode(13,OUTPUT);


  // Set up analog inputs on A0 and A1, internal reference voltage
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  analogReference(INTERNAL);
  
  // initialize serial communication at 57600 baud
  Serial.begin(57600);


  // Reset the DDS
  digitalWrite(RESET,HIGH);
  digitalWrite(RESET,LOW);
  
  //Initialise the incoming serial number to zero
  serial_input_number=0;

}

// the loop routine runs over and over again forever:
void loop() {
  //Check for character
  if(Serial.available()>0){
    incoming_char = Serial.read();
    switch(incoming_char){
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      serial_input_number=serial_input_number*10+(incoming_char-'0');
      break;
    case 'A':
      //Turn frequency into FStart
      Fstart_MHz = ((double)serial_input_number)/1000000;
      serial_input_number=0;
      break;
    case 'B':
      //Turn frequency into FStart
      Fstop_MHz = ((double)serial_input_number)/1000000;
      serial_input_number=0;
      break;
    case 'C':
      //Turn frequency into FStart and set DDS output to single frequency
      Fstart_MHz = ((double)serial_input_number)/1000000;
      SetDDSFreq(Fstart_MHz);
      serial_input_number=0;    
      break;
    case 'N':
      // Set number of steps in the sweep
      num_steps = serial_input_number;
      serial_input_number=0;
      break;
    case 'S':    
    case 's':    
      Perform_sweep();
      break;
    case '?':
      // Report current configuration to PC    
      Serial.print("Start Freq:");
      Serial.println(Fstart_MHz*1000000);
      Serial.print("Stop Freq:");
      Serial.println(Fstart_MHz*1000000);
      Serial.print("Num Steps:");
      Serial.println(num_steps);
      break;
    }
    Serial.flush();     
  } 
}

void Perform_sweep(){
  double FWD=0;
  double REV=0;
  double VSWR;
  double Fstep_MHz = (Fstop_MHz-Fstart_MHz)/num_steps;
 
  // Start loop 
  for(int i=0;i<=num_steps;i++){
    // Calculate current frequency
    current_freq_MHz = Fstart_MHz + i*Fstep_MHz;
    // Set DDS to current frequency
    SetDDSFreq(current_freq_MHz*1000000);
    // Wait a little for settling
    delay(10);
    // Read the forawrd and reverse voltages
    REV = analogRead(A0);
    FWD = analogRead(A1);
    if(REV>=FWD){
      // To avoid a divide by zero or negative VSWR then set to max 999
      VSWR = 999;
    }else{
      // Calculate VSWR
      VSWR = (FWD+REV)/(FWD-REV);
    }
    
    // Send current line back to PC over serial bus
    Serial.print(current_freq_MHz*1000000);
    Serial.print(",0,");
    Serial.print(int(VSWR*1000));
    Serial.print(",");
    Serial.print(FWD);
    Serial.print(",");
    Serial.println(REV);
  }
  // Send "End" to PC to indicate end of sweep
  Serial.println("End");
  Serial.flush();    
}

void SetDDSFreq(double Freq_Hz){
  // Calculate the DDS word - from AD9850 Datasheet
  int32_t f = Freq_Hz * 4294967295/125000000;
  // Send one byte at a time
  for (int b=0;b<4;b++,f>>=8){
    send_byte(f & 0xFF);
  }
  // 5th byte needs to be zeros
  send_byte(0);
  // Strobe the Update pin to tell DDS to use values
  digitalWrite(FQ_UD,HIGH);
  digitalWrite(FQ_UD,LOW);
}

void send_byte(byte data_to_send){
  // Bit bang the byte over the SPI bus
  for (int i=0; i<8; i++,data_to_send>>=1){
    // Set Data bit on output pin
    digitalWrite(SDAT,data_to_send & 0x01);
    // Strobe the clock pin
    digitalWrite(SCLK,HIGH);
    digitalWrite(SCLK,LOW);
  }
}