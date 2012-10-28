/**
* @file    SDK_StackGeneric_A.c
* @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
* @version V3.0.0
* @date    August 7, 2012
* @brief   Example of transmission of SPIRIT STack packets.
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
* @addtogroup SDK_Stack_Generic                                SDK STack Generic
* @{
*/

/**
* @addtogroup SDK_Stack_Generic_A                              SDK STack Generic A
* @brief Device A configured as a transmitter.
* @details This code explains how to configure and manage in the simpler way a transmitter
* of STack packets.
*
* The user can change the STack packet configuration parameters editing the defines
* at the beginning of the file.
* @{
*/

/**
* @defgroup Stack_Generic_A_Private_TypesDefinitions           STack Generic A Private TypesDefinitions
* @{
*/

/**
*@}
*/


/**
* @defgroup Stack_Generic_A_Private_Defines                    STack Generic A Private Defines
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
#define DATARATE                    	38400
#define FREQ_DEVIATION              	20e3
#define BANDWIDTH                   	100.5e3

#define POWER_DBM                   	10.0

/*  Packet configuration parameters  */
#define PREAMBLE_LENGTH             	PKT_PREAMBLE_LENGTH_08BYTES
#define SYNC_LENGTH                 	PKT_SYNC_LENGTH_4BYTES
#define SYNC_WORD                   	0x1A2635A8
#define LENGTH_TYPE                 	PKT_LENGTH_VAR
#define LENGTH_WIDTH                	6
#define CRC_MODE                    	PKT_CRC_MODE_8BITS
#define CONTROL_LENGTH              	PKT_CONTROL_LENGTH_0BYTES
#define EN_FEC                      	S_DISABLE
#define EN_WHITENING                	S_DISABLE

/*  Addresses configuration parameters  */
#define EN_FILT_MY_ADDRESS          	S_ENABLE
#define MY_ADDRESS                  	0x44
#define EN_FILT_MULTICAST_ADDRESS       S_DISABLE
#define MULTICAST_ADDRESS           	0xEE
#define EN_FILT_BROADCAST_ADDRESS       S_DISABLE
#define BROADCAST_ADDRESS           	0xFF
#define DESTINATION_ADDRESS        	0x34

/* LLP configuration parameters */
#define EN_AUTOACK                      S_DISABLE
#define EN_PIGGYBACKING             	S_DISABLE
#define MAX_RETRANSMISSIONS         	PKT_DISABLE_RETX

/**
*@}
*/


/**
* @defgroup Stack_Generic_A_Private_Macros                             STack Generic A Private Macros
* @{
*/

/**
*@}
*/

/**
* @defgroup Stack_Generic_A_Private_Variables                          STack Generic A Private Variables
* @{
*/

/**
* @brief Packet Stack structure fitting
*/
PktStackInit xStackInit=
{
  PREAMBLE_LENGTH,
  SYNC_LENGTH,
  SYNC_WORD,
  LENGTH_TYPE,
  LENGTH_WIDTH,
  CRC_MODE,
  CONTROL_LENGTH,
  EN_FEC,
  EN_WHITENING
};


/**
* @brief Address structure fitting
*/
PktStackAddressesInit xAddressInit=
{
  EN_FILT_MY_ADDRESS,
  MY_ADDRESS,
  EN_FILT_MULTICAST_ADDRESS,
  MULTICAST_ADDRESS,
  EN_FILT_BROADCAST_ADDRESS,
  BROADCAST_ADDRESS
};


/**
* @brief LLP structure fitting
*/
PktStackLlpInit xStackLLPInit =
{
  EN_AUTOACK,
  EN_PIGGYBACKING,
  MAX_RETRANSMISSIONS
};


/**
* @brief GPIO structure fitting
*/
SGpioInit xGpioIRQ=
{
  SPIRIT_GPIO_3,
  SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
  SPIRIT_GPIO_DIG_OUT_IRQ
};


/**
* @brief Declare the Tx done flag
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
* @defgroup Stack_Generic_A_Private_FunctionPrototypes                         STack Generic A Private FunctionPrototypes
* @{
*/

/**
*@}
*/


/**
* @defgroup Stack_Generic_A_Private_Functions                                  STack Generic A Private Functions
* @{
*/

/**
* @brief  This function handles External interrupt request (associated with Spirit GPIO 3).
* @param  None
* @retval None
*/
void M2S_GPIO_3_EXTI_IRQ_HANDLER(void)
{
  if(EXTI_GetFlagStatus(M2S_GPIO_3_EXTI_LINE) && SpiritIrqCheckFlag(TX_DATA_SENT))
  {
    xTxDoneFlag = SET;
    SdkEvalLedToggle(LED1);
    
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
  
  /* Start SysTick */
  SdkStartSysTick();
  
  /* Micro EXTI config */
  SdkEvalM2SGpioInit(M2S_GPIO_SDN,M2S_MODE_GPIO_OUT);
  
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
  SpiritPktStackInit(&xStackInit);
  SpiritPktStackAddressesInit(&xAddressInit);
  SpiritPktStackLlpInit(&xStackLLPInit);
  
  /* Spirit IRQs enable */
  SpiritIrqDeInit(NULL);
  SpiritIrq(TX_DATA_SENT , S_ENABLE);
  
  /* payload length config */
  SpiritPktStackSetPayloadLength(20);
  
  /* destination address */
  SpiritPktStackSetDestinationAddress(DESTINATION_ADDRESS);
  
  /* IRQ registers blanking */
  SpiritIrqClearStatus();
  
  
  /* infinite loop */
  while (1)
  {
    
#ifdef USE_VCOM
    printf("A data to transmit: [");
    
    for(uint8_t i=0 ; i<20 ; i++)
      printf("%d ", vectcTxBuff[i]);
    printf("]\r\n");
#endif
    
    /* fit the TX FIFO */
    SpiritCmdStrobeFlushTxFifo();
    SpiritSpiWriteLinearFifo(20, vectcTxBuff);
    
    /* send the TX command */
    SpiritCmdStrobeTx();
    
    /* wait for TX done */
    while(!xTxDoneFlag);
    xTxDoneFlag = RESET;
    
    
    /* pause between two TX */
    SdkDelayMs(800);
    
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