/**
 * @file    SDK_StackLlp_B.c
 * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
 * @version V3.0.0
 * @date    August 7, 2012
 * @brief   Example of reception of SPIRIT Basic packets.
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
#define USE_HIGH_BAND

#ifdef USE_VCOM
#include "SDK_EVAL_VC_General.h"
#endif

//#define PIGGYBACKING

/**
 * @addtogroup SDK_Examples
 * @{
 */

/**
 * @addtogroup SDK_StackLlp                     SDK STack LLP
 * @{
 */

/**
 * @addtogroup SDK_StackLlp_B                   SDK STack LLP B
 * @{
 */

/**
 * @defgroup StackLlp_B_Private_TypesDefinitions                STack LLP B Private TypesDefinitions
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup StackLlp_B_Private_Defines                         STack LLP B Private Defines
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
#define BANDWIDTH                   	100.5E3

#define POWER_DBM                   	10.0

/*  Packet configuration parameters  */
#define PREAMBLE_LENGTH             	PKT_PREAMBLE_LENGTH_08BYTES
#define SYNC_LENGTH                 	PKT_SYNC_LENGTH_4BYTES
#define SYNC_WORD                   	0x1A2635A8
#define LENGTH_TYPE                	PKT_LENGTH_VAR
#define LENGTH_WIDTH                	6
#define CRC_MODE                    	PKT_CRC_MODE_8BITS
#define CONTROL_LENGTH              	PKT_CONTROL_LENGTH_0BYTES
#define EN_FEC                      	S_DISABLE
#define EN_WHITENING                	S_DISABLE

/*  Addresses configuration parameters  */
#define EN_FILT_MY_ADDRESS          	S_DISABLE
#define MY_ADDRESS                  	0x34
#define EN_FILT_MULTICAST_ADDRESS       S_DISABLE
#define MULTICAST_ADDRESS           	0xEE
#define EN_FILT_BROADCAST_ADDRESS       S_DISABLE
#define BROADCAST_ADDRESS           	0xFF
#define DESTINATION_ADDRESS         	0x44

/* LLP configuration parameters */
#define EN_AUTOACK                  	S_ENABLE

#ifdef PIGGYBACKING
#define EN_PIGGYBACKING             	S_ENABLE
#else
#define EN_PIGGYBACKING             	S_DISABLE
#endif

#define MAX_RETRANSMISSIONS         	PKT_DISABLE_RETX

/**
 *@}
 */


/**
 * @defgroup StackLlp_B_Private_Macros                          STack LLP B Private Macros
 * @{
 */

/**
 *@}
 */

/**
 * @defgroup StackLlp_B_Private_Variables                       STack LLP B Private Variables
 * @{
 */

/* Packet Basic structure fitting */
PktStackInit xStackInit={
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

/* Address structure fitting */
PktStackAddressesInit xAddressInit={
  EN_FILT_MY_ADDRESS,
  MY_ADDRESS,
  EN_FILT_MULTICAST_ADDRESS,
  MULTICAST_ADDRESS,
  EN_FILT_BROADCAST_ADDRESS,
  BROADCAST_ADDRESS
};

/* LLP structure fitting */
PktStackLlpInit xStackLLPInit ={
  EN_AUTOACK,
  EN_PIGGYBACKING,
  MAX_RETRANSMISSIONS
};

/* GPIO structure fitting */
SGpioInit xGpioIRQ={
  SPIRIT_GPIO_3,
  SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
  SPIRIT_GPIO_DIG_OUT_IRQ
};


SpiritIrqs xIrqStatus;
uint8_t vectcRxBuff[96], cRxData;

#ifdef PIGGYBACKING
uint8_t vectcTxBuff[]={21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40};
#endif

/**
 *@}
 */

/**
 * @defgroup StackLlp_B_Private_FunctionPrototypes                      STack LLP B Private Function Prototypes
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup StackLlp_B_Private_Functions                               STack LLP B Private Functions
 * @{
 */


/**
* @brief  This function handles External lines 15 to 10 interrupt request.
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
      SpiritCmdStrobeRx();

    }
    /* Check the SPIRIT RX_DATA_DISC IRQ flag */
    if(xIrqStatus.IRQ_RX_DATA_DISC)
    {
      SdkEvalLedToggle(LED2);
    }

    /* Check the SPIRIT TX_DATA_SENT IRQ flag for the acknoledgement */
    if(xIrqStatus.IRQ_TX_DATA_SENT)
    {
      SdkEvalLedToggle(LED3);

#ifdef PIGGYBACKING
      SpiritSpiWriteLinearFifo(20, vectcTxBuff);
      printf("Loaded piggybacking data: [");

      for(uint8_t i=0 ; i<20 ; i++)
        printf("%d ", vectcTxBuff[i]);
      printf("]\n\r");

#endif
  
      SpiritCmdStrobeRx();
    }

    /* Check the SPIRIT RX_DATA_READY IRQ flag */
    if(xIrqStatus.IRQ_RX_DATA_READY)
    {
    	/* Get the RX FIFO size */
    	cRxData=SpiritLinearFifoReadNumElementsRxFifo();

    	/* Read the RX FIFO */
    	SpiritSpiReadLinearFifo(cRxData, vectcRxBuff);

    	/* Flush the RX FIFO */
    	SpiritCmdStrobeFlushRxFifo();

      /*  A simple way to control if the received data sequence is correct (in this case LED5 will toggle) */
      {
        SpiritBool correct=S_TRUE;

        for(uint8_t i=0 ; i<cRxData ; i++)
          if(vectcRxBuff[i] != i+1)
            correct=S_FALSE;

        if(correct)
          SdkEvalLedToggle(LED5);
      }
#ifdef USE_VCOM
      /* print the received data */
      printf("B data received: [");

      for(uint8_t i=0 ; i<cRxData ; i++)
        printf("%d ", vectcRxBuff[i]);
      printf("]\r\n");

#endif
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
  SpiritPktStackInit(&xStackInit);
  //SpiritPktStackAddressesInit(&xAddressInit);
  SpiritPktStackLlpInit(&xStackLLPInit);

  /* Spirit IRQs enable */
  SpiritIrqDeInit(&xIrqStatus);
  SpiritIrq(RX_TIMEOUT,S_ENABLE);
  SpiritIrq(RX_DATA_DISC,S_ENABLE);
  SpiritIrq(RX_DATA_READY,S_ENABLE);
  SpiritIrq(TX_DATA_SENT , S_ENABLE);
  SpiritIrq(VALID_SYNC,S_ENABLE);

  EXTI_GetFlagStatus(M2S_GPIO_3_EXTI_LINE);
  SdkEvalM2SGpioInit(M2S_GPIO_3,M2S_MODE_EXTI_IN);
  SdkEvalM2SGpioInterruptCmd(M2S_GPIO_3,0x0F,0x0F,ENABLE);

  /* payload length config */
  SpiritPktStackSetPayloadLength(20);

  /* enable SQI check */
  SpiritQiSetSqiThreshold(SQI_TH_0);
  SpiritQiSqiCheck(S_ENABLE);

  /* rx timeout config */
  SpiritTimerSetRxTimeoutMs(1000.0);
  //SET_INFINITE_RX_TIMEOUT();
  SpiritTimerSetRxTimeoutStopCondition(SQI_ABOVE_THRESHOLD);

  /* IRQ registers blanking */
  SpiritIrqClearStatus();

#ifdef PIGGYBACKING
  SpiritSpiWriteLinearFifo(20, vectcTxBuff);
#endif

  /* RX command */
  SpiritCmdStrobeRx();

  while (1){
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
