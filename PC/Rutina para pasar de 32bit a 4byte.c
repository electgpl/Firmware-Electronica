//------------------------------------------------------------------
#define HIGH_32(number)   ( (uint8_t)((number&(0xFF000000))>>24) )
#define MIDH_32(number)   ( (uint8_t)((number&(0x00FF0000))>>16) )
#define MIDL_32(number)   ( (uint8_t)((number&(0x0000FF00))>>8)  )
#define LOW_32(number)    ( (uint8_t) (number&(0x000000FF))      )
   
   uint32_t number = 69123456;
   
   vec[0] = HIGH_32(number);
   vec[1] = MIDH_32(number);
   vec[2] = MIDL_32(number);
   vec[3] = LOW_32(number);
//------------------------------------------------------------------
bool_t saveNumberInByteArray( uint64_t number,
                          uint8_t* byteArray,
                          uint8_t  byteArraySize ){
   uint8_t i = 0;
   uint8_t* ptr = (uint8_t*)&number;

   if( byteArraySize > 8 ){
      return FALSE;
   }

   for( i=0; i<byteArraySize; i++ ){
      byteArray[i] = *ptr;
      ptr++;
   }

   return TRUE;
}
//------------------------------------------------------------------
bool_t loadNumberFromByteArray( uint64_t* number,
                            uint8_t*  byteArray,
                            uint8_t   byteArraySize ){
   uint8_t i = 0;
   uint64_t numb = 0;

   if( byteArraySize > 8 ){
      return FALSE;
   }

   for( i=0; i<byteArraySize; i++ ){
      numb |= ((uint64_t)(byteArray[i]) << 8*i);
   }
   *number = numb;

   return TRUE;
}
//------------------------------------------------------------------