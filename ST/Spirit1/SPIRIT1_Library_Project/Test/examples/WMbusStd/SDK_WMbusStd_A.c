/**
 * @file    SDK_WMbusStd_A.c
 * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
 * @version V3.0.0
 * @date    August 7, 2012
 * @brief   Example of transmission of WMBUS N packets.
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
#include "SPIRIT_Management.h"

#define USE_VCOM

//#define SUBMODE_N

#ifdef USE_VCOM
#include "SDK_EVAL_VC_General.h"
#endif

/**
 * @addtogroup SDK_Examples SDK Examples
 * @{
 */

/**
 * @addtogroup SDK_WMbusStd                                SDK WMbus Std
 * @{
 */

/**
 * @addtogroup SDK_WMbusStd_A                                      SDK WMbus Std A
 * @brief Device A configured as a transmitter.
 * @details This code explains how to configure and manage
 * in the simpler way a transmitter with the WMBUS standard protocol.
 *
 * @{
 */

/**
 * @defgroup WMbusStd_A_Private_TypesDefinitions                   WMbus Std A Private TypesDefinitions
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup WMbusStd_A_Private_Defines                            WMbus Std A Private Defines
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

#define POWER_DBM                   0.0

#define PAYLOAD_LENGTH_FIX 256


#define CRC_POLYNOM 0x3D65

#ifndef SUBMODE_N

/*  Packet configuration parameters  */
#define MBUS_SUBMODE                    MBUS_SUBMODE_S1_S2_LONG_HEADER
#define PREAMBLE_LENGTH                 36
#define POSTAMBLE_LENGTH                16

#endif

/**
 *@}
 */


/**
 * @defgroup WMbusStd_A_Private_Macros                                     WMbus Std A Private Macros
 * @{
 */

#define CEILING(X) (X-(uint16_t)(X) > 0 ? (uint16_t)(X+1) : (uint16_t)(X))

/**
 *@}
 */

/**
 * @defgroup WMbusStd_A_Private_Variables                                  WMbus Std A Private Variables
 * @{
 */

/**
 * @brief GPIO structure fitting
 */
SGpioInit xGpioIRQ={
  SPIRIT_GPIO_3,
  SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
  SPIRIT_GPIO_DIG_OUT_IRQ
};

#ifndef SUBMODE_N

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
#ifdef USE_VCOM
uint8_t vectcVcomData[VIRTUAL_COM_PORT_DATA_SIZE];
extern uint8_t g_vectcVCRxBuffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern uint32_t g_lVCNbBytesReceived;
#endif

uint8_t cLField=80;
uint8_t cThreholdTxFifoAE = 48;
uint8_t vectcTxBuff[300];
uint16_t nDataToTx, nDataLength,nTxData, nResidualPayloadLength, nPayloadLength, nTxIndex;

uint8_t vect1[100],vect2[100];
uint8_t cAECounter;


/**
 *@}
 */

/**
 * @defgroup WMbusStd_A_Private_FunctionPrototypes                                         WMbus Std A Private FunctionPrototypes
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup WMbusStd_A_Private_Functions                                                  WMbus Std A Private Functions
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


uint16_t computeCrc(uint8_t *pStart, uint8_t cNBytes)
{
  uint16_t  seed= 0x0000;

  for(uint8_t i=0; i<cNBytes ; i++)
  {
    seed = crcCalc(seed, pStart[i]);
  }
  seed =~seed;
  return seed;
}


void fitTxBuffer()
{
  uint16_t nCrc, j=0, maxIndex, nFittedData;
  
#ifdef SUBMODE_N 
  vectcTxBuff[j++] = 0x55;
  vectcTxBuff[j++] = 0x55;
  vectcTxBuff[j++] = 0xF6;
  vectcTxBuff[j++] = 0x8D;
#endif
  
  vectcTxBuff[j++] = cLField;   // L  field
  
  for(uint8_t i=0 ; i<9 ; i++)
  {
    vectcTxBuff[i+j] = i+1;
  }
  
  nCrc = computeCrc(&vectcTxBuff[j-1],10);
  j+=9;

  vectcTxBuff[j++] = (uint8_t)(nCrc>>8);
  vectcTxBuff[j++] = (uint8_t) nCrc;
  nFittedData=9;
  
  while(nFittedData<cLField)
  {
    if((cLField-nFittedData)/16 != 0)
      maxIndex = 16;
    else
      maxIndex = (cLField-nFittedData)%16;
      
    for(uint8_t i=0 ; i<maxIndex ; i++)
    {
      vectcTxBuff[j++] = i+1;
      nFittedData++;
    }
    
    nCrc = computeCrc(&vectcTxBuff[j-maxIndex],maxIndex);
    vectcTxBuff[j++] = (uint8_t)(nCrc>>8);
    vectcTxBuff[j++] = (uint8_t) nCrc;
  }
}


/**
 * @brief  This function handles External interrupt request (associated with Spirit GPIO 0).
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

          /* fill the Tx fifo */
          SpiritSpiWriteLinearFifo(nResidualPayloadLength, &vectcTxBuff[nTxIndex]);
          
          /* unarm the AE threshold mechanism */
          SpiritLinearFifoSetAlmostEmptyThresholdTx(96);
          
          /* update the number of bytes to be transmitted */
          nResidualPayloadLength = 0;
          
          /* update the number of transmitted bytes */
          nTxIndex += nResidualPayloadLength;
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

#ifdef SUBMODE_N
  /* Spirit Direct Tx config */
  SpiritDirectRfSetTxMode(DIRECT_TX_FIFO_MODE);
#else
  /* Spirit Packet config */
  SpiritPktMbusInit(&xMbusInit);
#endif
  
  /* Spirit IRQs enable */
  SpiritIrqDeInit(NULL);
  SpiritIrq(TX_DATA_SENT , S_ENABLE);
  SpiritIrq(TX_FIFO_ALMOST_EMPTY , S_ENABLE);

  /* IRQ registers blanking */
  SpiritIrqClearStatus();

  EXTI_ClearITPendingBit(M2S_GPIO_3_EXTI_LINE);
  SdkEvalM2SGpioInterruptCmd(M2S_GPIO_3, 0x0F, 0x0F, ENABLE);

  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

  
  /* infinite loop */
  while (1){
    
    if(cLField>9)
      nPayloadLength = cLField+2+2*(CEILING(((float)cLField-9.0)/16.0));
    else
      nPayloadLength = cLField+2;
  
    nPayloadLength += 5;
      
    /* make a copy of the payload length (to be used to have the number of the residual bytes to transmit) */
    nResidualPayloadLength = nPayloadLength;

    /* Index TX buffer */
    nTxIndex = 0;

    /* reset the counter of the number of almost empty IRQs*/  
    cAECounter=0;
    
    /* fit the packet payload with data */
    fitTxBuffer();

#ifndef SUBMODE_N
    /* payload length config */
    SpiritPktMbusSetPayloadLength(nPayloadLength);
#endif
    
#ifdef USE_VCOM
    printf("A data to transmit: [");
    for(uint16_t i=0 ; i<nPayloadLength-4 ; i++)
    {
      printf("%d ", vectcTxBuff[i]);
    }
    printf("]\r\n");

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
      printf("vect1 [");
      for(uint16_t i=0 ; i<cAECounter ; i++)
      {
        printf("%d ", vect1[i]);
      }
      printf("]\r\n");

      printf("vect2 [");
      for(uint16_t i=0 ; i<cAECounter ; i++)
      {
        printf("%d ", vect2[i]);
      }
      printf("]\r\n");

      printf("\r\n%d\r\n%d\r\n ", cThreholdTxFifoAE,nPayloadLength);
#endif

    /* pause between two transmissions */
    SdkDelayMs(1000);
    
    /* increment the payload length (overflowing it at PAYLOAD_LENGTH_FIX) */
    cLField++;

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
