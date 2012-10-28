/**
* @file    SDK_BasicPktTest_A.c
* @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
* @version V3.0.0
* @date    August 7, 2012
* @brief   Example of SPIRIT Basic packet test (Tx).
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
#include <string.h>
#include "SDK_EVAL_VC_General.h"
#include "SDK_BasicPktTest_Common.h"
#include "SPIRIT_Management.h"


/**
* @addtogroup SDK_Examples
* @{
*/

/**
* @addtogroup SDK_BasicPktTest                                 SDK Basic Pkt Test
* @{
*/

/**
* @addtogroup SDK_BasicPktTest_A                                       SDK Basic Pkt Test A
* @brief Device A configured as a transmitter.
* @details This code is finalized to the computation of a packet error rate.
* It performs a counting of the transmitted packets and then use this value,
* together with the received packet counter, to compute the PER.
*
* The user can change the configuration parameters editing the defines at the
* beginning of the file <i>@ref SDK_BasicPktTest_Common.h</i>.
* @{
*/

/**
* @defgroup BasicPktTest_A_Private_TypesDefinitions                    Basic Pkt Test A Private TypesDefinitions
* @{
*/

/**
*@}
*/


/**
* @defgroup BasicPktTest_A_Private_Defines                             Basic Pkt Test A Private Defines
* @{
*/

/**
* @brief Time between two transmissions expressed in ms.
*/
#define WAIT_TIME       25.0

/**
*@}
*/


/**
* @defgroup BasicPktTest_A_Private_Macros                              Basic Pkt Test A Private Macros
* @{
*/

/**
*@}
*/

/**
* @defgroup BasicPktTest_A_Private_Variables                           Basic Pkt Test A Private Variables
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
* @brief Tx done flag declaration
*/
FlagStatus xTxDoneFlag = RESET,
xTim2Raised = RESET;

/**
* @brief IRQ status struct declaration
*/
SpiritIrqs xIrqStatus;

/**
* @brief Tx buffer declaration: data to transmit
*/
uint8_t vectcTxBuff[20]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

/**
* @brief Test start and stop buffer declaration
*/
uint8_t vectcTestStartBuff[20]={'S','T','A','R','T',' ','T','E','S','T','S','T','A','R','T',' ','T','E','S','T'},
vectcTestStopBuff[20]={'S','T','O','P',' ','T','E','S','T',0,0,0,0,0,0,0,0,0,0,0};

/**
* @brief Declaration of variables used to manage the V-COM communication
*/
uint8_t vectcFromVcom[21];

/**
*@}
*/

/**
* @defgroup BasicPktTest_A_Private_FunctionPrototypes                          Basic Pkt Test A Private FunctionPrototypes
* @{
*/

/**
*@}
*/


/**
* @defgroup BasicPktTest_A_Private_Functions                                   Basic Pkt Test A Private Functions
* @{
*/


/**
* @brief  This function handles TIM2 interrupt request.
*         In this example TIM2 is used as the sleep timer.
* @param  None
* @retval None
*/
void TIM2_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM2 , TIM_IT_Update))
  {
    /* Set the TIM2 raised flag */
    xTim2Raised=SET;
    
    /* toggle LED2 */
    SdkEvalLedToggle(LED2);
    
    /* Clear the TIM2 IRQ pending bit */
    TIM_ClearITPendingBit(TIM2 , TIM_IT_Update);
  }
  
}


/**
* @brief  This function handles External interrupt request (associated with Spirit GPIO 3).
* @param  None
* @retval None
*/
void M2S_GPIO_3_EXTI_IRQ_HANDLER(void)
{
  /* Check the flag status of EXTI line */
  if(EXTI_GetFlagStatus(M2S_GPIO_3_EXTI_LINE))
  {
    /* Check the SPIRIT TX_DATA_SENT IRQ flag */
    if(SpiritIrqCheckFlag(TX_DATA_SENT))
    {
      /* set the tx_done_flag to manage the event in the main() */
      xTxDoneFlag = SET;
      
      /* Toggle LED1 */
      SdkEvalLedToggle(LED1);
    }
    
    /* Clear the EXTI line flag */
    EXTI_ClearFlag(M2S_GPIO_3_EXTI_LINE);
  }
  
}


/**
* @brief  Virtual com string receiver. It waits for a string received through VCOM
*         searching the '\r' char.
* @param  vectcFromVcom: string to be received.
* @retval None
*/
void receiveFromVcom(uint8_t *vectcFromVcom)
{
  uint16_t nNFromVcom=0;
  
  do{
    while(!__io_getcharNonBlocking(&vectcFromVcom[nNFromVcom]));  
    nNFromVcom++;
    
    /* if '\r' has been received */
  }while((char)vectcFromVcom[nNFromVcom-1] != '\r');

  /* add the newline and the string termination chars */
  vectcFromVcom[nNFromVcom] = '\n';
  vectcFromVcom[nNFromVcom + 1] = '\0';
  
}


/**
* @brief  Transmission of a buffer via SPIRIT Basic packets.
* @param  buff: array of bytes to be transmitted.
* @retval None
*/
void txBuffer(uint8_t *buff)
{
  /* Flush the Tx FIFO */
  SpiritCmdStrobeFlushTxFifo();
  
  /* Fit the Tx FIFO */
  SpiritSpiWriteLinearFifo(20, buff);
  
  /* send the TX command */
  SpiritCmdStrobeTx();
  
  /* wait for TX done */
  while(!xTxDoneFlag);
  xTxDoneFlag = RESET;
  
  /* pause between RX and TX */
  SdkEvalTimersState(TIM2,ENABLE);
  while(!xTim2Raised);
  xTim2Raised=RESET;
  SdkEvalTimersState(TIM2,DISABLE);
}


/**
* @brief  System main function.
* @param  None
* @retval None
*/
void main (void)
{
  NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x3000 );
  
  FlagStatus xStopTest=SET;
  uint32_t lTxCount=0;
  
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
  
  /* TIMER 2 config */
  SdkEvalTimersTimConfig_ms(TIM2 , WAIT_TIME);
  
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
  
  /* Spirit Radio set power */
  SpiritRadioSetPALeveldBm(0,POWER_DBM);
  SpiritRadioSetPALevelMaxIndex(0);
  
  /* Spirit Packet config */
  SpiritPktBasicInit(&xBasicInit);
  SpiritPktBasicAddressesInit(&xAddressInit);
  
  /* Spirit IRQs enable */
  SpiritIrqDeInit(NULL);
  SpiritIrq(TX_DATA_SENT , S_ENABLE);
  
  /* payload length config */
  SpiritPktBasicSetPayloadLength(20);
  
  /* destination address */
  SpiritPktBasicSetDestinationAddress(DESTINATION_ADDRESS);
  
  /* IRQ registers blanking */
  SpiritIrqClearStatus();
  
  /* print a 'welcome' message */
  printf("DEVICE A (Tx)\n\rUse the following commands:\n\r - start: to start the test\n\r - stop: to stop the test\n\n\r");
  
  /* set the RSSI threshold */
  SpiritQiSetRssiThreshold(1);
  
  /* infinite loop */
  while (1){
    
    /* print message */
    if(xStopTest)
    {
      printf("\rInsert command \"start\" to begin\n\r");
    }
    else
    {
      printf("\rInsert command \"stop\" to stop\n\r");
    }
    
    /* while no bytes are sent through VCOM... */
    /* ...and if the test is not stopped */
    if(!xStopTest){
      /* ...then perform the transmission */
      txBuffer(vectcTxBuff);
      
      /* increment the Tx number */
      lTxCount++;
      
      /* print the tx_count */
      printf("\rtx_count: %d", lTxCount);
      
    }
    /* if some keys are pressed expect the '\r' char receiving a string */
    receiveFromVcom(vectcFromVcom);
    
    /* if the start command has been received */
    if(strcmp((char*)vectcFromVcom,"start\r\n") == 0)
    {
      /* reset the stop flag */
      xStopTest=RESET;
      vectcFromVcom[0]='\0';
      
      /* erase the video row */
      printf("\rtx_count:                                         ");
      
      /* reset the tx counter */
      lTxCount=0;
      
      /* send the 'start Tx' frame */
      txBuffer(vectcTestStartBuff);
    }
    /* if the stop command has been received */
    else if(strcmp((char*)vectcFromVcom,"stop\r\n") == 0)
    {
      /* set the stop flag */
      xStopTest=SET;
      vectcFromVcom[0]='\0';
      
      /* print a test finished message */
      printf("\rTEST FINISHED tx_count=%d", lTxCount);
      
      /* put into the stop frame the number of transmission to send it to the receiver */
      for(uint8_t i=0 ; i<4 ; i++)
        vectcTestStopBuff[9+i]=(uint8_t)(lTxCount >> i*8);
      
      /* send the stop frame to the receiver */
      txBuffer(vectcTestStopBuff);
      
    }
    
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
