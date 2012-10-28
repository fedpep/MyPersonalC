/**
 * @file    SDK_FifoHandler_A.c
 * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
 * @version V3.0.0
 * @date    August 7, 2012
 * @brief   Example of transmission of SPIRIT Basic packets with payload longer than 96 bytes.
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
#include "SPIRIT_Management.h"

#define USE_HIGH_BAND
#define USE_VCOM

#ifdef USE_VCOM
#include "SDK_EVAL_VC_General.h"
#endif

//#define COLERIDGE


/**
 * @addtogroup SDK_Examples SDK Examples
 * @{
 */

/**
 * @addtogroup SDK_Fifo_Handler                                SDK FIFO Handler
 * @{
 */

/**
 * @addtogroup SDK_Fifo_Handler_A                                      SDK FIFO Handler A
 * @brief Device A configured as a transmitter.
 * @details This code explains how to configure and manage with payload longer than 96 bytes.
 *
 * The user can change the Basic packet configuration parameters editing the defines at the beginning of the file.
 * @{
 */

/**
 * @defgroup Fifo_Handler_A_Private_TypesDefinitions                   FIFO Handler A Private TypesDefinitions
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup Fifo_Handler_A_Private_Defines                            FIFO Handler A Private Defines
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
#define BANDWIDTH                  750E3

#define POWER_DBM                   10.0

/*  Packet configuration parameters  */
#define PREAMBLE_LENGTH             PKT_PREAMBLE_LENGTH_08BYTES
#define SYNC_LENGTH                 PKT_SYNC_LENGTH_4BYTES
#define SYNC_WORD                   0x1A2635A8
#define LENGTH_TYPE                 PKT_LENGTH_VAR
#define LENGTH_WIDTH                12
#define CRC_MODE                    PKT_NO_CRC
#define CONTROL_LENGTH              PKT_CONTROL_LENGTH_0BYTES
#define EN_ADDRESS                  S_DISABLE
#define EN_FEC                      S_DISABLE
#define EN_WHITENING                S_DISABLE

/*  Addresses configuration parameters  */
#define EN_FILT_MY_ADDRESS          S_DISABLE
#define MY_ADDRESS                  0x34
#define EN_FILT_MULTICAST_ADDRESS   S_DISABLE
#define MULTICAST_ADDRESS           0xEE
#define EN_FILT_BROADCAST_ADDRESS   S_DISABLE
#define BROADCAST_ADDRESS           0xFF
#define DESTINATION_ADDRESS         0x44

/* max payload size */
#define PAYLOAD_LENGTH_FIX 1000

/**
 *@}
 */


/**
 * @defgroup Fifo_Handler_A_Private_Macros                                     FIFO Handler A Private Macros
 * @{
 */

/**
 *@}
 */

/**
 * @defgroup Fifo_Handler_A_Private_Variables                                  FIFO Handler A Private Variables
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

#ifndef COLERIDGE
uint8_t vectcTxBuff[PAYLOAD_LENGTH_FIX];
#else
uint8_t vectcTxBuff[]="\n\rIt is an ancient Mariner,\n\r\
And he stoppeth one of three.\n\r\
'By thy long beard and glittering eye,\n\r\
Now wherefore stopp'st thou me?\n\n\r\
The Bridegroom's doors are opened wide,\n\r\
And I am next of kin;\n\r\
The guests are met, the feast is set:\n\r\
May'st hear the merry din.'\n\n\r\
He holds him with his skinny hand,\n\r\
'There was a ship,' quoth he.\n\r\
'Hold off! unhand me, grey-beard loon!'\n\r\
Eftsoons his hand dropt he.";
#endif


/**
 * @brief Tx buffer index, residual and total bytes to be transmitted
 */
uint16_t nTxIndex, nResidualPayloadLength, nPayloadLength=20;

/**
 * @brief Almost empty Tx FIFO threshold
 */
uint8_t cThreholdTxFifoAE = 48;


/**
 * @brief Some useful debug variables
 */
uint8_t cAECounter=0;
uint8_t vect1[100];
uint8_t vect2[100];

/**
 *@}
 */

/**
 * @defgroup Fifo_Handler_A_Private_FunctionPrototypes                                         FIFO Handler A Private FunctionPrototypes
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup Fifo_Handler_A_Private_Functions                                                  FIFO Handler A Private Functions
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
    /* Get the IRQ status */
    SpiritIrqGetStatus(&xIrqStatus);

    /* Clear the EXTI line flag */
    EXTI_ClearFlag(M2S_GPIO_3_EXTI_LINE);

    /* Check the SPIRIT TX_DATA_SENT IRQ flag */
    if(xIrqStatus.IRQ_TX_DATA_SENT)
    {
      /* set the xTxDoneFlag to manage the event in the main() */
      xTxDoneFlag = SET;

      SdkEvalLedToggle(LED1);
    }
    else
      /* Check the SPIRIT TX_FIFO_ALMOST_EMPTY IRQ flag */
      if(xIrqStatus.IRQ_TX_FIFO_ALMOST_EMPTY)
      {
        SdkEvalLedToggle(LED2);

        /* read the number of elements in the Tx FIFO */
        uint8_t cNElemTxFifo = SpiritLinearFifoReadNumElementsTxFifo();

        /* store it in a debug array (before write to the FIFO) */
        vect1[cAECounter] = cNElemTxFifo;

        /* check if the sum of the residual payload to be transmitted and the actual bytes in FIFO are higher than 96 */
        if(nResidualPayloadLength+cNElemTxFifo>96)
        {
          /* .. if yes another almost full IRQ has to be managed */

          /* ..so fill the Tx FIFO */
          SpiritSpiWriteLinearFifo(96-cNElemTxFifo-1, &vectcTxBuff[nTxIndex]);

          /* update the number of bytes to be transmitted */
          nResidualPayloadLength -= 96-cNElemTxFifo-1;

          /* update the number of bytes transmitted until now */
          nTxIndex += 96-cNElemTxFifo-1;
        }
        else
        {
          /* .. if not all the nResidualPayloadLength bytes remaining can be written to the Tx FIFO */
          /* ..so disable the TX_FIFO_ALMOST_EMPTY IRQ */
          SpiritIrq(TX_FIFO_ALMOST_EMPTY , S_DISABLE);

          /* unarm the AE threshold mechanism */
          SpiritLinearFifoSetAlmostEmptyThresholdTx(96);

          /* fill the Tx fifo */
          SpiritSpiWriteLinearFifo(nResidualPayloadLength, &vectcTxBuff[nTxIndex]);

          /* update the number of transmitted bytes */
          nTxIndex += nResidualPayloadLength;

          /* update the number of bytes to be transmitted */
          nResidualPayloadLength = 0;

        }

        /* re-read the number of elements in the Tx FIFO */
        cNElemTxFifo = SpiritLinearFifoReadNumElementsTxFifo();

        /* store it in a debug array (after write to the FIFO) */
        vect2[cAECounter] = cNElemTxFifo;

        /* increment the debug vectors index */
        cAECounter++;

      }

  }

}


/**
 * @brief  Fits the Tx buffer with the first nPayloadLength integer numbers starting from 0
 * @param  None
 * @retval None
 */
void fitPayload(void)
{
  for(uint16_t i=0; i<nPayloadLength ; i++)
    vectcTxBuff[i]=i;

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
  SdkEvalM2SGpioInterruptCmd(M2S_GPIO_3,0x0F,0x0F,DISABLE);
  
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
  SpiritIrq(TX_DATA_SENT, S_ENABLE);

  /* IRQ registers blanking */
  SpiritIrqClearStatus();

  /* enable the GPIO 0 IRQ (MCU side) */
  EXTI_ClearITPendingBit(M2S_GPIO_3_EXTI_LINE);
  SdkEvalM2SGpioInterruptCmd(M2S_GPIO_3, 0x0F, 0x0F, ENABLE);

  /* destination address */
  SpiritPktBasicSetDestinationAddress(DESTINATION_ADDRESS);

  /* IRQ registers blanking */
  SpiritIrqClearStatus();

  /* flush the Tx FIFO */
  SpiritCmdStrobeFlushTxFifo();

#ifdef COLERIDGE
  nPayloadLength = strlen((char*)vectcTxBuff);
#endif

    
  /* infinite loop */
  while (1)
  {
    /* make a copy of the payload length (to be used to have the number of the residual bytes to transmit) */
    nResidualPayloadLength = nPayloadLength;

    /* index TX buffer */
    nTxIndex = 0;

    /* reset the counter of the number of almost empty IRQs*/
    cAECounter=0;

#ifndef COLERIDGE
    /* fit the packet payload with data */
    fitPayload();
#endif

    /* payload length config */
    SpiritPktBasicSetPayloadLength(nPayloadLength);

#ifdef USE_VCOM
    printf("A data to transmit: \n\r");
    for(uint16_t i=0 ; i<nPayloadLength ; i++)
    {
#ifndef COLERIDGE
      printf("%d ", vectcTxBuff[i]);
#else
      printf("%c", vectcTxBuff[i]);
#endif
    }
    printf("\n\n\r");
#endif

    /* flush the Tx FIFO */
    SpiritCmdStrobeFlushTxFifo();

    /* check if the payload length is higher than the FIFO length */
    if(nPayloadLength>96)
      {
        /* ... if yes transmit data using an AE IRQ and a FIFO reloading mechanism */
        /* set the almost empty threshold */
        SpiritLinearFifoSetAlmostEmptyThresholdTx(cThreholdTxFifoAE);
        SpiritIrq(TX_FIFO_ALMOST_EMPTY , S_ENABLE);

        /* write the linear fifo with the first 96 bytes of payload */
        SpiritSpiWriteLinearFifo(96, vectcTxBuff);

        /* store the number of transmitted bytes */
        nTxIndex = 96;

        /* update the residual number of bytes to be transmitted */
        nResidualPayloadLength -= 96;
      }
    else
      {
        /* .. if not transmit data writing all of them to the Tx FIFO */
        SpiritSpiWriteLinearFifo(nPayloadLength, vectcTxBuff);
      }

    /* send the TX command */
    SpiritCmdStrobeTx();

    /* wait for TX done */
    while(!xTxDoneFlag);
    xTxDoneFlag = RESET;

#ifdef USE_VCOM
    printf("Data in FIFO before writing: [");

      for(uint16_t i=0 ; i<cAECounter ; i++)
      {
        printf("%d ", vect1[i]);
      }
      printf("]\n\r");

      printf("\n\rAE Thr=%d\n\rPayload length=%d\n\n\r ", cThreholdTxFifoAE,nPayloadLength);

#endif

    /* pause between two transmissions */
    SdkDelayMs(500);
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
