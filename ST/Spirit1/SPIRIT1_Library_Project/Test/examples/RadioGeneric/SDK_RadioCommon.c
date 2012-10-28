/**
 * @file    SDK_RadioCommon.c
 * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
 * @version V3.0.0
 * @date    August 7, 2012
 * @brief   Example of configuration and management of SPIRIT Radio.
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
#include "SDK_RadioCommon.h"

/**
 * @addtogroup SDK_Examples
 * @{
 */

/**
 * @addtogroup SDK_Radio                        SDK Radio
 * @{
 */

/**
 * @addtogroup SDK_Radio_Common                 SDK Radio Common
 * @brief Common module used to share functionalities between the two sourcecodes of device A and B.
 * @details This code provides some useful functions to manage the menu and the interactions with the
 * user. Every choose is associated with a specific block of code which manages the request interfacing
 * Spirit through APIs taken from the Spirit libraries.
 *
 * @{
 */

/**
 * @addtogroup Radio_Common_Private_TypesDefinitions            Radio Common Private TypesDefinitions
 * @{
 */

/**
 * @brief typedef used to print and manage the menu
 */
typedef struct{
  char** pMenuStrings;                  /*!<  menu strings array to be displayed */
  void(**pMenuHandler)(void);           /*!<  menu handler functions pointers array */
} MenuHandler;

/**
 *@}
 */


/**
 * @addtogroup Radio_Common_Private_Defines                     Radio Common Private Defines
 * @{
 */

#define MENU_SIZE 7

/**
 *@}
 */


/**
 * @addtogroup Radio_Common_Private_Macros                      Radio Common Private Macros
 * @{
 */

/**
 *@}
 */

/**
 * @addtogroup Radio_Common_Private_Variables                   Radio Common Private Variables
 * @{
 */

/* Declaration of variables used to manage the V-COM communication */
extern uint8_t vectcVcomData[VIRTUAL_COM_PORT_DATA_SIZE];
//extern uint8_t g_vectcVCRxBuffer[VIRTUAL_COM_PORT_DATA_SIZE];
//extern uint32_t g_lVCNbBytesReceived;

/**
 * @brief Static MenuHandler variable to manage the menu is fitted with the
 *        previously defined vectors.
 */

/**
 * @brief Fitting of the functions pointer vector
 */
static void(*s_pFuncMenuHandlerVect[])(void) = {
  updateBaseFrequency,
  updateChannelSpace,
  updateChannel,
  updateModulation,
  updateDatarate,
  updateFrequencyDev,
  updateFilterBandwidth
};


/**
 * @brief Fitting of the string vector
 */
static char* s_pStringMenuHandlerVect[]={
    "Base frequency",
    "Channel space",
    "Channel number",
    "Modulation select",
    "Datarate",
    "Frequency deviation",
    "Filter bandwidth"
};


/**
 * @brief Static MenuHandler variable to manage the menu is fitted with the
 *        previously defined vectors.
 */
static MenuHandler s_xMenuHandler={
  s_pStringMenuHandlerVect,
  s_pFuncMenuHandlerVect
};

/**
 *@}
 */

/**
 * @addtogroup Radio_Common_Private_FunctionPrototypes                          Radio Common Private FunctionPrototypes
 * @{
 */

/**
 *@}
 */


/**
 * @addtogroup Radio_Common_Private_Functions                                   Radio Common Private Functions
 * @{
 */


/**
 * @brief  Radio menu printing.
 * @param  None
 * @retval None
 */
void radioPrintMenu(void)
{
 printf("\n\rRADIO CONFIGURATION\n\r");

 /* call printf for every string in the s_xMenuHandler.pMenuStrings vector */
  for(uint8_t i=0 ; i<MENU_SIZE ; i++)
    printf("%d. %s\n\r",i+1, s_xMenuHandler.pMenuStrings[i]);

 printf("\n\r");

}


/**
 * @brief  Virtual com string receiver. It waits for a string received through VCOM
 *         searching the '\r' char.
 * @param  vectcFromVcom: string to be received.
 * @retval None
 */
void radioReceiveFromVcom(uint8_t *vectcFromVcom)
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
 * @brief  Function called to handle the base frequency update request by the user.
 * @param  None
 * @retval None
 */
void updateBaseFrequency(void)
{
  uint32_t lBaseFreq;

  /* print message */
  printf("Insert the base frequency: ");

  /* receive the data string */
  radioReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  lBaseFreq = (uint32_t)atof((char*)vectcVcomData);

  /* update the frequency base */
  SpiritRadioSetFrequencyBase(lBaseFreq);

  /* print message */
  printf("%d\n\r", lBaseFreq);

}


/**
 * @brief  Function called to handle the channel space update request by the user.
 * @param  None
 * @retval None
 */
void updateChannelSpace(void)
{
  uint32_t lChSpace;

  /* print message */
  printf("Insert the channel space: ");

  /* receive the data string */
  radioReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  lChSpace = (uint32_t)atof((char*)vectcVcomData);

  /* update the channel space */
  SpiritRadioSetChannelSpace(lChSpace);

  /* print message */
  printf("%d\n\r", lChSpace);

}


/**
 * @brief  Function called to handle the channel update request by the user.
 * @param  None
 * @retval None
 */
void updateChannel(void)
{
  uint8_t cChannel;

  /* print message */
  printf("Insert the channel number: ");

  /* receive the data string */
  radioReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  cChannel = (uint8_t)atof((char*)vectcVcomData);

  /* update the channel number */
  SpiritRadioSetChannel(cChannel);

  /* print message */
  printf("%d\n\r", cChannel);

}


/**
 * @brief  Function called to handle the modulation scheme update request by the user.
 * @param  None
 * @retval None
 */
void updateModulation(void)
{
  uint8_t cModulation;
  char vectcModulationStr[21];

  /* print message */
  printf("Select the modulation scheme:\n\r1.FSK\n\r2.GFSK (BT=0.5)\n\r3.GFSK (BT=1)\n\r4.ASK/OOK\n\r5.MSK\n\n\r");

  /* receive the data string */
  radioReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  cModulation = (uint8_t)atof((char*)vectcVcomData);

  /* select the modulation type */
  switch(cModulation)
  {
  case 2:
    cModulation=GFSK_BT05;
    strcpy(vectcModulationStr,"GFSK (BT=0.5)");
    break;
  case 3:
    cModulation=GFSK_BT1;
    strcpy(vectcModulationStr,"GFSK (BT=1)");
    break;
  case 4:
    cModulation=ASK_OOK;
    strcpy(vectcModulationStr,"ASK/OOK");
    break;
  case 5:
    cModulation=MSK;
    strcpy(vectcModulationStr,"MSK");
    break;
  default:
    cModulation=FSK;
    strcpy(vectcModulationStr,"FSK");
    break;
  }

  /* update the channel space */
  SpiritRadioSetModulation((ModulationSelect)cModulation);

  /* print message */
  printf("%s\n\r",vectcModulationStr);

}


/**
 * @brief  Function called to handle the datarate update request by the user.
 * @param  None
 * @retval None
 */
void updateDatarate(void)
{
  uint32_t lDatarate;

  /* print message */
  printf("Insert the datarate: ");

  /* receive the data string */
  radioReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  lDatarate = (uint32_t)atof((char*)vectcVcomData);

  /* update the datarate */
  SpiritRadioSetDatarate(lDatarate);

  /* print message */
  printf("%d\n\r", lDatarate);

}


/**
 * @brief  Function called to handle the frequency deviation update request by the user.
 * @param  None
 * @retval None
 */
void updateFrequencyDev(void)
{
  uint32_t lFdev;

  /* print message */
  printf("Insert the frequency deviation: ");

  /* receive the data string */
  radioReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  lFdev = (uint32_t)atof((char*)vectcVcomData);

  /* update the frequency deviation */
  SpiritRadioSetFrequencyDev(lFdev);

  /* print message */
  printf("%d\n\r", lFdev);
}


/**
 * @brief  Function called to handle the filter bandwidth update request by the user.
 * @param  None
 * @retval None
 */
void updateFilterBandwidth(void)
{
  uint32_t lFbandwidth;

  /* print message */
  printf("Insert the filter bandwidth: ");

  /* receive the data string */
  radioReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  lFbandwidth = (uint32_t)atof((char*)vectcVcomData);

  /* update the filter bandwidth */
  SpiritRadioSetChannelBW(lFbandwidth);

  /* print message */
  printf("%d\n\r", lFbandwidth);
}


/**
 * @brief  Menu choose management.
 * @param  None
 * @retval None
 */
void radioMenuManagement(void){
  uint8_t cChoose;

  /* receive the data string */
  radioReceiveFromVcom(vectcVcomData);

  /* if the message is different from 'p' go on, else do nothing and re-print the menu */
  if(vectcVcomData[0] != 'p')
  {
    /* convert to an integer (index of the functions vector) */
    cChoose = atoi((char*)vectcVcomData)-1;

    /* check the index to be lesser than the vector size */
    if(cChoose < MENU_SIZE){
      /* call the desired function */
      (s_xMenuHandler.pMenuHandler[cChoose])();

      return;
    }
    else
    {
      /* if this point is reached print a bad choose message */
      printf("Bad choose.\n\r");
    }
  }
}

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


