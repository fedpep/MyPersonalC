/**
* @file    SDK_LDCR_B.c
* @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
* @version V3.0.0
* @date    August 7, 2012
* @brief   Example of reception of SPIRIT Basic packets in LDCR mode.
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
* @addtogroup SDK_LDCR                         SDK LDCR
* @{
*/

/**
* @addtogroup SDK_LDCR_B                       SDK LDCR B
* @brief Device B configured as a receiver.
* @details This code shows a synchronization method between two devices in order to obtain a low
* duty cycle communication and, as a matter of fact, save power. This code runs on the receiver side.
*
* The user is allowed to change the algorithm parameters editing the defines at the beginning of the file.
* @{
*/

/**
* @defgroup LDCR_B_Private_TypesDefinitions            LDCR B Private TypesDefinitions
* @{
*/

/**
*@}
*/


/**
* @defgroup LDCR_B_Private_Defines                     LDCR B Private Defines
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
#define DATARATE                    38400
#define FREQ_DEVIATION              20e3
#define BANDWIDTH                   100.5E3

/*  Packet configuration parameters  */
#define PREAMBLE_LENGTH             PKT_PREAMBLE_LENGTH_04BYTES
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

/* phase reference */
#define PHASE_REF                   8
/* max number of empty Tx slots before sending a dummy packet */
#define MAX_NO_TX                   15

/**
*@}
*/


/**
* @defgroup LDCR_B_Private_Macros                              LDCR B Private Macros
* @{
*/


/**
*@}
*/

/**
* @defgroup LDCR_B_Private_Variables                           LDCR B Private Variables
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


///**
//* @brief GPIO for Rx test signal structure fitting
//*/
//SGpioInit xGpio1Rx=
//{
//  SPIRIT_GPIO_1,
//  SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
//  SPIRIT_GPIO_DIG_OUT_RX_STATE
//};
//
///**
//* @brief GPIO for Sleep test signal structure fitting
//*/
//SGpioInit xGpio0Sleep=
//{
//  SPIRIT_GPIO_0,
//  SPIRIT_GPIO_MODE_DIGITAL_OUTPUT_LP,
//  SPIRIT_GPIO_DIG_OUT_SLEEP_OR_STANDBY
//};

/**
* @brief Rx buffer declaration: how to store the received data
*/
uint8_t vectcRxBuff[96], cRxData;



/**
* @brief Flags declarations
*/
FlagStatus xRxDataReadyFlag=RESET,
xFirstReception=RESET;


/**
* @brief IRQ status struct declaration
*/
SpiritIrqs xIrqStatus;

/**
*@}
*/

/**
* @defgroup LDCR_B_Private_FunctionPrototypes                          LDCR B Private FunctionPrototypes
* @{
*/

/**
*@}
*/


/**
* @defgroup LDCR_B_Private_Functions                                   LDCR B Private Functions
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
    
    /* Check the SPIRIT RX_DATA_READY IRQ flag */
    if(xIrqStatus.IRQ_RX_DATA_READY)
    {
      /* Get the RX FIFO size */
      cRxData = SpiritLinearFifoReadNumElementsRxFifo();
      
      /* Read the RX FIFO */
      SpiritSpiReadLinearFifo(cRxData, vectcRxBuff);
      
#ifdef USE_VCOM
      /* print the received data */
      printf("B data received: [");
      for(uint8_t i=0 ; i<cRxData ; i++)
        printf("%d ", vectcRxBuff[i]);
      printf("]\r\n");
#endif
      /* Set the rx data ready flag */
      xRxDataReadyFlag=SET;
      
      SdkEvalLedToggle(LED3);
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
  
  /* Start SysTick */
  SdkStartSysTick();
  
  /* Spirit SPI config */
  SpiritSpiInit();
  
  /* Reset Spirit registers to default */
  SpiritCmdStrobeSres();
  
  /* Compute the Spirit version and XTAL frequency */
  SpiritManagementIdentificationRFBoard();
  
  SpiritCmdStrobeSres();
  
  SdkEvalM2SGpioInit(M2S_GPIO_3,M2S_MODE_EXTI_IN);
  SdkEvalM2SGpioInterruptCmd(M2S_GPIO_3,0x0F,0x0F,ENABLE);
  
  /* Spirit IRQ config */
  SpiritGpioInit(&xGpioIRQ);
  
  /* Configure some output signals (to be probed with a scope to verify) */
//  SpiritGpioInit(&xGpio1Rx);
//  SpiritGpioInit(&xGpio0Sleep);
  
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
  SpiritCalibrationRco(S_ENABLE);
  SdkEvalLedOn(LED1);
  
  /* Spirit Packet config */
  SpiritPktBasicInit(&xBasicInit);
  
  /* IRQ registers blanking */
  SpiritIrqClearStatus();
  
  SpiritQiSetSqiThreshold(SQI_TH_0);
  SpiritQiSqiCheck(S_ENABLE);
  
  /* Spirit IRQs enable */
  SpiritIrqDeInit(&xIrqStatus);
  SpiritIrq(RX_DATA_READY,S_ENABLE);
//  SpiritIrq(VALID_SYNC,S_ENABLE);
//  SpiritIrq(RX_TIMEOUT,S_ENABLE);
//  SpiritIrq(RX_DATA_DISC,S_ENABLE);
  
  /* Payload length config */
  SpiritPktBasicSetPayloadLength(20);
  
  /* RX timeout config */
  SET_INFINITE_RX_TIMEOUT();
  SpiritTimerSetRxTimeoutStopCondition(SQI_ABOVE_THRESHOLD);
  
  float fWuTimer;
  uint8_t cWuPrescaler,cWuCounter,cWuReloadCounter;
  
  /* Set the wake Up event every 100 ms */
  SpiritTimerSetWakeUpTimerMs(100.0);
  
  /* Get the counter and the prescaler of the WU timer */
  SpiritTimerGetWakeUpTimer(&fWuTimer,&cWuCounter,&cWuPrescaler);
  
  /*
  Compute the rephasing timer to be reloaded on sync.
  This value will be equal to: WAKE_UP_TIMER - (PREAMB_TIME + SYNC_TIME) - GUARD_TIME,
  where:
  - (PREAMB_TIME + SYNC_TIME) is the time needed to transmit preamble and sync.
  - GUARD_TIME is a security bound to make the Rx awake before the Tx.
  */
  float fReloadMs = 100.0-1000.0*(float)(SpiritPktBasicGetPreambleLength()+SpiritPktBasicGetSyncLength())*8.0/DATARATE-10.0;
  
  /* In order to have not lack of accuracy it is recommended that the reload timer
  has the same prescaler of the WakeUp timer.
  So compute the counter using the WU prescaler previously computed.
  */
  cWuReloadCounter=(uint8_t)(1000.0*fReloadMs/28.818/cWuPrescaler);
  
  /* Set the Wake up reload timer */
  SpiritTimerSetWakeUpTimerReload(cWuReloadCounter,cWuPrescaler);
  
  /* Enable the auto reload on sync */
  SpiritTimerLdcrAutoReload(S_ENABLE);
  
  /* Enable the LDCR mode */
  SpiritTimerLdcrMode(S_ENABLE);
  
  SpiritCmdStrobeFlushRxFifo();
  
  /* Set Spirit in Rx state */
  SpiritCmdStrobeRx();
  
  /* infinite loop */
  while (1){
    
    while(!xRxDataReadyFlag);
    xRxDataReadyFlag=RESET;
      
    if(xFirstReception==RESET)
    {
      SpiritTimerSetRxTimeoutMs(80.0);
      
      xFirstReception=SET;
      SdkEvalLedToggle(LED2);
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
