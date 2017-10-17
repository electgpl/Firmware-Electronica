; *****************************************************************************
; *    BOOTLOADER PARA PIC16F883 BASADO EN AN1310 DE MICROCHIP 
; *    MODIFICACION DE PERFORMANCE DE MEMORIA POR CACCAVALLO SEBASTIAN 
; *    FECHA DE MODIFICACION - 10/02/2016 - Electgpl
; *****************************************************************************
#include "devices.inc"
#include "bankswitch.inc"
#include "bootconfig.inc"
#if BOOTLOADERSIZE < ERASE_FLASH_BLOCKSIZE
    #define BOOTBLOCKSIZE ERASE_FLASH_BLOCKSIZE
    #ifndef BOOTLOADER_ADDRESS
        #ifdef CONFIG_AS_FLASH
            #define BOOTLOADER_ADDRESS  (END_FLASH - BOOTBLOCKSIZE - ERASE_FLASH_BLOCKSIZE)
        #else
            #define BOOTLOADER_ADDRESS  (END_FLASH - BOOTBLOCKSIZE)
        #endif
    #endif
#else
    #if (BOOTLOADERSIZE % ERASE_FLASH_BLOCKSIZE) == 0
        #define BOOTBLOCKSIZE BOOTLOADERSIZE     
    #else
        #define BOOTBLOCKSIZE (BOOTLOADERSIZE / ERASE_FLASH_BLOCKSIZE + 1) * ERASE_FLASH_BLOCKSIZE
    #endif
    #ifndef BOOTLOADER_ADDRESS
        #define BOOTLOADER_ADDRESS  (END_FLASH - BOOTBLOCKSIZE)
    #endif
#endif
#ifndef AppVector 
    #define AppVector (BootloaderStart-.5)
#endif
; *****************************************************************************
#define STX             0x0F
#define ETX             0x04
#define DLE             0x05
#define NTX             0xFF
; *****************************************************************************
CRCL                equ 0xA0 
CRCH                equ 0xA1
RXDATA              equ 0xA2
TXDATA              equ 0xA3
COMMAND             equ 0x20 
ADDRESS_L           equ 0x21
ADDRESS_H           equ 0x22
ADDRESS_U           equ 0x23
ADDRESS_X           equ 0x24
DATA_COUNTL         equ 0x25
PACKET_DATA         equ 0x26
DATA_COUNTH         equ 0x26 
#if BOOTLOADER_ADDRESS == 0
#ifndef BSR
PCLATH_TEMP	        equ	0x7E	
W_TEMP		        equ	0x7F
#endif
#endif
; *****************************************************************************
#ifdef PORTC
    #define PORTRX  PORTC
    #define RXPIN   7

    #define TRISTX  TRISC
    #define TXPIN   6
#else
    #define PORTRX  PORTB
    #define RXPIN   2

    #define TRISTX  TRISB
    #define TXPIN   5
#endif
    errorlevel -302
; *****************************************************************************
#if BOOTLOADER_ADDRESS != 0
    ORG     0
        errorlevel -306
        errorlevel +306
    ORG     BOOTLOADER_ADDRESS
BootloaderStart:
    movlw   high(BootloadMode)
    movwf   PCLATH
    goto    BootloadMode
; *****************************************************************************
BootloaderBreakCheck:
    movlw   high(AppVector)
    movwf   PCLATH
#ifdef INVERT_UART
    btfss   PORTRX, RXPIN
    goto    AppVector

    btfsc   PORTRX, RXPIN
    goto    $-1
#else
    btfsc   PORTRX, RXPIN 
    goto    AppVector

    btfss   PORTRX, RXPIN
    goto    $-1
#endif
#else 
    ORG     0
BootloaderStart:
    nop
    BXtoB0
    goto    BootloaderBreakCheck

    ORG     0x0004
InterruptVector:
#ifndef BSR
	movwf	W_TEMP
	swapf	PCLATH, W
	movwf	PCLATH_TEMP
#endif
    movlw   high(AppIntVector) 
    movwf   PCLATH               
    goto    AppIntVector         
BootloaderBreakCheck:
#ifdef INVERT_UART
    btfsc   PORTRX, RXPIN        
    goto    WaitForRxIdle        
#else
    btfss   PORTRX, RXPIN        
    goto    WaitForRxIdle        
#endif
    banksel EEADR               
    movlw   low(AppVector)      
    movwf   EEADR               
    movlw   high(AppVector)
    movwf   EEADRH              
    movwf   PCLATH              
    call    ReadFlashWord       
    addlw   .1
    btfss   STATUS, Z           
    goto    AppVector           

    movlw   0x3F
    xorwf   FSR, w              
    btfss   STATUS, Z           
    goto    AppVector

    movlw   high(BootloadMode)
    movwf   PCLATH              
#ifdef INVERT_UART
WaitForRxIdle:
    btfsc   PORTRX, RXPIN         
    goto    WaitForRxIdle
#else
WaitForRxIdle:
    btfss   PORTRX, RXPIN         
    goto    WaitForRxIdle
#endif
#endif
BootloadMode:
#ifdef BRG16
    movlw   b'00110000'    
    movwf   T1CON          
#endif
#ifdef BSR
    banksel RCSTA
    movlw   b'10010000'    
    movwf   RCSTA          
    movlw   b'00100110'    
    movwf   TXSTA          
    banksel OPTION_REG
#else
    movlw   b'10010000'    
    movwf   RCSTA           
    B0toB1                  
    movlw   b'00100110'     
    movwf   TXSTA           
#endif
#ifndef BRG16
    clrwdt                  
    movlw   b'00000011'     
    movwf   OPTION_REG      
#endif
    B1toB3                  
#ifdef INVERT_UART
    bsf     BAUDCTL, RXDTP  
    bsf     BAUDCTL, TXCKP  
#endif
#ifdef BRG16
    bsf     BAUDCTL, BRG16  
#endif
; *****************************************************************************
DoAutoBaud:
#ifdef BSR
    banksel RCSTA
#else
    BXtoB0                       
#endif
    bcf     RCSTA, CREN          
    movf    RCREG, W             
    movf    RCREG, W             
#ifdef BSR
    banksel TMR1H
#endif
RetryAutoBaud:
#ifdef BRG16
    clrf    TMR1H                
    clrf    TMR1L                
    bcf     PIR1, TMR1IF         
#else
    bcf     STATUS, C            
#endif
    call    WaitForRise          
#ifdef BRG16
    bsf     T1CON, TMR1ON        
#else
    clrf    TMR0                 
#endif
    call    WaitForRise          
#ifdef BRG16
    bcf     T1CON, TMR1ON        
    btfsc   PIR1, TMR1IF         
    goto    RetryAutoBaud        

    movf    TMR1L, w              
  #ifdef BSR
    banksel SPBRG
  #else
    B0toB1                        
  #endif
    movwf   SPBRG                 
  #ifdef BSR
    banksel TMR1H
  #else
    B1toB0                        
  #endif
    movf    TMR1H, w              
  #ifdef BSR
    banksel SPBRGH
  #else
    B0toB1                        
  #endif
    movwf   SPBRGH                
#else
    movf    TMR0, w
    movwf   FSR
    rrf     FSR, w          
    nop
    nop
    btfss   STATUS, C       
    addlw   0xFF            

  #ifdef BSR
    banksel SPBRG
  #else
    B0toB1                  
  #endif
    movwf   SPBRG           
#endif
WaitForHostCommand:         
#ifdef BSR
    banksel RCSTA
#else
    B1toB0                  
#endif
    bsf     RCSTA, CREN     
    lfsr    COMMAND         
    call    ReadHostByte    
    xorlw   STX
    bnz     DoAutoBaud      
; *****************************************************************************
StartOfLine:
    movlw   STX                  
    call    SendHostByte         
ReceiveDataLoop:
    call    ReadHostByte         
    xorlw   STX                  
    bz      StartOfLine          
NoSTX:
    movf    INDF, W              
    xorlw   ETX                  
    bz      VerifyPacketCRC      
NoETX:
    movf    INDF, W              
    xorlw   DLE                  
    bnz     AppendDataBuffer
    call    ReadHostByte         
AppendDataBuffer:
    incf    FSR, f               
    goto    ReceiveDataLoop
VerifyPacketCRC:
    B0toB1                       
    decf    FSR, w               
    movwf   TXDATA               
    decf    TXDATA, f            
    lfsr    COMMAND              
    clrf    CRCL                 
    clrf    CRCH                 
VerifyPacketCrcLoop:
    movf    INDF, w              
    call    AddCrcB1             
    incf    FSR, f               
    movf    FSR, w               
    subwf   TXDATA, w            
    bnz     VerifyPacketCrcLoop  
    movf    CRCL, w              
    subwf   INDF, w              
    bnz     DoAutoBaud           
    incf    FSR, f               
    movf    CRCH, w              
    subwf   INDF, w              
    bnz     DoAutoBaud           
; *****************************************************************************
    clrf    CRCL                    
    clrf    CRCH                    
    B1toB0                          
    movf    ADDRESS_H, W            
    movwf   FSR                     
    movf    ADDRESS_L, W            
    banksel EEADR                   
    movwf   EEADR                   
    movf    FSR, w                  
    movwf   EEADRH                  
    lfsr    PACKET_DATA             
    BXtoB0                          
; *****************************************************************************
CheckCommand:
    movlw   (JUMPTABLE_END - JUMPTABLE_BEGIN)
    subwf   COMMAND, w              
    bc      DoAutoBaud              
    movf    COMMAND, W              
#if ($ & 0xFF) > (0xFF - .10)
    messg   "Asegurando la tabla para no perder espacio de indexado."
    ORG     $+(0x100 - ($ & 0xFF))
#endif
    addwf   PCL, F                 
JUMPTABLE_BEGIN:
    goto    BootloaderInfo         
    goto    ReadFlash              
    goto    VerifyFlash            
    goto    EraseFlash             
    goto    WriteFlash             
    goto    ReadEeprom             
    goto    WriteEeprom            
    goto    SendAcknowledge        
    nop                            
    movlw   high(AppVector)        
JUMPTABLE_END:
    movwf   PCLATH                 
    goto    AppVector              
#if (JUMPTABLE_BEGIN & 0xFF) > (JUMPTABLE_END & 0xFF)
    error "La tabla de saltos excede las 256 Bytes de direcciones."
#endif
WaitForRise:                     
    clrwdt
WaitForRiseLoop:                 
#ifdef BRG16
    btfsc   PIR1, TMR1IF         
    return                       
#endif
    btfsc   PORTRX, RXPIN        
    goto    WaitForRiseLoop      

WtSR:
    btfss   PORTRX, RXPIN        
    goto    WtSR                 
    return
AddCrc:                            
    B0toB1                         
AddCrcB1:
    xorwf   CRCH, w                
    movwf   RXDATA                 
    movf    CRCL, w                
    movwf   CRCH                   
    movf    RXDATA, w              
    movwf   CRCL                   
    swapf   CRCL, w                
    andlw   0x0F                   
    xorwf   CRCL, f                
    swapf   CRCL, w                
    andlw   0xF0                   
    xorwf   CRCH, f                
    swapf   CRCL, f                
    bcf     STATUS, C              
    rlf     CRCL, w                
    btfsc   STATUS, C              
    addlw   .1
    xorwf   CRCH, f                
    andlw   b'11100000'            
    xorwf   CRCH, f                
    swapf   CRCL, f                
    xorwf   CRCL, f                
    return
; *****************************************************************************
BootInfoBlock:
    db      high(BOOTBLOCKSIZE), low(BOOTBLOCKSIZE)
    db      MINOR_VERSION, MAJOR_VERSION
#ifdef FREE
    db      0x02, 0x01            
#else
    db      0x02, 0x00            
#endif
    db      high(BootloaderStart), low(BootloaderStart)
    db      0, upper(BootloaderStart)
    db      high(DEVICEID), low(DEVICEID)
BootInfoBlockEnd:
BootloaderInfo:                    
    movlw   (BootInfoBlockEnd - BootInfoBlock)
    movwf   DATA_COUNTL           
    clrf    DATA_COUNTH           
    banksel EEADR
    movlw   low(BootInfoBlock)
    movwf   EEADR                 
    movlw   high(BootInfoBlock)   
    movwf   EEADRH                
ReadFlash:                         
    call    ReadFlashWord          
    call    SendEscapeByte         
    call    AddCrcB1               
    movf    FSR, w                 
    call    SendEscapeByte         
    call    AddCrcB1               
    banksel EEADR                  
    incf    EEADR, f               
    btfsc   STATUS, Z              
    incf    EEADRH,F               
    
    BXtoB0                         
    movlw   1
    subwf   DATA_COUNTL, f         
    btfss   STATUS, C
    decf    DATA_COUNTH, f         

    movf    DATA_COUNTH, w       
    iorwf   DATA_COUNTL, w       
    bnz     ReadFlash            
    goto    SendChecksum         
ReadFlashWord:                   
    banksel EECON1               
    bsf     EECON1, EEPGD        
    bsf     EECON1, RD           
    nop                          
    nop                          
    banksel EEDATA               
    movf    EEDATH, w            
    movwf   FSR                  
    movf    EEDATA, w            
    BXtoB0                       
    return
VerifyFlash:                       
    call    ReadFlashWord          
    call    AddCrc                 
    movf    FSR, w                 
    call    AddCrcB1               
    banksel EEADR                  
    incf    EEADR, f               
    btfsc   STATUS, Z              
    incf    EEADRH,F               

    movf    EEADR, w               
    andlw   (ERASE_FLASH_BLOCKSIZE-1)
    bnz     VerifyFlash           
    call    SendCRCWord           
    BXtoB0                        
    movlw   1
    subwf   DATA_COUNTL, f        
    btfss   STATUS, C             
    decf    DATA_COUNTH, f        

    movf    DATA_COUNTH, w        
    iorwf   DATA_COUNTL, w        
    bnz     VerifyFlash           
    goto    SendETX               
#ifdef FREE
#ifdef USE_SOFTBOOTWP
    goto    BootloaderStart       
    goto    BootloaderStart       
#endif
EraseFlash:                       
#ifdef USE_SOFTBOOTWP
  #define ERASE_ADDRESS_MASK  ( (~(ERASE_FLASH_BLOCKSIZE-1)) & (END_FLASH-1) )
    banksel EEADR
  #if high(ERASE_ADDRESS_MASK) != 0xFF
    movlw   high(ERASE_ADDRESS_MASK)   
    andwf   EEADRH, f
  #endif
  #if low(ERASE_ADDRESS_MASK) != 0xFF
    movlw   low(ERASE_ADDRESS_MASK)    
    andwf   EEADR, f
  #endif
  #if BOOTLOADER_ADDRESS != 0
    movlw   high(BOOTLOADER_ADDRESS)
    subwf   EEADRH, w
    movlw   low(BOOTLOADER_ADDRESS)
    btfsc   STATUS, Z
    subwf   EEADR, w
    btfss   STATUS, C
    goto    EraseAddressOkay   
  #endif
    movlw   high(BOOTLOADER_ADDRESS + BOOTBLOCKSIZE)
    subwf   EEADRH, w
    movlw   low(BOOTLOADER_ADDRESS + BOOTBLOCKSIZE)
    btfsc   STATUS, Z
    subwf   EEADR, w
    btfsc   STATUS, C
    goto    EraseAddressOkay   
    banksel EECON1             
    clrf    EECON1             
    goto    NextEraseBlock     
    goto    BootloaderStart    
    goto    BootloaderStart    
#endif
EraseAddressOkay:
    banksel EECON1             
    movlw   b'10010100'        
    movwf   EECON1             
    call    StartWriteB3       
NextEraseBlock:
    banksel EEADR                  
#if ERASE_FLASH_BLOCKSIZE >= .256
    movlw   high(ERASE_FLASH_BLOCKSIZE)
    subwf   EEADRH, F              
#else
    movlw   ERASE_FLASH_BLOCKSIZE
    subwf   EEADR, F               
    btfss   STATUS, C
    decf    EEADRH, f              
#endif
    banksel DATA_COUNTL            
    decfsz  DATA_COUNTL, F         
    goto    EraseFlash    
    goto    SendAcknowledge        
#endif
#ifdef USE_SOFTBOOTWP
    goto    BootloaderStart        
    goto    BootloaderStart        
#endif
WriteFlash:
#ifdef USE_SOFTBOOTWP
  #define WRITE_ADDRESS_MASK  ( (~(WRITE_FLASH_BLOCKSIZE-1)) & (END_FLASH-1) )
    banksel EEADR
  #if high(WRITE_ADDRESS_MASK) != 0xFF
    movlw   high(WRITE_ADDRESS_MASK)   
    andwf   EEADRH, f
  #endif
  #if low(WRITE_ADDRESS_MASK) != 0xFF
    movlw   low(WRITE_ADDRESS_MASK)    
    andwf   EEADR, f
  #endif
  #if BOOTLOADER_ADDRESS != 0
    movlw   high(BOOTLOADER_ADDRESS)
    subwf   EEADRH, w
    movlw   low(BOOTLOADER_ADDRESS)
    btfsc   STATUS, Z
    subwf   EEADR, w
    btfss   STATUS, C
    goto    WriteAddressOkay  
  #endif
    movlw   high(BOOTLOADER_ADDRESS + BOOTBLOCKSIZE)
    subwf   EEADRH, w
    movlw   low(BOOTLOADER_ADDRESS + BOOTBLOCKSIZE)
    btfsc   STATUS, Z
    subwf   EEADR, w
    btfsc   STATUS, C
    goto    WriteAddressOkay    

    banksel EECON1              
    clrf    EECON1              
    goto    LoadHoldingRegisters
    goto    BootloaderStart     
    goto    BootloaderStart     
#endif
WriteAddressOkay:
    banksel EECON1              
    movlw   b'10000100'         
    movwf   EECON1              
LoadHoldingRegisters:
    banksel EEDATA              
    movf    INDF, w             
    incf    FSR, f               
    movwf   EEDATA               
    movf    INDF, w              
    incf    FSR, f               
    movwf   EEDATH               
    call    StartWrite           
    banksel EEADR
    incf    EEADR, f             
    btfsc   STATUS, Z            
    incf    EEADRH, f            

    movlw   (WRITE_FLASH_BLOCKSIZE-1)
    andwf   EEADR, w               
    bnz     LoadHoldingRegisters   
    B2toB0                         
    decfsz  DATA_COUNTL, F         
    goto    WriteFlash             
    goto    SendAcknowledge        
ReadEeprom:                        
    banksel EECON1                 
    clrf    EECON1                 
ReadEepromLoop:
    bsf     EECON1, RD             
    btfsc   EECON1, RD
    goto    $-1                    
    banksel EEDATA                 
    movf    EEDATA, w              
    banksel EEADR
    incf    EEADR, f               
    btfsc   STATUS, Z              
    incf    EEADRH, f              

    call    SendEscapeByte         
    call    AddCrcB1               
    BXtoB0                         
    decfsz  DATA_COUNTL, F         
    goto    ReadEeprom             
    goto    SendChecksum           
WriteEeprom:                      
    incf    FSR, f                
    movf    INDF, w               
    banksel EEDATA                
    movwf   EEDATA                
    B2toB3                        
    movlw   b'00000100'           
    movwf   EECON1                
    call    StartWriteB3          
    btfsc   EECON1, WR            
    goto    $-1

    banksel EEADR
    incf    EEADR, F              
    B2toB0                        
    decfsz  DATA_COUNTL, f        
    goto    WriteEeprom           
    goto    SendAcknowledge       
#ifndef FREE
EraseFlash:                       
#endif
SendAcknowledge:                  
    movf    COMMAND, w            
    call    SendEscapeByte        
    call    AddCrcB1              
SendChecksum:                     
    call    SendCRCWord           
SendETX:                          
    BXtoB0                        
    movlw   ETX                   
    call    SendHostByte          
    goto    WaitForHostCommand    
; *****************************************************************************
SendCRCWord:                      
    banksel CRCL
    movf    CRCL, w               
    call    SendEscapeByte        
    movf    CRCH, w               
SendEscapeByte:                   
    banksel TXDATA
    movwf   TXDATA                
    xorlw   STX                   
    bz      WrDLE                 
    movf    TXDATA, W             
    xorlw   ETX                   
    bz      WrDLE                 
    movf    TXDATA, W             
    xorlw   DLE                   
    bnz     WrNext                
WrDLE:
    movlw   DLE                   
    call    SendHostByte          
WrNext:
    movf    TXDATA, W            
SendHostByte:                    
    B1toB0                       
    clrwdt
    btfss   PIR1, TXIF           
    goto    $-1
    
    banksel TXREG
    movwf   TXREG                
    B0toB1                       
    return
; *****************************************************************************
ReadHostByte:                    
    BXtoB0                       
    clrwdt
    btfss   PIR1, RCIF           
    goto    $-1   
#ifdef BSR
    banksel RCREG
#endif
    movf    RCREG, W             
#ifdef BSR
    movlb   .0
#endif
    movwf   INDF                
    return
#ifdef USE_SOFTBOOTWP
    goto    BootloaderStart     
    goto    BootloaderStart     
#endif
StartWrite:                     
    B2toB3                      
StartWriteB3:
    clrwdt                      
    movlw   0x55                
    movwf   EECON2              
    movlw   0xAA                
    movwf   EECON2              
    bsf     EECON1, WR          
    nop                         
    nop                         
    return                      
; *****************************************************************************
    END