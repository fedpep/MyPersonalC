/**
* @file    SDK_PingPong_B.c
* @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
* @version V3.0.0
* @date    August 7, 2012
* @brief   Example of transmission and reception of SPIRIT STack packets.
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
* @addtogroup SDK_PingPong                             SDK Ping Pong
* @{
*/

/**
* @addtogroup SDK_PingPong_B                           SDK Ping Pong B
* @brief Device B configured as the second ping-pong player.
* @details This code configures the device as ping-pong player making it switches from the Rx to the Tx state and
* vice-versa.
*
* The user can change the configuration parameters editing the defines at the beginning of the file.
* @{
*/

/**
* @defgroup PingPong_B_Private_TypesDefinitions                Ping Pong B Private TypesDefinitions
* @{
*/

/**
*@}
*/


/**
* @defgroup PingPong_B_Private_Defines                         Ping Pong B Private Defines
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

/*  Packet configuration parameters  */
#define PREAMBLE_LENGTH             	PKT_PREAMBLE_LENGTH_08BYTES
#define SYNC_LENGTH                 	PKT_SYNC_LENGTH_4BYTES
#define SYNC_WORD                   	0x1A2635A8
#define LENGTH_TYPE                	PKT_LENGTH_FIX
#define LENGTH_WIDTH                	6
#define CRC_MODE                    	PKT_CRC_MODE_8BITS
#define CONTROL_LENGTH              	PKT_CONTROL_LENGTH_0BYTES
#define EN_FEC                      	S_DISABLE
#define EN_WHITENING                	S_DISABLE

/*  Addresses configuration parameters  */
#define EN_FILT_MY_ADDRESS          	S_ENABLE
#define MY_ADDRESS                  	0x34
#define EN_FILT_MULTICAST_ADDRESS       S_DISABLE
#define MULTICAST_ADDRESS           	0xEE
#define EN_FILT_BROADCAST_ADDRESS       S_DISABLE
#define BROADCAST_ADDRESS           	0xFF
#define DESTINATION_ADDRESS         	0x44

/* LLP configuration parameters */
#define EN_AUTOACK                  	S_DISABLE
#define EN_PIGGYBACKING             	S_DISABLE
#define MAX_RETRANSMISSIONS         	PKT_DISABLE_RETX

/**
*@}
*/


/**
* @defgroup PingPong_B_Private_Macros                          Ping Pong B Private Macros
* @{
*/

/**
*@}
*/

/**
* @defgroup PingPong_B_Private_Variables                       Ping Pong B Private Variables
* @{
*/

/**
* @brief Packet Stack structure fitting
*/
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


/**
* @brief Address structure fitting
*/
PktStackAddressesInit xAddressInit={
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
PktStackLlpInit xStackLLPInit ={
  EN_AUTOACK,
  EN_PIGGYBACKING,
  MAX_RETRANSMISSIONS
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
* @brief Flags declarations
*/
FlagStatus xRxDoneFlag = RESET,xTxDoneFlag=RESET;

/**
* @brief IRQ status struct declaration
*/
SpiritIrqs xIrqStatus;

/**
* @brief Ball buffer declaration: data to transmit
*/
uint8_t vectcBallBuff[20],
cRxData;

/**
*@}
*/

/**
* @defgroup PingPong_B_Private_FunctionPrototypes                      Ping Pong B Private FunctionPrototypes
* @{
*/

/**
*@}
*/


/**
* @defgroup PingPong_B_Private_Functions                               Ping Pong B Private Functions
* @{
*/

/**
* @brief  This function handles External interrupt request (associated with Spirit GPIO 3).
* @param  None
* @retval None
*/
void M2S_GPIO_3_EXTI_IRQ_HANDLER(void)
{
  /* control the EXTI flag */
  if(EXTI_GetFlagStatus(M2S_GPIO_3_EXTI_LINE))
  {
    /* read the IRQ status bytes */
    SpiritIrqGetStatus(&xIrqStatus);
    
    /* set flags according to the raised IRQ*/
    if(xIrqStatus.IRQ_TX_DATA_SENT)
    {
      xTxDoneFlag = SET;
      SdkEvalLedToggle(LED1);
    }
    if(xIrqStatus.IRQ_RX_DATA_READY)
    {
      xRxDoneFlag = SET;
      SdkEvalLedToggle(LED2);
    }
    
    /* clear the EXTI flag */
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
  SpiritPktStackInit(&xStackInit);
  SpiritPktStackAddressesInit(&xAddressInit);
  SpiritPktStackLlpInit(&xStackLLPInit);
  
  /* Spirit IRQs enable */
  SpiritIrqDeInit(NULL);
  SpiritIrq(TX_DATA_SENT, S_ENABLE);
  SpiritIrq(RX_DATA_READY, S_ENABLE);
  
  /* RX timeout config */
  SET_INFINITE_RX_TIMEOUT();
  SpiritTimerSetRxTimeoutStopCondition(ANY_ABOVE_THRESHOLD);
  
  /* Payload length config */
  SpiritPktStackSetPayloadLength(20);
  
  /* Destination address */
  SpiritPktStackSetDestinationAddress(DESTINATION_ADDRESS);
  
  /* IRQ registers blanking */
  SpiritIrqClearStatus();
  
  /* infinite loop */
  while (1){
    /* RX command */
    SpiritCmdStrobeRx();
    
    /* wait for data received */
    while(!xRxDoneFlag);
    xRxDoneFlag=RESET;
    
    /* when rx data ready read the number of received bytes */
    cRxData=SpiritLinearFifoReadNumElementsRxFifo();
    
    /* read the RX FIFO */
    SpiritSpiReadLinearFifo(cRxData, vectcBallBuff);
    SpiritCmdStrobeFlushRxFifo();
    
    /*  A simple way to control if the received data sequence is correct.
    Use a local block.
    */
    {
      SpiritBool correct=S_TRUE;
      
      /* control all the received bytes and verify that they are sequential
      numbers from 1 to 20. If one of them is not an expected value then
      stop the communication.
      */
      for(uint8_t i=0 ; i<cRxData ; i++)
        if(vectcBallBuff[i] != i+1)
          correct=S_FALSE;
      
#ifdef USE_VCOM
      /* send messages via COM */
      if(correct)
      {
        printf("B Rx data: [");
        for(uint8_t i=0 ; i<cRxData ; i++)
          printf("%d ", vectcBallBuff[i]);
        printf("]\r\n");
      }
      else
      {
        printf("data error! ping pong stopped\n\r");
        
      }
      
#endif
      
      /* stop communication if an incorrect sequence has been received */
      if(!correct){
        SdkEvalLedOn(LED4);
        
        while(1);
      }
    }
    
    /* pause between RX and TX */
    SdkDelayMs(500);
    
    /* fit the TX FIFO */
    SpiritCmdStrobeFlushTxFifo();
    SpiritSpiWriteLinearFifo(20, vectcBallBuff);
    
    /* Spirit Radio config */
    //    SpiritRadioInit(&xRadioInit);
    
    /* send the TX command */
    SpiritCmdStrobeTx();
    
    /* wait for TX done */
    while(!xTxDoneFlag){
      ;
    }
    
    xTxDoneFlag = RESET;
    
#ifdef USE_VCOM
    /* send messages via COM */
    printf("B Tx data: [");
    for(uint8_t i=0 ; i<cRxData ; i++)
      printf("%d ", vectcBallBuff[i]);
    printf("]\n\r");
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
