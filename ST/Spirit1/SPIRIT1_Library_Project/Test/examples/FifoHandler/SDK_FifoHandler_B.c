/**
* @file    SDK_FifoHandler_B.c
 * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
 * @version V3.0.0
 * @date    August 7, 2012
* @brief   Example of reception of SPIRIT Basic packets with payload longer than 96 bytes.
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

//#define COLERIDGE

/**
* @addtogroup SDK_Examples
* @{
*/

/**
* @addtogroup SDK_Fifo_Handler        SDK FIFO Handler
* @{
*/

/**
* @addtogroup SDK_Fifo_Handler_B              SDK FIFO Handler B
* @brief Device B configured as a receiver.
* @details This code explains how to configure a receiver for basic packets with payload longer than 96 bytes.
*
* The user can change the Basic packet configuration parameters editing the defines
* at the beginning of the file.
* @{
*/

/**
* @defgroup Fifo_Handler_B_Private_TypesDefinitions           FIFO Handler B Private TypesDefinitions
* @{
*/

/**
*@}
*/


/**
* @defgroup Fifo_Handler_B_Private_Defines                    FIFO Handler B Private Defines
* @{
*/

/*  Radio configuration parameters  */
#define XTAL_OFFSET_PPM             0

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

//#define DATARATE                    1200
//#define FREQ_DEVIATION              20e3
//#define BANDWIDTH                   50.5E3

//#define DATARATE                    38400
//#define FREQ_DEVIATION              20e3
//#define BANDWIDTH                   100.5E3

//#define DATARATE                    100000
//#define FREQ_DEVIATION              50e3
//#define BANDWIDTH                   325E3

//#define DATARATE                    180000
//#define FREQ_DEVIATION              100e3
//#define BANDWIDTH                   500E3


//#define DATARATE                    250000
//#define FREQ_DEVIATION              127e3
//#define BANDWIDTH                   540E3

#define DATARATE                    500000
#define FREQ_DEVIATION              150e3
#define BANDWIDTH                   750E3

/*  Packet configuration parameters */
#define PREAMBLE_LENGTH             PKT_PREAMBLE_LENGTH_08BYTES
#define SYNC_LENGTH                 PKT_SYNC_LENGTH_4BYTES
#define SYNC_WORD                   0x1A2635A8
#define LENGTH_TYPE                 PKT_LENGTH_VAR
#define LENGTH_WIDTH                12
#define CRC_MODE                    PKT_CRC_MODE_8BITS
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

/* max payload size */
#define PAYLOAD_LENGTH_FIX 1000

/**
*@}
*/


/**
* @defgroup Fifo_Handler_B_Private_Macros                             FIFO Handler B Private Macros
* @{
*/

/**
*@}
*/

/**
* @defgroup Fifo_Handler_B_Private_Variables                          FIFO Handler B Private Variables
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
* @brief GPIO IRQ structure fitting
*/
SGpioInit xGpioIRQ={
  SPIRIT_GPIO_3,
  SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
  SPIRIT_GPIO_DIG_OUT_IRQ
};

/**
 * @brief Declare the Rx done flag
 */
FlagStatus xRxDoneFlag = RESET;


/**
* @brief IRQ status struct declaration
*/
SpiritIrqs xIrqStatus;


/**
* @brief Rx buffer declaration: how to store the received data
*/
uint8_t vectcRxBuff[PAYLOAD_LENGTH_FIX];

/**
 * @brief Rx buffer index, residual and total bytes to be received
 */
uint16_t nRxIndex, nPayloadLength = 20;

/**
 * @brief Almost full Tx FIFO threshold  (measured from the tail)
 */
uint8_t cThreholdRxFifoAF = 78;


/**
 * @brief Rx data in FIFO
 */
uint8_t cRxDataLen;


/**
 * @brief Some useful debug variables
 */
uint8_t cAFCounter = 0;
uint8_t vett1[100];
uint8_t vett2[100];

/**
*@}
*/

/**
* @defgroup Fifo_Handler_B_Private_FunctionPrototypes                         FIFO Handler B Private FunctionPrototypes
* @{
*/

/**
*@}
*/


/**
* @defgroup Fifo_Handler_B_Private_Functions                                  FIFO Handler B Private Functions
* @{
*/

/**
* @brief  This function handles External interrupt request. In this application it is used
*         to manage the Spirit IRQ configured to be notified on the Spirit GPIO_3.
* @param  None
* @retval None
*/
void M2S_GPIO_3_EXTI_IRQ_HANDLER(void)
{
  /* Check the flag status of EXTI line */
  if(EXTI_GetITStatus(M2S_GPIO_3_EXTI_LINE))
  {
    /* Clear the EXTI line flag */
    EXTI_ClearITPendingBit(M2S_GPIO_3_EXTI_LINE);

    /* Get the IRQ status */
    SpiritIrqGetStatus(&xIrqStatus);

    /* Check the RX_FIFO_ALMOST_FULL IRQ flag */
    if(xIrqStatus.IRQ_RX_FIFO_ALMOST_FULL)
    {
      SdkEvalLedToggle(LED2);

      /* Get the RX FIFO size */
      cRxDataLen = SpiritLinearFifoReadNumElementsRxFifo();

      /* ..check */
      if(cRxDataLen >= cThreholdRxFifoAF)
      {
        /* unarm the almost full threshold */
        SpiritIrq(RX_FIFO_ALMOST_FULL,S_DISABLE);
        SpiritLinearFifoSetAlmostFullThresholdRx(1);

        /* store the FIFO length before reading it */
        vett1[cAFCounter] = cRxDataLen;

        /* Read the RX FIFO */
        SpiritSpiReadLinearFifo(cRxDataLen-1, &vectcRxBuff[nRxIndex]);

        /* update the number of received bytes */
        nRxIndex += cRxDataLen-1;

        /* store the FIFO length after reading it */
//        cRxDataLen = SpiritLinearFifoReadNumElementsRxFifo();
//        vett2[cAFCounter] = cRxDataLen;

        /* increment the debug vectors index */
        cAFCounter++;

        /* rearm the FIFO AF threshold */
        SpiritLinearFifoSetAlmostFullThresholdRx(96-cThreholdRxFifoAF);
        SpiritIrq(RX_FIFO_ALMOST_FULL,S_ENABLE);
      }
    }
    /* Check the RX_DATA_READY IRQ flag */
    if(xIrqStatus.IRQ_RX_DATA_READY /*|| SpiritIrqCheckFlag(RX_DATA_READY)*/)
    {
      SdkEvalLedToggle(LED3);

      /* set the Rx done flag */
      xRxDoneFlag = SET;
    }

    /* Check the SPIRIT RX_TIMEOUT IRQ flag */
    if(xIrqStatus.IRQ_RX_TIMEOUT)
    {
      SdkEvalLedToggle(LED1);

      /* RX command - to ensure the device will be ready for the next reception */
      SpiritCmdStrobeRx();
    }
  }
}

/**
* @brief  Debug printing function.
* @param  None
* @retval None
*/
void printDbgData(void)
{
  SpiritBool correct=S_TRUE;

  /* print the received data */
    printf("B data received: \n\r");

    for(uint16_t i=0 ; i<nRxIndex ; i++)
    {
#ifndef COLERIDGE
      printf("%d ", vectcRxBuff[i]);
#else
      printf("%c", vectcRxBuff[i]);
#endif
      if(vectcRxBuff[i] != (uint8_t)i)
        correct=S_FALSE;
    }
    printf("\n\n\r");

    if(correct)
      SdkEvalLedToggle(LED5);

    /* print the received data */
    printf("Data in FIFO before reading: [");

    for(uint16_t i=0 ; i<cAFCounter ; i++)
    {
      printf("%d ", vett1[i]);
    }
    printf("]\n\r");

    printf("\n\rAF Thr=%d\n\rPayload length=%d\n\n\r", cThreholdRxFifoAF,nRxIndex);
}


/**
* @brief  System main function.
* @param  None
* @retval None
*/
void main (void)
{
  /* Remap the vector table to the index 0x3000 (to be used for DFU).
      The user should comment the following line if the DFU firmware has not been flashed on the MCU. */
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
  
  /* Spirit Packet config */
  SpiritPktBasicInit(&xBasicInit);
  SpiritPktBasicFilterOnCrc(S_DISABLE);
  SpiritPktBasicAddressesInit(&xAddressInit);

  /* Spirit IRQs enable */
  SpiritIrqDeInit(&xIrqStatus);
  SpiritIrq(RX_TIMEOUT,S_ENABLE);
  SpiritIrq(RX_DATA_READY,S_ENABLE);

  /* enable SQI check */
  SpiritQiSetSqiThreshold(SQI_TH_0);
  SpiritQiSqiCheck(S_ENABLE);

  /* RX timeout config */
  SpiritTimerSetRxTimeoutMs(1500.0);
  SpiritTimerSetRxTimeoutStopCondition(SQI_ABOVE_THRESHOLD);

  /* IRQ registers blanking */
  SpiritIrqClearStatus();

#ifdef COLERIDGE
  nPayloadLength = 402;
#endif

  /* infinite loop */
  while (1)
  {

    /* reset the Rx buffer index (number of bytes received until now) */
    nRxIndex = 0;

    /* reset the debug vectors index */
    cAFCounter = 0;

    /* payload length config */
    SpiritPktBasicSetPayloadLength(nPayloadLength);

    /* .. set the almost full threshold and configure the associated IRQ */
    SpiritLinearFifoSetAlmostFullThresholdRx(96-cThreholdRxFifoAF);
    SpiritIrq(RX_FIFO_ALMOST_FULL,S_ENABLE);

    /* RX command */
    SpiritCmdStrobeRx();
    
    /* wait for RX done */
    while(!xRxDoneFlag) 
    {
    ;
    }
    
    xRxDoneFlag = RESET;

    /* Get the RX FIFO size */
    cRxDataLen = SpiritLinearFifoReadNumElementsRxFifo();

    /* store the FIFO length before reading it */
    vett1[cAFCounter] = cRxDataLen;

    if(cRxDataLen!=0)
      /* Read the RX FIFO */
      SpiritSpiReadLinearFifo(cRxDataLen, &vectcRxBuff[nRxIndex]);

    /* update the number of received bytes */
    nRxIndex += cRxDataLen;

    /* store the FIFO length after reading it */
    vett2[cAFCounter] = SpiritLinearFifoReadNumElementsRxFifo();

    /* increment the debug vectors index */
    cAFCounter++;

#ifdef USE_VCOM
    printDbgData();
#endif

#ifndef COLERIDGE
    /* increment the payload length (overflowing it at PAYLOAD_LENGTH_FIX) */
    nPayloadLength = (nPayloadLength+1)%PAYLOAD_LENGTH_FIX;
#endif
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
