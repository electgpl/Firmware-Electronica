//***************************************************************************
// NOMBRE:             hal_i2c.c
// FECHA:              09/11/2016
// DESCRIPCION:        Rutina de Bit Banging para protocolo i2c.       
// LOG MOFICIACIONES:  N/A
//***************************************************************************
#include "ioCC2530.h"
#include "zcomdef.h"
#include "hal_i2c.h"
#define STATIC static
//***************************************************************************
//                      DEFINICION DE PINES DEL CC2530
//                            SDA: 1.6 y SCL: 1.5
//***************************************************************************
#ifndef OCM_DATA_PORT
  #define OCM_DATA_PORT 1
#endif
#ifndef OCM_DATA_PIN
  #define OCM_DATA_PIN  6
#endif
#ifndef OCM_CLK_PORT
  #define OCM_CLK_PORT  1
#endif
#ifndef OCM_CLK_PIN
  #define OCM_CLK_PIN   5
#endif
//***************************************************************************
//                            DEFINICION DE GPIO
//***************************************************************************
#define IO_GIO  0               // General purpose I/O
#define IO_PER  1               // Peripheral function
#define IO_IN   0               // Input pin
#define IO_OUT  1               // Output pin
#define IO_PUD  0               // Pullup/pulldn input
#define IO_TRI  1               // Tri-state input
#define IO_PUP  0               // Pull-up input pin
#define IO_PDN  1               // Pull-down input pin
#define OCM_ADDRESS  (0xA0)
#define OCM_READ     (0x01)
#define OCM_WRITE    (0x00)
#define SMB_ACK      (0)
#define SMB_NAK      (1)
#define SEND_STOP    (0)
#define NOSEND_STOP  (1)
#define SEND_START   (0)
#define NOSEND_START (1)
//#define hali2cWait(a)  asm("NOP")
//***************************************************************************
//                         DEFINICION DE MACROS
//***************************************************************************
#undef P
// I/O PORT CONFIGURATION
#define CAT1(x,y) x##y          // Concatenates 2 strings
#define CAT2(x,y) CAT1(x,y)     // Forces evaluation of CAT1
// OCM port I/O defintions
// Builds I/O port name: PNAME(1,INP) ==> P1INP
#define PNAME(y,z) CAT2(P,CAT2(y,z))
// Builds I/O bit name: BNAME(1,2) ==> P1_2
#define BNAME(port,pin) CAT2(CAT2(P,port),CAT2(_,pin))
//***************************************************************************
//                         DEFINICION DE PUERTOS
//***************************************************************************
#define OCM_SCL BNAME(OCM_CLK_PORT, OCM_CLK_PIN)
#define OCM_SDA BNAME(OCM_DATA_PORT, OCM_DATA_PIN)
#define IO_DIR_PORT_PIN(port, pin, dir) \
{\
  if ( dir == IO_OUT ) \
    PNAME(port,DIR) |= (1<<(pin)); \
  else \
    PNAME(port,DIR) &= ~(1<<(pin)); \
}
#define OCM_DATA_HIGH()\
{ \
  IO_DIR_PORT_PIN(OCM_DATA_PORT, OCM_DATA_PIN, IO_IN); \
}
#define OCM_DATA_LOW() \
{ \
  IO_DIR_PORT_PIN(OCM_DATA_PORT, OCM_DATA_PIN, IO_OUT); \
  OCM_SDA = 0;\
}
#define IO_FUNC_PORT_PIN(port, pin, func) \
{ \
  if( port < 2 ) \
  { \
    if ( func == IO_PER ) \
      PNAME(port,SEL) |= (1<<(pin)); \
    else \
      PNAME(port,SEL) &= ~(1<<(pin)); \
  } \
  else \
  { \
    if ( func == IO_PER ) \
      P2SEL |= (1<<(pin>>1)); \
    else \
      P2SEL &= ~(1<<(pin>>1)); \
  } \
}
#define IO_IMODE_PORT_PIN(port, pin, mode) \
{ \
  if ( mode == IO_TRI ) \
    PNAME(port,INP) |= (1<<(pin)); \
  else \
    PNAME(port,INP) &= ~(1<<(pin)); \
}
#define IO_PUD_PORT(port, dir) \
{ \
  if ( dir == IO_PDN ) \
    P2INP |= (1<<(port+5)); \
  else \
    P2INP &= ~(1<<(port+5)); \
}
//***************************************************************************
//                         PROTOTIPO DE FUNCIONES
//***************************************************************************
STATIC void   hali2cInit(void);
STATIC _Bool  hali2cWriteByte(uint8 dByte);
STATIC _Bool  hali2cWriteByteAdd(uint8 dByte);
STATIC _Bool  hali2cReadByteAdd(uint8 dByte);
STATIC void   hali2cClock(bool dir);
STATIC void   hali2cStart(void);
STATIC void   hali2cStop(void);
STATIC uint8  hali2cReadByte(void);
STATIC __near_func void   hali2cWait(uint8);
STATIC uint8 s_xmemIsInit;
STATIC _Bool  i2cRead(uint8  i2cNumber,
                      uint8  i2cSlaveAddress,
                      uint8* dataToReadBuffer,
                      uint16 dataToReadBufferSize,
                      bool   sendWriteStop,
                      uint8* receiveDataBuffer,
                      uint16 receiveDataBufferSize,
                      bool   sendReadStop );
STATIC _Bool i2cWrite(uint8  i2cNumber,
                      uint8  i2cSlaveAddress,
                      uint8* transmitDataBuffer,
                      uint16 transmitDataBufferSize,
                      bool   sendWriteStop );
//***************************************************************************
// TRAMA I2C A EJECUTAR
//***************************************************************************
void HalI2CProc(void)
{
  uint8 valor=0;
  uint8 address=0x55;
  uint8 buffer[2]={0xfe,0x03};
  
  hali2cInit();
  
  i2cRead(0,address,buffer,2,1,&valor,1,1);
  
  //i2cWrite();
}
//***************************************************************************
// FUNCION i2cRead PARA NFC
//
// ENTRADA DE LA FUNCION
// i2cNumber: Puerto I2C (si hay 1, porner 0).
// i2cSlaveAddress: Direccion del esclavo.
// dataToReadBuffer: Vector de direccion, sub direccion, a leer
// dataToReadBufferSize: Tamaño del vector
// sendWriteStop: True (1) si mando stop.
// receiveDataBuffer: Donde recivo los datos.
// receiveDataBufferSize: Tamaño del vector.
// sendReadStop: True (1) si mando stop.
//
// SALIDA DE LA FUNCION
// retVal: True si salio bien.
//***************************************************************************
STATIC _Bool i2cRead(uint8  i2cNumber,
                     uint8  i2cSlaveAddress,
                     uint8* dataToReadBuffer,
                     uint16 dataToReadBufferSize,
                     bool   sendWriteStop,
                     uint8* receiveDataBuffer,
                     uint16 receiveDataBufferSize,
                     bool   sendReadStop)
{
   bool retVal = TRUE;
   uint16 i = 0;
   // Check Errors 
   if((dataToReadBuffer == NULL)||(dataToReadBufferSize < 0)|| 
      (receiveDataBuffer == NULL)||(receiveDataBufferSize <= 0)){
      return FALSE;   
   }
   // First Write
   // Start condition
   hali2cStart();        
   // 7 bit address + Write = 0
   hali2cWriteByteAdd(i2cSlaveAddress);
   hali2cWait(1);
   // Write all data buffer
   for(i=0; i<dataToReadBufferSize; i++){ 
      hali2cWriteByte(dataToReadBuffer[i]); 
      hali2cWait(1);
   }
   // Send Stop condition
   if(sendWriteStop){
      hali2cStop();       
   }
   // Then Read
   // Start condition
   hali2cStart();        
   // 7 bit address + Read = 1
   hali2cReadByteAdd(i2cSlaveAddress);
   hali2cWait(1);
   // Write all data buffer
   for(i=0; i<receiveDataBufferSize; i++){
      receiveDataBuffer[i] = hali2cReadByte();
      hali2cWait(1);
   }
   // Send Stop condition
   if(sendReadStop){
      hali2cStop();       
   }
   return retVal;
}
//***************************************************************************
// FUNCION i2cWrite PARA NFC
//
// ENTRADA DE LA FUNCION
// i2cNumber: Puerto I2C (si hay 1, porner 0).
// i2cSlaveAddress: Direccion del esclavo.
// transmitDataBuffer: Vector a transmitir.
// transmitDataBufferSize: Tamaño del vector.
// sendWriteStop: True (1) si mando stop.
//
// SALIDA DE LA FUNCION
// retVal: True si salio bien.
//***************************************************************************
STATIC _Bool i2cWrite(uint8  i2cNumber,
                      uint8  i2cSlaveAddress,
                      uint8* transmitDataBuffer,
                      uint16 transmitDataBufferSize,
                      bool   sendWriteStop)
{
   bool retVal = TRUE;
   uint16 i = 0;
   // Check Errors 
   if((transmitDataBuffer == NULL)||(transmitDataBufferSize <= 0)){
      return FALSE;   
   }
   // Start condition
   hali2cStart();        
   // 7 bit address + Write = 0
   hali2cWriteByteAdd(i2cSlaveAddress);
   // Write all data buffer
   for(i=0; i<transmitDataBufferSize; i++){ 
      hali2cWriteByte(transmitDataBuffer[i]);   
   }
   // Send Stop condition
   if(sendWriteStop){
      hali2cStop();       
   }
   return retVal; 
}
//***************************************************************************
// INICIALIZA PUERTOS Y REALIZA TRAMA
//***************************************************************************
void hali2cInit(void)
{
  if (!s_xmemIsInit)            
  {
    s_xmemIsInit = 1;
// Seteo de pines como ENTRADA
      IO_DIR_PORT_PIN( OCM_CLK_PORT, OCM_CLK_PIN, IO_IN );
      IO_DIR_PORT_PIN( OCM_DATA_PORT, OCM_DATA_PIN, IO_IN );
// Seteo de pines como GPIO
      IO_FUNC_PORT_PIN( OCM_CLK_PORT, OCM_CLK_PIN, IO_GIO );
      IO_FUNC_PORT_PIN( OCM_DATA_PORT, OCM_DATA_PIN, IO_GIO );
// Seteo de GPIO para pull-up/pull-down
//    IO_IMODE_PORT_PIN( OCM_CLK_PORT, OCM_CLK_PIN, IO_PUD );
//    IO_IMODE_PORT_PIN( OCM_DATA_PORT, OCM_DATA_PIN, IO_PUD );
// Seteo de pines con pull-up
//    IO_PUD_PORT( OCM_CLK_PORT, IO_PUP );
//    IO_PUD_PORT( OCM_DATA_PORT, IO_PUP );
  }
}
//***************************************************************************
// ESCRIBE BYTE DE DATO PARA ESCRITURA [WRITE BYTE]
//***************************************************************************
STATIC _Bool hali2cWriteByte(uint8 dByte)
{
  uint8 i;
  for (i=0;i<8;i++)             //Lazo para enviar el dato serial
  {
    hali2cClock(0);             //Clock (SCL) LOW
    hali2cWait(1);              //Demora 1 Ciclo
    if (dByte & 0x80)           //Mascara del dato contra 1000 0000
    {
      OCM_DATA_HIGH();          //Data (SDA) HIGH
    }
    else
    {
      OCM_DATA_LOW();           //Data (SDA) LOW
    }
    hali2cClock(1);             //Clock (SCL) HIGH
    hali2cWait(1);              //Demora 1 Ciclo
    dByte <<= 1;                //Shift a la izquierda
  }
  hali2cWait(2);                //Demora 2 Ciclos
  hali2cClock(0);               //Clock (SCL) LOW
  hali2cWait(2);                //Demora 2 Ciclos
  hali2cClock(1);               //Clock (SCL) HIGH
  OCM_SDA;                      //Lee Data (SDA)
  hali2cWait(3);                //Demora 3 Ciclos
  hali2cClock(0);               //Clock (SCL) LOW
  hali2cWait(8);                //Demora 8 Ciclos
  return (!OCM_SDA);            
}
//***************************************************************************
// ESCRIBE BYTE DE DIRECCION PARA ESCRITURA [WRITE ADDRESS]
//***************************************************************************
STATIC _Bool hali2cWriteByteAdd(uint8 dByte)
{
  uint8 i;
  for (i=0;i<8;i++)             //Lazo para enviar el dato serial
  {
    hali2cClock(0);             //Clock (SCL) LOW
    hali2cWait(1);              //Demora 1 Ciclo
    if (dByte & 0x40)           //Mascara del dato contra 0100 0000
    {
      OCM_DATA_HIGH();          //Data (SDA) HIGH
    }
    else
    {
      OCM_DATA_LOW();           //Data (SDA) LOW
    }
    hali2cClock(1);             //Clock (SCL) HIGH
    hali2cWait(1);              //Demora 1 Ciclo
    dByte <<= 1;                //Shift a la izquierda
  }
  hali2cWait(2);                //Demora 2 Ciclos
  hali2cClock(0);               //Clock (SCL) LOW
  hali2cWait(2);                //Demora 2 Ciclos
  hali2cClock(1);               //Clock (SCL) HIGH
  OCM_SDA;                      //Lee Data (SDA)
  hali2cWait(3);                //Demora 3 Ciclos
  hali2cClock(0);               //Clock (SCL) LOW
  hali2cWait(8);                //Demora 8 Ciclos
  return (!OCM_SDA);
}
//***************************************************************************
// ESCRIBE BYTE DE DIRECCION PARA LECTURA [READ BYTE]
//***************************************************************************
STATIC _Bool hali2cReadByteAdd(uint8 dByte)
{
  uint8 i;
  for (i=0;i<7;i++)             //Lazo para enviar el dato serial
  {
    hali2cClock(0);             //Clock (SCL) LOW
    hali2cWait(1);              //Demora 1 Ciclo
    if (dByte & 0x40)           //Mascara del dato contra 0100 0000
    {
      OCM_DATA_HIGH();          //Data (SDA) HIGH
    }
    else
    {
      OCM_DATA_LOW();           //Data (SDA) LOW
    }
    hali2cClock(1);             //Clock (SCL) HIGH
    hali2cWait(1);              //Demora 1 Ciclo
    dByte <<= 1;                //Shift a la izquierda
  }
  hali2cClock(0);               //Clock (SCL) LOW
  hali2cWait(1);                //Demora 1 Ciclo
  OCM_DATA_HIGH();              //Data (SDA) HIGH
  hali2cClock(1);               //Clock (SCL) HIGH
  hali2cWait(1);                //Demora 1 Ciclo
  dByte <<= 1;                  //Shift a la izquierda
  hali2cWait(2);                //Demora 2 Ciclos
  hali2cClock(0);               //Clock (SCL) LOW
  hali2cWait(2);                //Demora 2 Ciclos
  hali2cClock(1);               //Clock (SCL) HIGH
  OCM_SDA;                      //Lee Data (SDA)
  hali2cWait(3);                //Demora 3 Ciclos
  hali2cClock(0);               //Clock (SCL) LOW
  hali2cWait(8);                //Demora 8 Ciclos
  return (!OCM_SDA);
}
//***************************************************************************
// CAMBIA DE ESTADO EL CLOCK [SCL]
//***************************************************************************
STATIC void hali2cClock( bool dir )
{
  if ( dir )
  {
    IO_DIR_PORT_PIN( OCM_CLK_PORT, OCM_CLK_PIN, IO_IN );
    while(!OCM_SCL)             //Espera hasta que el clock este en alto
        hali2cWait(1);          //Demora 1 Ciclo
  }
  else
  {
    IO_DIR_PORT_PIN( OCM_CLK_PORT, OCM_CLK_PIN, IO_OUT );
    OCM_SCL = 0;                //Setea el clock a LOW
  }
  hali2cWait(1);                //Demora 1 Ciclo
}
//***************************************************************************
// DEMORA DE PROCESO [DELAY]
//***************************************************************************
STATIC __near_func void hali2cWait( uint8 count )
{
    while ( count-- );          //15us por iteracion
      asm("NOP");               //Demora NOP de ensamblador
}
//***************************************************************************
// RECIVE BYTE DE DATOS [READ]
//***************************************************************************
STATIC uint8 hali2cReadByte()
{
  uint8 *buffer;                //Variable de salida de la funcion
  int8 i;                       
  for (i=7; i>=0; --i)          //Iteracion de lectura serie
  {
    hali2cClock(0);             //Clock (SCL) LOW
    hali2cWait(1);              //Demora 1 Ciclo
    hali2cClock(1);             //Clock (SCL) HIGH
    hali2cWait(1);              //Demora 1 Ciclo
    if (OCM_SDA)                //Consulta el bit presente en Data
    {
       buffer[i] |= 1<<i;       //Shift bajada del valor a la variable
    }
  }
  return *buffer;
}
//***************************************************************************
// START
// SCL ------___
// SDA ---______
//***************************************************************************
STATIC void hali2cStart(void)
{
  OCM_DATA_HIGH();              //Data (SDA) HIGH
  hali2cClock(1);               //Clock (SCL) HIGH
  hali2cWait(1);                //Demora 1 Ciclo
  OCM_DATA_LOW();               //Data (SDA) LOW
  hali2cWait(1);                //Demora 1 Ciclo
  hali2cClock(0);               //Clock (SCL) LOW
  hali2cWait(1);                //Demora 1 Ciclo
}
//***************************************************************************
// STOP
// SCL ___------
// SDA ______---
//***************************************************************************
STATIC void hali2cStop(void)
{
  OCM_DATA_LOW();               //Data (SDA) LOW
  hali2cClock(0);               //Clock (SCL) LOW
  hali2cWait(1);                //Demora 1 Ciclo
  hali2cClock(1);               //Clock (SCL) HIGH
  hali2cWait(1);                //Demora 1 Ciclo
  OCM_DATA_HIGH();              //Data (SDA) HIGH
  hali2cWait(1);                //Demora 1 Ciclo
}
//***************************************************************************
// FIN DE LA FUNCION I2C
//***************************************************************************