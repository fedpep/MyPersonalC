/**
* @file    SDK_BasicPktTest_B.c
* @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
* @version V3.0.0
* @date    August 7, 2012
* @brief   Example of SPIRIT Basic packet test (Rx).
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
#include "SDK_EVAL_VC_General.h"
#include "SDK_BasicPktTest_Common.h"
#include "SPIRIT_Management.h"

/**
* @addtogroup SDK_Examples
* @{
*/

/**
* @addtogroup SDK_BasicPktTest                                 SDK BasicPktTest
* @{
*/

/**
* @addtogroup SDK_BasicPktTest_B                               SDK Basic Pkt Test B
* @brief Device B configured as a receiver.
* @details This code is finalized to the computation of a packet error rate.
* It performs a counting of the received packets and then send this value
* to the device A to make it computes the PER.
*
* The user can change the configuration parameters editing the defines at the
* beginning of the file <i>@ref SDK_BasicPktTest_Common.h</i>.
* @{
*/

/**
* @defgroup BasicPktTest_B_Private_TypesDefinitions            Basic Pkt Test B Private TypesDefinitions
* @{
*/

/**
*@}
*/


/**
* @defgroup BasicPktTest_B_Private_Defines                     Basic Pkt Test B Private Defines
* @{
*/

/**
*@}
*/


/**
* @defgroup BasicPktTest_B_Private_Macros                      Basic Pkt Test B Private Macros
* @{
*/

/**
*@}
*/

/**
* @defgroup BasicPktTest_B_Private_Variables                   Basic Pkt Test B Private Variables
* @{
*/


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


/**
* @brief GPIO structure fitting
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
uint8_t vectcRxBuff[96], cRxData;


/**
* @brief Test start and stop buffer declaration
*/
uint8_t vectcTestStartBuff[20]={'S','T','A','R','T',' ','T','E','S','T','S','T','A','R','T',' ','T','E','S','T'},
vectcTestStopBuff[20]={'S','T','O','P',' ','T','E','S','T',0,0,0,0,0,0,0,0,0,0,0};


/**
* @brief Variable used to store the RSSI value
*/
uint8_t cRssiValue;


/**
* @brief Variables used to compute the packet error rate
*/
uint32_t lRxCount,lTxCount;

/**
*@}
*/

/**
* @defgroup BasicPktTest_B_Private_FunctionPrototypes                          Basic Pkt Test B Private FunctionPrototypes
* @{
*/

/**
*@}
*/


/**
* @defgroup BasicPktTest_B_Private_Functions                                   Basic Pkt Test B Private Functions
* @{
*/


/**
* @brief  This function handles External interrupt request (associated with Spirit GPIO 3).
* @param  None
* @retval None
*/
void M2S_GPIO_3_EXTI_IRQ_HANDLER(void)
{
  /* Check the flag status of EXTI line */
  if(EXTI_GetITStatus(M2S_GPIO_3_EXTI_LINE)){
    
    /* Get the IRQ status */
    SpiritIrqGetStatus(&xIrqStatus);
    
    /* Check the SPIRIT RX_TIMEOUT IRQ flag */
    if(xIrqStatus.IRQ_RX_TIMEOUT)
    {
      SdkEvalLedToggle(LED1);
      
      cRssiValue=SpiritQiGetRssi();
      
    }
    /* Check the SPIRIT RX_DATA_DISC IRQ flag */
    if(xIrqStatus.IRQ_RX_DATA_DISC)
    {
      SdkEvalLedToggle(LED2);
      
      /* RX command */
    SpiritCmdStrobeRx();
    }
    /* Check the SPIRIT RX_DATA_READY IRQ flag */
    if(xIrqStatus.IRQ_RX_DATA_READY)
    {
      SdkEvalLedToggle(LED3);
      
      /* Get the RX FIFO size */
      cRxData=SpiritLinearFifoReadNumElementsRxFifo();
      
      /* Read the RX FIFO */
      SpiritSpiReadLinearFifo(cRxData, vectcRxBuff);
      
      /* Flush the RX FIFO */
      SpiritCmdStrobeFlushRxFifo();
      
      /*  A simple way to control if the received data sequence is correct (in this case LED5 will toggle) */
      {
        SpiritBool xCorrect=S_TRUE, xStartFlag=S_TRUE, xStopFlag=S_TRUE;
        
        for(uint8_t i=0 ; i<20 ; i++)
        {
          if(vectcRxBuff[i] != i+1)
            xCorrect=S_FALSE;
          
          if(vectcRxBuff[i] != vectcTestStartBuff[i])
            xStartFlag=S_FALSE;
          
          if(i<9 && vectcRxBuff[i] != vectcTestStopBuff[i])
            xStopFlag=S_FALSE;
          else if(i>=9 && i<=12 && xStopFlag)
          {
            if(i==9) lTxCount=0;
            
            /* rebuild the number of Tx packets extracted from the STOP PACKET */
            lTxCount |= (vectcRxBuff[i]<<((i-9)*8));
          }
          
        }
        printf("B data received: [");
        for(uint8_t i=0 ; i<20 ; i++)
          printf("%d ", vectcRxBuff[i]);
        printf("]\r\n");
        
        /* give some messages to the user */
        if(xCorrect)
        {
          lRxCount++;
          SdkEvalLedToggle(LED5);
        }
        
        if(xStartFlag)
        {
          lRxCount=0;
        }
        
        if(xStopFlag)
        {
          printf("TEST FINISHED rx_count=%d, tx_count=%d\n\r%.2f%% successful , %.2f%% failed\n\n\r", lRxCount, lTxCount, (float)(lRxCount)/lTxCount*100.0, (float)(lTxCount-lRxCount)/lTxCount*100.0);
        }
        else
        {
          printf("rx_count=%d  rssi=%.2f\r", lRxCount, SpiritQiGetRssidBm());
        }
      }
      /* RX command */
    SpiritCmdStrobeRx();
    }
    /* Check the SPIRIT VALID_SYNC IRQ flag */
    if(xIrqStatus.IRQ_VALID_SYNC)
    {
      SdkEvalLedToggle(LED4);
    }

    /* Clear the EXTI line flag */
    EXTI_ClearITPendingBit(M2S_GPIO_3_EXTI_LINE);
    
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
  
  /* Micro EXTI config */
  SdkEvalM2SGpioInit(M2S_GPIO_SDN,M2S_MODE_GPIO_OUT);
  
  /* Start SysTick */
  SdkStartSysTick();
  
  /* VC config */
  SdkEvalVCInit();
  while(bDeviceState != CONFIGURED);
  
  /* Give power supply to Spirit */
  SdkEvalPmRfSwitchInit();
  SdkEvalPmRfSwitchToVRf();
  SdkEvalPmI2CInit();
  SdkEvalPmADCInit();
  SdkEvalPmRegulateVRfI(3.3);
  
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
  
  /* Spirit Packet config */
  SpiritPktBasicInit(&xBasicInit);
  SpiritPktBasicAddressesInit(&xAddressInit);
  
  /* Spirit IRQs enable */
  SpiritIrqDeInit(&xIrqStatus);
  SpiritIrq(RX_TIMEOUT,S_ENABLE);
  SpiritIrq(RX_DATA_DISC,S_ENABLE);
  SpiritIrq(RX_DATA_READY,S_ENABLE);
  SpiritIrq(VALID_SYNC,S_ENABLE);
  
  /* payload length config */
  SpiritPktBasicSetPayloadLength(20);
  
  /* enable SQI check */
  SpiritQiSetSqiThreshold(SQI_TH_0);
  SpiritQiSqiCheck(S_ENABLE);
  
  /* set RSSI threshold */
  SpiritQiSetRssiThreshold(RSSI_THRESHOLD);
  
  /* rx timeout config */
  SpiritTimerSetRxTimeoutMs(1000.0);
  SpiritTimerSetRxTimeoutStopCondition(SQI_ABOVE_THRESHOLD);
  
  /* IRQ registers blanking */
  SpiritIrqClearStatus();
  
  /* RX command */
  SpiritCmdStrobeRx();
  
  /* print a "welcome" message */
  printf("DEVICE B (Rx)\n\r");
  
  while (1)
  {
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
