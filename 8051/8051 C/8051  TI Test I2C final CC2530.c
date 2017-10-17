/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include "OSAL_Timers.h"

#include "GenericAppTest.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "hal_board.h"
#include "hal_drivers.h"
#include "hal_timer.h"

#include "sapi.h"
#include "sapi_i2c.h"
#include "sapi_peripheral_map.h"
#include "androidAppComunicationTask.h"
#include "ntagI2cPlus.h"


/* RTOS */
#if defined( IAR_ARMCM3_LM )
#include "RTOS_App.h"
#endif  

void HalI2CProc(void);
void delayT( uint8_t count );

void delayT( uint8_t count )
{
    while ( count-- );          //15us por iteracion
      asm( "NOP" );             //Demora NOP de ensamblador
}
//***************************************************************************
// TRAMA I2C A EJECUTAR
// Bits         Funcion
// 7 a 5        000: Tick Frequency / 1
//              001: Tick Frequency / 2
//              010: Tick Frequency / 4
//              011: Tick Frequency / 8
//              100: Tick Frequency / 16
//              101: Tick Frequency / 32
//              110: Tick Frequency / 64
//              111: Tick Frequency / 128
// 4            Start a Nivel alto
// 3            Mascara de Interrupcion por Overflow
// 2            Clear Counter, a 1 = CLR
// 1 a 0        00: Free Running, Repetidamente desde 0x00 a 0xFF
//              01: Down, Cuenta desde T4CC0 a 0x00
//              10: Modulo, Repetidamente desde 0x00 a T4CC0
//              11: Up-Down, Repetidamente desde 0x00 a T4CC0 y Down a 0x00
//***************************************************************************
void HalI2CProc(void)
{
  /*
  static int i=0;
  if (i==0)
    androidAppComunicationTaskInit();
  i=1;
  androidAppComunicationTask();
  */
  
/*  
  T4CTL = 0x00;                 //Stop Timer                    0000 0000
  T4CNT = 0x00;                 //Inicializa el valor           0000 0000
  T4CTL = 0x90;                 //Start Timer, Divide 16        1001 0000
  static uint8 valor=0;
  static uint8 address=0x55;
  static uint8 buffer[2]={0xfe,0x03};
  i2cConfig( I2C0, 57470 );
  while(TRUE){
    buffer[1]=T4CNT;            //Mueve el valor
    i2cRead(I2C0,address,buffer,2,1,&valor,1,1);
  }
  T4CTL = 0x00;                 //Stop Timer                    0000 0000
*/
  
  i2cConfig( I2C0, 57470 );
  while(TRUE){
    OCM_DATA_HIGH();          //Data (SDA) HIGH
    asm( "NOP" );
    OCM_DATA_LOW();           //Data (SDA) LOW
    asm( "NOP" );
    
  }
}

/*********************************************************************
 * GLOBAL VARIABLES
 */

zAddrType_t dstAddr; 

// This list should be filled with Application specific Cluster IDs.
const cId_t GenericAppTest_ClusterList[GenericAppTest_MAX_CLUSTERS] =
{
  GenericAppTest_CLUSTERID
};

const SimpleDescriptionFormat_t GenericAppTest_SimpleDesc =
{
  GenericAppTest_ENDPOINT,              //  int Endpoint;
  GenericAppTest_PROFID,                //  uint16 AppProfId[2];
  GenericAppTest_DEVICEID,              //  uint16 AppDeviceId[2];
  GenericAppTest_DEVICE_VERSION,        //  int   AppDevVer:4;
  GenericAppTest_FLAGS,                 //  int   AppFlags:4;
  GenericAppTest_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericAppTest_ClusterList,  //  byte *pAppInClusterList;
  GenericAppTest_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericAppTest_ClusterList   //  byte *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in GenericAppTest_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t GenericAppTest_epDesc;

/*********************************************************************
 * LOCAL VARIABLES
 */
byte GenericAppTest_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // GenericAppTest_Init() is called.
devStates_t GenericAppTest_NwkState;


byte GenericAppTest_TransID;  // This is the unique message ID (counter)

afAddrType_t GenericAppTest_DstAddr;

unsigned int valor=0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void GenericAppTest_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
static void GenericAppTest_MessageMSGCB( afIncomingMSGPacket_t *pckt );
static void GenericAppTest_SendTheMessage( void );

#if defined( IAR_ARMCM3_LM )
static void GenericAppTest_ProcessRtosMessage( void );
#endif

/*********************************************************************
 * @fn      GenericAppTest_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void GenericAppTest_Init( uint8 task_id )
{
  
  GenericAppTest_TaskID = task_id;
  GenericAppTest_NwkState = DEV_INIT;
  GenericAppTest_TransID = 0;

  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

  GenericAppTest_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  GenericAppTest_DstAddr.endPoint = 0;
  GenericAppTest_DstAddr.addr.shortAddr = 0;

  // Fill out the endpoint description.
  GenericAppTest_epDesc.endPoint = GenericAppTest_ENDPOINT;
  GenericAppTest_epDesc.task_id = &GenericAppTest_TaskID;
  GenericAppTest_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&GenericAppTest_SimpleDesc;
  GenericAppTest_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &GenericAppTest_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( GenericAppTest_TaskID );

  // Update the display
#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "GenericAppTest", HAL_LCD_LINE_1 );
#endif

  ZDO_RegisterForZDOMsg( GenericAppTest_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( GenericAppTest_TaskID, Match_Desc_rsp );

#if defined( IAR_ARMCM3_LM )
  // Register this task with RTOS task initiator
  RTOS_RegisterApp( task_id, GenericAppTest_RTOS_MSG_EVT );
#endif
}

/*********************************************************************
 * @fn      GenericAppTest_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 GenericAppTest_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  afDataConfirm_t *afDataConfirm;

  // Data Confirmation message fields
  byte sentEP;
  ZStatus_t sentStatus;
  byte sentTransID;       // This should match the value sent
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericAppTest_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          GenericAppTest_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
 
          break;

        case AF_DATA_CONFIRM_CMD:
          HalI2CProc();
          if(valor<2)
          {
            HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
            // Initiate an End Device Bind Request for the mandatory endpoint
            dstAddr.addrMode = Addr16Bit;
            dstAddr.addr.shortAddr = 0x0000; // Coordinator
            ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(),
                                  GenericAppTest_epDesc.endPoint,
                                  GenericAppTest_PROFID,
                                  GenericAppTest_MAX_CLUSTERS, (cId_t *)GenericAppTest_ClusterList,
                                  GenericAppTest_MAX_CLUSTERS, (cId_t *)GenericAppTest_ClusterList,
                                  FALSE );
          }
          else
          {
            HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
            // Initiate a Match Description Request (Service Discovery)
            dstAddr.addrMode = AddrBroadcast;
            dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
            ZDP_MatchDescReq( &dstAddr, NWK_BROADCAST_SHORTADDR,
                              GenericAppTest_PROFID,
                              GenericAppTest_MAX_CLUSTERS, (cId_t *)GenericAppTest_ClusterList,
                              GenericAppTest_MAX_CLUSTERS, (cId_t *)GenericAppTest_ClusterList,
                              FALSE ); 
            valor=0;
          }
          valor++;
          // This message is received as a confirmation of a data packet sent.
          // The status is of ZStatus_t type [defined in ZComDef.h]
          // The message fields are defined in AF.h
          afDataConfirm = (afDataConfirm_t *)MSGpkt;
          sentEP = afDataConfirm->endpoint;
          sentStatus = afDataConfirm->hdr.status;
          sentTransID = afDataConfirm->transID;
          (void)sentEP;
          (void)sentTransID;

          // Action taken when confirmation is received.
          if ( sentStatus != ZSuccess )
          {
            // The data wasn't delivered -- Do something
          }
          break;

        case AF_INCOMING_MSG_CMD:
          GenericAppTest_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          GenericAppTest_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (GenericAppTest_NwkState == DEV_ZB_COORD)
              || (GenericAppTest_NwkState == DEV_ROUTER)
              || (GenericAppTest_NwkState == DEV_END_DEVICE) )
          {
            // Start sending "the" message in a regular interval.
            osal_start_timerEx( GenericAppTest_TaskID,
                                GenericAppTest_SEND_MSG_EVT,
                                GenericAppTest_SEND_MSG_TIMEOUT );
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericAppTest_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in GenericAppTest_Init()).
  if ( events & GenericAppTest_SEND_MSG_EVT )
  {
    // Send "the" message
    GenericAppTest_SendTheMessage();

    // Setup to send message again
    osal_start_timerEx( GenericAppTest_TaskID,
                        GenericAppTest_SEND_MSG_EVT,
                        GenericAppTest_SEND_MSG_TIMEOUT );

    // return unprocessed events
    return (events ^ GenericAppTest_SEND_MSG_EVT);
  }

  
#if defined( IAR_ARMCM3_LM )
  // Receive a message from the RTOS queue
  if ( events & GenericAppTest_RTOS_MSG_EVT )
  {
    // Process message from RTOS queue
    GenericAppTest_ProcessRtosMessage();

    // return unprocessed events
    return (events ^ GenericAppTest_RTOS_MSG_EVT);
  }
#endif

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn      GenericAppTest_ProcessZDOMsgs()
 *
 * @brief   Process response messages
 *
 * @param   none
 *
 * @return  none
 */
static void GenericAppTest_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
#if defined( BLINK_LEDS )
      else
      {
        // Flash LED to show failure
        HalLedSet ( HAL_LED_4, HAL_LED_MODE_FLASH );
      }
#endif
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            GenericAppTest_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            GenericAppTest_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            GenericAppTest_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}

/*********************************************************************
 * @fn      GenericAppTest_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
static void GenericAppTest_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId )
  {
    case GenericAppTest_CLUSTERID:
      // "the" message
#if defined( LCD_SUPPORTED )
      HalLcdWriteScreen( (char*)pkt->cmd.Data, "rcvd" );
#elif defined( WIN32 )
      WPRINTSTR( pkt->cmd.Data );
#endif
      break;
  }
}

/*********************************************************************
 * @fn      GenericAppTest_SendTheMessage
 *
 * @brief   Send "the" message.
 *
 * @param   none
 *
 * @return  none
 */
static void GenericAppTest_SendTheMessage( void )
{
  char theMessageData[] = "Enviando I2C";

  if ( AF_DataRequest( &GenericAppTest_DstAddr, &GenericAppTest_epDesc,
                       GenericAppTest_CLUSTERID,
                       (byte)osal_strlen( theMessageData ) + 1,
                       (byte *)&theMessageData,
                       &GenericAppTest_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    // Successfully requested to be sent.
  }
  else
  {
    // Error occurred in request to send.
  }
}

#if defined( IAR_ARMCM3_LM )
/*********************************************************************
 * @fn      GenericAppTest_ProcessRtosMessage
 *
 * @brief   Receive message from RTOS queue, send response back.
 *
 * @param   none
 *
 * @return  none
 */
static void GenericAppTest_ProcessRtosMessage( void )
{
  osalQueue_t inMsg;

  if ( osal_queue_receive( OsalQueue, &inMsg, 0 ) == pdPASS )
  {
    uint8 cmndId = inMsg.cmnd;
    uint32 counter = osal_build_uint32( inMsg.cbuf, 4 );

    switch ( cmndId )
    {
      case CMD_INCR:
        counter += 1;  /* Increment the incoming counter */
                       /* Intentionally fall through next case */

      case CMD_ECHO:
      {
        userQueue_t outMsg;

        outMsg.resp = RSP_CODE | cmndId;  /* Response ID */
        osal_buffer_uint32( outMsg.rbuf, counter );    /* Increment counter */
        osal_queue_send( UserQueue1, &outMsg, 0 );  /* Send back to UserTask */
        break;
      }
      
      default:
        break;  /* Ignore unknown command */    
    }
  }
}
#endif