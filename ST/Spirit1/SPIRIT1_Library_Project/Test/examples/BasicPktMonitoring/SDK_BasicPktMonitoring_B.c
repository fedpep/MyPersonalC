/**
* @file    SDK_BasicPktMonitoring_B.c
* @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
* @version V3.0.0
* @date    August 7, 2012
* @brief   Example of configuration and management of SPIRIT Basic packets.
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
#include <stdlib.h>
#include <string.h>
#include "SDK_EVAL_VC_General.h"
#include "SDK_BasicPktCommon.h"
#include "SPIRIT_Management.h"

#define USE_HIGH_BAND

/**
* @addtogroup SDK_Examples
* @{
*/

/**
* @addtogroup SDK_Basic_packet_monitoring              SDK Basic packet monitoring
* @{
*/

/**
* @addtogroup SDK_Basic_packet_monitoring_B            SDK Basic packet monitoring B
* @brief Example of configuration and management of SPIRIT Basic packets on the receiver side.
* @details This code configures the device B as a receiver of basic packets. Through
* a menu, the user can change the packet parameters during runtime, testing in this way all
* the basic packets features.
* @{
*/

/**
* @defgroup Basic_packet_monitoring_B_Private_TypesDefinitions                 Basic packet monitoring B Private TypesDefinitions
* @{
*/

/**
*@}
*/


/**
* @defgroup Basic_packet_monitoring_B_Private_Defines                          Basic packet monitoring B Private Defines
* @{
*/

/*  Radio configuration parameters  */
#define XTAL_OFFSET_PPM            0

#ifdef USE_VERY_LOW_BAND
#define BASE_FREQUENCY              169.0e6
#endif

#ifdef USE_LOW_BAND
#define BASE_FREQUENCY              315.0e6
#endif

#ifdef USE_MIDDLE_BAND
#define BASE_FREQUENCY              433.0e6
#endif

#ifdef USE_HIGH_BAND
#define BASE_FREQUENCY              868.0e6
#endif

#define CHANNEL_SPACE               20e3
#define CHANNEL_NUMBER              0
#define MODULATION_SELECT           FSK
#define DATARATE                    38400
#define FREQ_DEVIATION              20e3
#define BANDWIDTH                   100.5E3

/*  Packet configuration parameters  */
#define PREAMBLE_LENGTH             PKT_PREAMBLE_LENGTH_08BYTES
#define SYNC_LENGTH                 PKT_SYNC_LENGTH_4BYTES
#define SYNC_WORD                   0x1A2635A8
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

/**
*@}
*/


/**
* @defgroup Basic_packet_monitoring_B_Private_Macros                                   Basic packet monitoring B Private Macros
* @{
*/

/**
*@}
*/

/**
* @defgroup Basic_packet_monitoring_B_Private_Variables                                Basic packet monitoring B Private Variables
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
* @brief Declaration of variable used to manage the V-COM communication
*/
uint8_t vectcVcomData[VIRTUAL_COM_PORT_DATA_SIZE];


/**
*@}
*/

/**
* @defgroup Basic_packet_monitoring_B_Private_FunctionPrototypes                       Basic packet monitoring B Private FunctionPrototypes
* @{
*/

/**
*@}
*/


/**
* @defgroup Basic_packet_monitoring_B_Private_Functions                                Basic packet monitoring B Private Functions
* @{
*/

/**
* @brief  This function handles External lines 15 to 10 interrupt request.
* @param  None
* @retval None
*/
void EXTI15_10_IRQHandler(void)
{
  /* Check the flag status of EXTI line 10 */
  if(EXTI_GetITStatus(EXTI_Line10)){
    
    /* Get the IRQ status */
    SpiritIrqGetStatus(&xIrqStatus);
    
    /* Check the SPIRIT RX_TIMEOUT IRQ flag */
    if(xIrqStatus.IRQ_RX_TIMEOUT)
    {
      SdkEvalLedToggle(LED1);      
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
        SpiritBool correct=S_TRUE;
        
        for(uint8_t i=0 ; i<20 ; i++)
          if(vectcRxBuff[i] != i+1)
            correct=S_FALSE;
        
        if(correct)
          SdkEvalLedToggle(LED5);
      }
      
      printf("B data received: [");
      for(uint8_t i=0 ; i<20 ; i++)
        printf("%d ", vectcRxBuff[i]);
      printf("]\r\n");
      
      /* RX command */
      SpiritCmdStrobeRx();
    }
//    /* Check the SPIRIT VALID_SYNC IRQ flag */
//    if(xIrqStatus.IRQ_VALID_SYNC)
//    {
//      SdkEvalLedToggle(LED4);
//    }
    
    /* Clear the EXTI line 10 flag */
    EXTI_ClearITPendingBit(EXTI_Line10);
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
  
  /* Spirit IRQ config */
  SpiritGpioInit(&xGpioIRQ);
  
  SdkEvalM2SGpioInit(M2S_GPIO_3,M2S_MODE_EXTI_IN);
  SdkEvalM2SGpioInterruptCmd(M2S_GPIO_3,0x0F,0x0F,ENABLE);
  
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
//  SpiritIrq(VALID_SYNC,S_ENABLE);
  
  /* payload length config */
  SpiritPktBasicSetPayloadLength(20);
  
  /* enable SQI check */
  SpiritQiSetSqiThreshold(SQI_TH_0);
  SpiritQiSqiCheck(S_ENABLE);
  
  /* rx timeout config */
  SpiritTimerSetRxTimeoutMs(1000.0);
  SpiritTimerSetRxTimeoutStopCondition(SQI_ABOVE_THRESHOLD);
  
  /* IRQ registers blanking */
  SpiritIrqClearStatus();
  
  /* RX command */
  SpiritCmdStrobeRx();
  
  /* infinite loop */
  while (1){
    /* print menu */
    pktMonPrintMenu();
    
    /* handle the user choose */
    pktMonMenuManagement();  
    
    
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
