/**
* @file    SDK_MbusGeneric_A.c
* @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
* @version V3.0.0
* @date    August 7, 2012
* @brief   Example of transmission of SPIRIT MBUS packets.
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

#define USE_HIGH_BAND
#define USE_VCOM

#ifdef USE_VCOM
#include "SDK_EVAL_VC_General.h"
#endif

/**
* @addtogroup SDK_Examples
* @{
*/

/**
* @addtogroup SDK_Mbus_Generic                         SDK MBUS Generic
* @{
*/

/**
* @addtogroup SDK_Mbus_Generic_A                       SDK MBUS Generic A
* @brief Device A configured as a transmitter.
* @details This code explains how to configure and manage in the simpler way a transmitter of WMBUS packets.
*
* The user can change the WMBUS packet configuration parameters editing the defines
* at the beginning of the file.
* @{
*/

/**
* @defgroup Mbus_Generic_A_Private_TypesDefinitions            MBUS Generic A Private TypesDefinitions
* @{
*/

/**
*@}
*/


/**
* @defgroup Mbus_Generic_A_Private_Defines                     MBUS Generic A Private Defines
* @{
*/


/*  Radio configuration parameters  */
#define XTAL_OFFSET_PPM             	0

#ifdef USE_VERY_LOW_BAND
#define BASE_FREQUENCY              169.0e6
#endif

#ifdef USE_LOW_BAND
#define BASE_FREQUENCY              	315.0e6
#endif

#ifdef USE_MIDDLE_BAND
#define BASE_FREQUENCY              	433.0e6
#endif

#ifdef USE_HIGH_BAND
#define BASE_FREQUENCY              	868.0e6
#endif

#define CHANNEL_SPACE               	20e3
#define CHANNEL_NUMBER              	0
#define MODULATION_SELECT           	FSK
#define DATARATE                    	32768
#define FREQ_DEVIATION              	50e3
#define BANDWIDTH                   	232e3

#define POWER_DBM                   	10.0

/*  Packet configuration parameters  */
#define MBUS_SUBMODE                    MBUS_SUBMODE_S1_S2_LONG_HEADER
#define PREAMBLE_LENGTH                 36
#define POSTAMBLE_LENGTH                16

/**
*@}
*/


/**
* @defgroup Mbus_Generic_A_Private_Macros                              MBUS Generic A Private Macros
* @{
*/

/**
*@}
*/

/**
* @defgroup Mbus_Generic_A_Private_Variables                           MBUS Generic A Private Variables
* @{
*/

/**
* @brief Packet Mbus structure fitting
*/
PktMbusInit mbusInit={
  MBUS_SUBMODE,
  PREAMBLE_LENGTH,
  POSTAMBLE_LENGTH
};


/* GPIO structure fitting */
SGpioInit xGpioIRQ={
  SPIRIT_GPIO_3,
  SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
  SPIRIT_GPIO_DIG_OUT_IRQ
};


/**
* @brief Tx done flag declaration
*/
FlagStatus xTxDoneFlag = RESET;


/**
* @brief IRQ status struct declaration
*/
SpiritIrqs xIrqStatus;


/**
* @brief Tx buffer declaration: data to transmit
*/
uint8_t vectcTxBuff[20]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};


/**
*@}
*/

/**
* @defgroup Mbus_Generic_A_Private_FunctionPrototypes                          MBUS Generic A Private FunctionPrototypes
* @{
*/

/**
*@}
*/


/**
* @defgroup Mbus_Generic_A_Private_Functions                                   MBUS Generic A Private Functions
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
  if(EXTI_GetFlagStatus(M2S_GPIO_3_EXTI_LINE))
  {
    /* Check the SPIRIT TX_DATA_SENT IRQ flag */
    if(SpiritIrqCheckFlag(TX_DATA_SENT))
    {
      /* set the tx_done_flag to manage the event in the main() */
      xTxDoneFlag = SET;
      
      SdkEvalLedToggle(LED1);
    }
    
    /* Clear the EXTI line flag */
    EXTI_ClearFlag(M2S_GPIO_3_EXTI_LINE);
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
  
#ifdef USE_VCOM
  /* VC config */
  SdkEvalVCInit();
  while(bDeviceState != CONFIGURED);
#endif
  
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
  SRadioInit xRadioInit = 
  {
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
  SpiritPktMbusInit(&mbusInit);
  
  /* Spirit IRQs enable */
  SpiritIrqDeInit(NULL);
  SpiritIrq(TX_DATA_SENT , S_ENABLE);
  
  /* IRQ registers blanking */
  SpiritIrqClearStatus();
  
  
  /* infinite loop */
  while (1){
    
#ifdef USE_VCOM
    /* print data */
    printf("A data to transmit: [");
    
    for(uint8_t i=0 ; i<20 ; i++)
      printf("%d ", vectcTxBuff[i]);
    printf("]\n\r");
#endif
    
    /* fit the TX FIFO */
    SpiritCmdStrobeFlushTxFifo();
    SpiritSpiWriteLinearFifo(20, vectcTxBuff);
    
    /* send the TX command */
    SpiritCmdStrobeTx();
    
    /* wait for TX done */
    while(!xTxDoneFlag);
    xTxDoneFlag = RESET;
    
    /* pause between two transmissions */
    SdkDelayMs(500);
    
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