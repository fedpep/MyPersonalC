/**
* @file    SDK_WMbusStd_B.c
* @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
* @version V3.0.0
* @date    August 7, 2012
* @brief   Example of reception of WMBUS N packets.
* @details
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*
* <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
*/


/* Includes ------------------------------------------------------------------*/
#include "SDK_EVAL_Config.h"
#include "SPIRIT_Config.h"
#include "stm32l1xx.h"
#include "SPIRIT_Management.h"

#define USE_VCOM

//#define SUBMODE_N

#ifdef USE_VCOM
#include "SDK_EVAL_VC_General.h"
#endif


/**
* @addtogroup SDK_Examples
* @{
*/

/**
* @addtogroup SDK_WMbusStd        SDK WMbus Std
* @{
*/

/**
* @addtogroup SDK_WMbusStd_B              SDK WMbus Std B
* @brief Device B configured as a receiver.
* @details This code explains how to configure a receiver with the
* WMBUS N protocol.
*
* @{
*/

/**
* @defgroup WMbusStd_B_Private_TypesDefinitions           WMbus Std B Private TypesDefinitions
* @{
*/

/**
*@}
*/


/**
* @defgroup WMbusStd_B_Private_Defines                    WMbus Std B Private Defines
* @{
*/

/*  Radio configuration parameters  */
#define XTAL_OFFSET_PPM             0

#define BASE_FREQUENCY              868.3e6

#define CHANNEL_SPACE               20e3
#define CHANNEL_NUMBER              0
#define MODULATION_SELECT           GFSK_BT05
#define DATARATE                    32768
#define FREQ_DEVIATION              50e3
#define BANDWIDTH                   125E3

#ifdef SUBMODE_N

/*  Packet configuration parameters */
#define PREAMBLE_LENGTH             PKT_PREAMBLE_LENGTH_02BYTES
#define SYNC_LENGTH                 PKT_SYNC_LENGTH_2BYTES
#define SYNC_WORD                   0xF68D0000
#define LENGTH_TYPE                 PKT_LENGTH_FIX
#define LENGTH_WIDTH                7
#define CRC_MODE                    PKT_NO_CRC
#define CONTROL_LENGTH              PKT_CONTROL_LENGTH_0BYTES
#define EN_ADDRESS                  S_DISABLE
#define EN_FEC                      S_DISABLE
#define EN_WHITENING                S_DISABLE

/*  Addresses configuration parameters  */
#define EN_FILT_MY_ADDRESS          S_DISABLE
#define MY_ADDRESS                  0x44
#define EN_FILT_MULTICAST_ADDRESS   S_DISABLE
#define MULTICAST_ADDRESS           0xEE
#define EN_FILT_BROADCAST_ADDRESS   S_DISABLE
#define BROADCAST_ADDRESS           0xFF
#define DESTINATION_ADDRESS         0x34

#else

/*  Packet configuration parameters  */
#define MBUS_SUBMODE                    MBUS_SUBMODE_S1_S2_LONG_HEADER
#define PREAMBLE_LENGTH                 36
#define POSTAMBLE_LENGTH                16

#endif

#define CRC_POLYNOM 0x3D65

/**
*@}
*/


/**
* @defgroup WMbusStd_B_Private_Macros                             WMbus Std B Private Macros
* @{
*/


#define CEILING(X) (X-(uint16_t)(X) > 0 ? (uint16_t)(X+1) : (uint16_t)(X))

/**
*@}
*/

/**
* @defgroup WMbusStd_B_Private_Variables                          WMbus Std B Private Variables
* @{
*/

#ifdef SUBMODE_N
/**
* @brief Packet Basic structure fitting
*/
PktBasicInit xBasicInit={
  PREAMBLE_LENGTH,
  SYNC_LENGTH,
  SYNC_WORD,
  LENGTH_TYPE,
  LENGTH_WIDTH,
  CRC_MODE,
  CONTROL_LENGTH,
  EN_ADDRESS,
  EN_FEC,
  EN_WHITENING
};


/**
* @brief Address structure fitting
*/
PktBasicAddressesInit xAddressInit={
  EN_FILT_MY_ADDRESS,
  MY_ADDRESS,
  EN_FILT_MULTICAST_ADDRESS,
  MULTICAST_ADDRESS,
  EN_FILT_BROADCAST_ADDRESS,
  BROADCAST_ADDRESS
};

#else


/**
* @brief Packet Mbus structure fitting
*/
PktMbusInit xMbusInit={
  MBUS_SUBMODE,
  PREAMBLE_LENGTH,
  POSTAMBLE_LENGTH
};
#endif

/**
* @brief GPIO IRQ structure fitting
*/
SGpioInit xGpioIRQ={
  SPIRIT_GPIO_3,
  SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
  SPIRIT_GPIO_DIG_OUT_IRQ
};


/**
* @brief IRQ status struct declaration
*/
SpiritIrqs xIrqStatus;


/**
* @brief Rx buffer declaration: how to store the received data
*/
uint8_t vectcRxBuff[300], cRxDataLen, cLField;

uint16_t nRxData, nPktLength=0;
SpiritFlagStatus xLengthReceived=S_RESET,
xRxDone=S_RESET, xNextLastRx=S_RESET;
SpiritBool xCrcResult = S_TRUE;

/**
* @brief Rx buffer index, residual and total bytes to be received
*/
uint16_t nRxIndex, nResidualPayloadLength, nPayloadLength = 20;

/**
* @brief Almost full Tx FIFO threshold  (measured from the tail)
*/
uint8_t cThreholdRxFifoAF = 48;


uint8_t cAFCounter = 0;
uint8_t vett1[100];
uint8_t vett2[100];

/**
*@}
*/

/**
* @defgroup WMbusStd_B_Private_FunctionPrototypes                         WMbus Std B Private FunctionPrototypes
* @{
*/

/**
*@}
*/


/**
* @defgroup WMbusStd_B_Private_Functions                                  WMbus Std B Private Functions
* @{
*/




/**
* @brief  Calculates the 16-bit CRC. The function requires that the CRC_POLYNOM is defined,
*           which gives the wanted CRC polynom. 
* @param  Input Param:uint8  crcData  - Data to perform the CRC-16 operation on.
*                     uint16 crcReg   - Current or initial value of the CRC calculation
* @retval LINK_STATUS : LINK_STATUS_SUCCESS
*       LINK_STATUS_INVALID_SIZE   - The data-field buffer is too large
*                       
*/
uint16_t crcCalc(uint16_t crcReg, uint8_t crcData) 
{
  uint8_t i;
  
  for (i = 0; i < 8; i++) 
  {
    // If upper most bit is 1
    if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
      crcReg = (crcReg << 1)  ^ CRC_POLYNOM;
    else
      crcReg = (crcReg << 1);
    
    crcData <<= 1;
  }
  
  return crcReg;
}


/**
* @brief  Calculates the 16-bit CRC. 
* @param  Input Param:uint8  crcData  - Data to perform the CRC-16 operation on.
*                     uint16 crcReg   - Current or initial value of the CRC calculation
* @retval LINK_STATUS : LINK_STATUS_SUCCESS
*       LINK_STATUS_INVALID_SIZE   - The data-field buffer is too large
*                       
*/
SpiritBool CRCCheck(uint8_t *pStart, uint8_t *pStop)
{
  uint16_t  seed= 0x0000;
  
  while (pStart != pStop)
  {
    seed = crcCalc(seed, *pStart);
    pStart++;
  }
  seed =~seed;
  if((pStop[0]  == (uint8_t)(seed>>8))&&(pStop[1]  == (uint8_t)(seed)))
  {
    return S_TRUE;
  }
  return S_FALSE;
}


/**
* @brief  This function handles External interrupt request. In this application it is used
*         to manage the Spirit IRQ configured to be notified on the Spirit GPIO_3.
* @param  None
* @retval None
*/
void M2S_GPIO_3_EXTI_IRQ_HANDLER(void)
{
  /* Check the flag status of EXTI line */
  if(EXTI_GetITStatus(M2S_GPIO_3_EXTI_LINE)){
    
    /* Get the IRQ status */
    SpiritIrqGetStatus(&xIrqStatus);
    
    /* Clear the EXTI line flag */
    EXTI_ClearITPendingBit(M2S_GPIO_3_EXTI_LINE);
    
    if(xIrqStatus.IRQ_RX_FIFO_ALMOST_FULL)
    {
      /* First byte reception */
      if(xLengthReceived == S_RESET)
      {
        SdkEvalLedToggle(LED1);
        
        SpiritIrq(RX_FIFO_ALMOST_FULL,S_DISABLE);
        SpiritLinearFifoSetAlmostFullThresholdRx(2);
        
        nRxIndex=0;
        SpiritSpiReadLinearFifo(12, &vectcRxBuff[nRxIndex]);
        cLField = vectcRxBuff[0];
        
        if(cLField>9)
          nPktLength = cLField+2+2*(CEILING(((float)cLField-9.0)/16.0));
        else
          nPktLength = cLField+2;
        
        //nPktLength = cLField; // to debug
        
        nResidualPayloadLength = nPktLength;
        
        nRxIndex=12;
        nResidualPayloadLength-=11;
        
        xCrcResult = CRCCheck(&vectcRxBuff[0],&vectcRxBuff[10]);
        
        if(xCrcResult)
        {
          cRxDataLen = SpiritLinearFifoReadNumElementsRxFifo();
          
          
          if(nResidualPayloadLength+cRxDataLen>94)
          {
            cThreholdRxFifoAF=48;
            SpiritLinearFifoSetAlmostFullThresholdRx(96-cThreholdRxFifoAF);
            xNextLastRx = S_RESET;
          }
          else
          {
            cThreholdRxFifoAF = nResidualPayloadLength+cRxDataLen;
            SpiritLinearFifoSetAlmostFullThresholdRx(96-cThreholdRxFifoAF);
            xNextLastRx = S_SET;
            
          }
          
          SpiritIrq(RX_FIFO_ALMOST_FULL,S_ENABLE);
          
          xLengthReceived = S_SET;
        }
        else
        {
          SpiritIrq(RX_FIFO_ALMOST_FULL,S_DISABLE);
          SpiritIrqClearStatus();
          SpiritCmdStrobeSabort();
        }
        
        
      }
      else
      {
        /* Get the RX FIFO size */
        cRxDataLen = SpiritLinearFifoReadNumElementsRxFifo();
        
        /* ..check */
        if(cRxDataLen >= cThreholdRxFifoAF)
        { 
          
          /* unarm the almost full threshold */
          SpiritIrq(RX_FIFO_ALMOST_FULL,S_DISABLE);
          
          /* store the FIFO length before reading it */
          vett1[cAFCounter] = cRxDataLen;
          
          if(xNextLastRx == S_SET)
          {
            SpiritCmdStrobeSabort();
            
            /* Read the RX FIFO */
            SpiritSpiReadLinearFifo(nResidualPayloadLength, &vectcRxBuff[nRxIndex]);
            
            /* store the FIFO length after reading it */
            cRxDataLen = SpiritLinearFifoReadNumElementsRxFifo();
            vett2[cAFCounter] = cRxDataLen;
            
            /* increment the debug vectors index */
            cAFCounter++;
            
            /* update the number of received bytes */
            nRxIndex += nResidualPayloadLength;
            
            /* update the number of bytes to be received */
            nResidualPayloadLength = 0;
            
            
            
            xRxDone=S_SET;
            
            
          }
          else
          { 
            /* Read the RX FIFO */
            SpiritSpiReadLinearFifo(cRxDataLen-1, &vectcRxBuff[nRxIndex]);
            
            /* update the number of bytes to be received */
            nResidualPayloadLength -= cRxDataLen-1;
            
            /* update the number of received bytes */
            nRxIndex += cRxDataLen-1;
            
            /* store the FIFO length after reading it */
            cRxDataLen = SpiritLinearFifoReadNumElementsRxFifo();
            vett2[cAFCounter] = cRxDataLen;
            
            
            
            /* increment the debug vectors index */
            cAFCounter++;
            
            
            
            if(nResidualPayloadLength+cRxDataLen <= 94)
            {
              cThreholdRxFifoAF = nResidualPayloadLength+cRxDataLen;
              SpiritLinearFifoSetAlmostFullThresholdRx(96-cThreholdRxFifoAF);
              xNextLastRx = S_SET;
              
              
            }
            
            SpiritIrq(RX_FIFO_ALMOST_FULL,S_ENABLE);
          }
        }
        
      }
    }
  }
}


/**
* @brief  System main function.
* @param  None
* @retval None
*/
void main (void)
{
  NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x3000 );
  
  /* Leds config */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
  SdkEvalLedInit(LED3);
  SdkEvalLedInit(LED4);
  SdkEvalLedInit(LED5);
  
  /* Start SysTick */
  SdkStartSysTick();
  
  /* Micro EXTI config */
  SdkEvalM2SGpioInit(M2S_GPIO_SDN,M2S_MODE_GPIO_OUT);
  
#ifdef USE_VCOM
  /* VC config */
  SdkEvalVCInit();
  while(bDeviceState != CONFIGURED);
#endif
  
  /* Spirit ON */
  SdkEvalSpiritExitShutdown();
  
  /* Spirit SPI config */
  SpiritSpiInit();
  
  /* Reset Spirit registers to default */
  SpiritCmdStrobeSres();
  
  /* Compute the Spirit version and XTAL frequency */
  SpiritManagementIdentificationRFBoard();
  
  SdkEvalM2SGpioInit(M2S_GPIO_3,M2S_MODE_EXTI_IN);
  SdkEvalM2SGpioInterruptCmd(M2S_GPIO_3,0x0F,0x0F,ENABLE);
  
  /* Spirit IRQ config */
  SpiritGpioInit(&xGpioIRQ);
  
  /**
  * @brief Radio structure fitting
  */
  SRadioInit xRadioInit = {
    SpiritManagementXtalFrequency(),
    XTAL_OFFSET_PPM,
    BASE_FREQUENCY,
    CHANNEL_SPACE,
    CHANNEL_NUMBER,
    MODULATION_SELECT,
    DATARATE,
    FREQ_DEVIATION,
    BANDWIDTH
  };
  
  /* Spirit Radio config */
  SpiritRadioInit(&xRadioInit);
  SdkEvalLedOn(LED1);
  
  SpiritRadioCsBlanking(S_ENABLE);
  
#ifdef SUBMODE_N
  /* Spirit Packet config */
  SpiritPktBasicInit(&xBasicInit);
  SpiritPktBasicAddressesInit(&xAddressInit);
#else
  /* Spirit Packet config */
  SpiritPktMbusInit(&xMbusInit);
#endif
  
  /* RX timeout config */
  SET_INFINITE_RX_TIMEOUT();
  
  /* Spirit IRQs enable */
  SpiritIrqDeInit(&xIrqStatus);
  
#ifdef SUBMODE_N
  /* payload length config */
  SpiritPktBasicSetPayloadLength(0x0FFF);
#else
  /* payload length config */
  SpiritPktMbusSetPayloadLength(0x0FFF);
#endif
  
  /* enable SQI check */
  SpiritQiSetSqiThreshold(SQI_TH_0);
  SpiritQiSqiCheck(S_ENABLE);
  
  SpiritQiSetRssiThresholddBm(-85);
  
  SpiritTimerSetRxTimeoutStopCondition(RSSI_AND_SQI_ABOVE_THRESHOLD);
  
  /* IRQ registers blanking */
  SpiritIrqClearStatus();
  
  EXTI_ClearFlag(M2S_GPIO_0_EXTI_LINE);
  EXTI_ClearFlag(M2S_GPIO_3_EXTI_LINE);
  
  SdkEvalM2SGpioInterruptCmd(M2S_GPIO_3,0x0F,0x0F,ENABLE);
  SdkEvalM2SGpioInterruptCmd(M2S_GPIO_0,0x0F,0x0F,ENABLE);
  
  /* infinite loop */
  while (1){
    
    nRxData = 0;
    
    xLengthReceived = S_RESET;
    xNextLastRx = S_RESET;
    
    SpiritLinearFifoSetAlmostFullThresholdRx(96-12);
    SpiritIrq(RX_FIFO_ALMOST_FULL,S_ENABLE);
    
    /* RX command */
    SpiritCmdStrobeRx();
    
    while(xRxDone==S_RESET && xCrcResult==S_TRUE)
    
    for(uint16_t i=0 ; i<(nRxIndex-12)/18 ; i++)
    {
      xCrcResult &= CRCCheck(&vectcRxBuff[12+18*i],&vectcRxBuff[12+18*(i+1)-2]);
    }
    
    if((nRxIndex-12)%18!=0)
      xCrcResult &= CRCCheck(&vectcRxBuff[nRxIndex-((nRxIndex-12)%18)],&vectcRxBuff[nRxIndex-2]);
    
    SpiritCmdStrobeFlushRxFifo();
    
    if(xCrcResult)
    {
      SdkEvalLedToggle(LED3);
      
#ifdef USE_VCOM
      /* print the received data */
      printf("\n\rB data received: [");
      
      for(uint16_t i=0 ; i<nRxIndex ; i++)
      {
        printf("%d ", vectcRxBuff[i]);
      }
      printf("]\r\n");
      
      printf("RSSI = %.2f\npayload length= %d\n", SpiritQiGetRssidBm(), nPktLength);
      
      /* print the received data */
      printf("vett1 [");
      
      for(uint16_t i=0 ; i<cAFCounter ; i++)
      {
        printf("%d ", vett1[i]);
      }
      printf("]\n");
      
      printf("vett2 [");
      for(uint16_t i=0 ; i<cAFCounter ; i++)
      {
        printf("%d ", vett2[i]);
      }
      printf("]\r\n");
#endif
    }
    xCrcResult=S_TRUE;
    xRxDone=S_RESET;
    
    cAFCounter=0;
  }
  
}



#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval : None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number */
  printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/**
*@}
*/

/**
*@}
*/

/**
*@}
*/

/**
*@}
*/


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
