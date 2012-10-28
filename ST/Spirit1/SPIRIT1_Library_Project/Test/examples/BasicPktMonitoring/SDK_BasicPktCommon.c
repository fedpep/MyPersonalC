/**
 * @file    SDK_BasicPktCommon.c
 * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
 * @version V3.0.0
 * @date    August 7, 2012
 * @brief   Example of configuration and management of SPIRIT Basic packets.
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
#include "SDK_BasicPktCommon.h"

/**
 * @addtogroup SDK_Examples
 * @{
 */

/**
 * @addtogroup SDK_Basic_packet_monitoring              SDK Basic packet monitoring
 * @{
 */

/**
 * @addtogroup SDK_Basic_packet_monitoring_Common               SDK Basic packet monitoring Common
 * @brief  Common module used to share functionalities between the two sourcecodes of device A and B.
 * @details This code provides some useful functions to manage the menu and the interactions with the
 * user. Every choose is associated with a specific block of code which manages the request interfacing
 * Spirit through APIs taken from the Spirit libraries.
 * @{
 */

/**
 * @defgroup Basic_packet_monitoring_Common_Private_TypesDefinitions            Basic packet monitoring Common Private TypesDefinitions
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
 * @defgroup Basic_packet_monitoring_Common_Private_Defines                     Basic packet monitoring Common Private Defines
 * @{
 */

#ifdef DEVICE_A

  #define DEVICE_NAME "A (Tx)"
  #define MENU_SIZE 9

#endif
#ifdef DEVICE_B

  #define DEVICE_NAME "B (Rx)"
  #define MENU_SIZE 9

#endif


/**
 *@}
 */


/**
 * @defgroup Basic_packet_monitoring_Common_Private_Macros                      Basic packet monitoring Common Private Macros
 * @{
 */

/**
 *@}
 */

/**
 * @defgroup Basic_packet_monitoring_Common_Private_Variables                   Basic packet monitoring Common Private Variables
 * @{
 */

/**
 * @brief Declaration of variables used to manage the V-COM communication
 */
extern uint8_t vectcVcomData[VIRTUAL_COM_PORT_DATA_SIZE];


#ifdef DEVICE_A
/**
 * @brief Fitting of the functions pointer vector
 */
  static void(*s_pFuncMenuHandlerVect[])(void) = {

    updatePreambleLength,       /* 1 */
    updateSyncLength,           /* 2 */
    updateSyncWord,             /* 3 */
    updateLengthSize,           /* 4 */
    updateCrcLength,            /* 5 */
    updateFecMode,              /* 6 */
    updateWhiteningMode,        /* 7 */
    updateControlTx,            /* 8 */
    updateDestinationAddress    /* 9 */
  };

/**
 * @brief Fitting of the string vector
 */
  static char* s_pStringMenuHandlerVect[]={
    "Preamble length",
    "Sync length",
    "Sync word",
    "Length type",
    "CRC mode",
    "FEC",
    "Whitening",
    "Configure the control field to TX",
    "Change the destination address"
  };

#endif


#ifdef DEVICE_B

/**
 * @brief Fitting of the functions pointer vector
 */
  static void(*s_pFuncMenuHandlerVect[])(void) = {

    updatePreambleLength,       /* 1 */
    updateSyncLength,           /* 2 */
    updateSyncWord,             /* 3 */
    updateLengthSize,           /* 4 */
    updateCrcLength,            /* 5 */
    updateFecMode,              /* 6 */
    updateWhiteningMode,        /* 7 */
    updateAddresses,            /* 8 */
    updateControlRx             /* 9 */
  };

/**
 * @brief Fitting of the string vector
 */
  static char* s_pStringMenuHandlerVect[]={
    "Preamble length",
    "Sync length",
    "Sync word",
    "Length type",
    "CRC mode",
    "FEC",
    "Whitening",
    "Change addresses",
    "Configure the control field reference"
  };

#endif


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
 * @defgroup Basic_packet_monitoring_Common_Private_FunctionPrototypes                          Basic packet monitoring Common Private FunctionPrototypes
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup Basic_packet_monitoring_Common_Private_Functions                                   Basic packet monitoring Common Private Functions
 * @{
 */


/**
 * @brief  Packet monitoring enu printing.
 * @param  None
 * @retval None
 */
void pktMonPrintMenu(void)
{
  /* print a menu title */
  printf("\n\rDEVICE %s - BASIC PACKET MENU\n\n\r", DEVICE_NAME);

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
void pktMonReceiveFromVcom(uint8_t *vectcFromVcom)
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
 * @brief  Function called to handle the preamble length update request by the user.
 * @param  None
 * @retval None
 */
void updatePreambleLength(void)
{
  uint8_t cPreambleLength;

  /* print message */
  printf("Insert the preamble length: ");

  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  cPreambleLength = ((uint8_t)atoi((char*)vectcVcomData))<<3;

  /* update the preamble length */
  SpiritPktBasicSetPreambleLength((BasicPreambleLength)cPreambleLength);

  /* print message */
  printf("%d\n\r", cPreambleLength>>3);

}


/**
 * @brief  Function called to handle the sync length update request by the user.
 * @param  None
 * @retval None
 */
void updateSyncLength(void)
{
  uint8_t cSyncLength;

  /* print message */
  printf("Insert the sync length: ");

  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  cSyncLength = ((uint8_t)atoi((char*)vectcVcomData)-1)<<1;

  /* update the sync length */
  SpiritPktBasicSetSyncLength((BasicSyncLength)cSyncLength);

  /* print message */
  printf("%d\n\r", (cSyncLength>>1)+1);

}


/**
 * @brief  Function called to handle the sync word update request by the user.
 * @param  None
 * @retval None
 */
void updateSyncWord(void)
{
  uint32_t lSyncWord;

  /* print message */
  printf("Insert the sync word (in hex): ");

  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer in hex format */
  sscanf((char*)vectcVcomData,"%x",(int*) &lSyncWord);

  /* update the sync word */
  SpiritPktBasicSetSyncWords(lSyncWord,PKT_SYNC_LENGTH_4BYTES);

  /* print message */
  printf("0x%xn\r", lSyncWord);
}


/**
 * @brief  Function called to handle the length size update request by the user.
 * @param  None
 * @retval None
 */
void updateLengthSize(void)
{
  uint8_t cFixvar;

  /* print message */
  printf("Select the length size type:\n\r1.FIXED\n\r2.VARIABLE\n\n\r");

  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  cFixvar = (uint8_t)(atoi((char*)vectcVcomData-1)!=0);

  /* update the length type */
  SpiritPktBasicSetFixVarLength((BasicFixVarLength)cFixvar);

  /* if variable */
  if(cFixvar==1)
  {
    uint8_t cLengthWidth;

    /* get the address field length to be used as an argument for the LengthWidth function*/
    SpiritFunctionalState address=SpiritPktBasicGetAddressField();
    /* get the control field length to be used as an argument for the LengthWidth function*/
    uint8_t control_length=SpiritPktBasicGetControlLength();

    /* print message */
    printf("Insert the length field width: ");

    /* receive the data string */
    pktMonReceiveFromVcom(vectcVcomData);

    /* convert it to an integer */
    cLengthWidth = (uint8_t)atoi((char*)vectcVcomData);

    /* update the length type width */
    SpiritPktBasicSetVarLengthWidth(cLengthWidth,address,(BasicControlLength)control_length);

    /* print message */
    printf("%d\n\r", cLengthWidth);

  }

  /* print newline and carriage return */
  printf("\n\r");
  
}


/**
 * @brief  Function called to handle the CRC mode update request by the user.
 * @note   This function automatically enables the filtering on CRC field if it is not equal to NO_CRC.
 * @param  None
 * @retval None
 */
void updateCrcLength(void)
{
  BasicCrcMode xCrcMode;

  /* print message */
  printf("Select the CRC mode:\n\r1. NO CRC\n\r2. 8 bits with poly 0x07\n\r3. 16 bits with poly 0x8005\n\r4. 16 bits with poly 0x1021\n\r5. 24 bits with poly 0x864CFB\n\n\r");
  
  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  xCrcMode =(BasicCrcMode)((atoi((char*)vectcVcomData)-1)<<5);

  /* update the CRC mode */
  SpiritPktBasicSetCrcMode(xCrcMode);

  /* if the inserted value is not NO_CRC then enable the CRC filtering */
  if(xCrcMode != PKT_NO_CRC)
    SpiritPktBasicFilterOnCrc(S_ENABLE);
  else
    SpiritPktBasicFilterOnCrc(S_DISABLE);

  /* print message */
  printf("%d\n\r", (((uint8_t)xCrcMode)>>5)+1);
  
}


/**
 * @brief  Function called to handle the FEC mode update request by the user.
 * @param  None
 * @retval None
 */
void updateFecMode(void)
{
  SpiritFunctionalState xFec;

  /* print message */
  printf("FEC:\n\r1.Enable\n\r0.Disable\n\r");
  
  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  xFec = (SpiritFunctionalState)(atoi((char*)vectcVcomData)!=0);

  /* update the FEC mode */
  SpiritPktBasicFec(xFec);

  /* print message */
  printf("%d\n\r", xFec);
  
}


/**
 * @brief  Function called to handle the whitening mode update request by the user.
 * @param  None
 * @retval None
 */
void updateWhiteningMode(void)
{
  SpiritFunctionalState xWhitening;

  /* print message */
  printf("Whitening:\n\r1.Enable\n\r0.Disable\n\r");
  
  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  xWhitening = (SpiritFunctionalState)(atoi((char*)vectcVcomData)!=0);

  /* update the whitening mode */
  SpiritPktBasicWhitening(xWhitening);

  /* print message */
  printf("%d\n\r", xWhitening);
  
}


/**
 * @brief  Function called to handle the addresses update request by the user.
 * @param  None
 * @retval None
 */
void updateAddresses(void)
{
  uint8_t cAddress,cAddressFlag;
  SpiritFunctionalState xEnDis;

  /* print message */
  printf("Select which address has to be changed:\n\r1. My address\n\r2. Multicast address\n\r3. Broadcast address\n\r");
  
  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer */
  cAddressFlag = (uint8_t)(atoi((char*)vectcVcomData));

  /* print message: ask for a new address */
  printf("Insert the new address (in hex, 0 to disable the filtering):");
  
  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer in hex format */
  sscanf((char*)vectcVcomData,"%x",(int*) &cAddress);

  /* save an enable/disable flag */
  xEnDis=(SpiritFunctionalState)(cAddress!=0);

  switch(cAddressFlag)
  {
  case 1:
    /* set my address */
    SpiritPktBasicSetMyAddress(cAddress);
    SpiritPktBasicFilterOnMyAddress(xEnDis);
    break;
  case 2:
    /* set the multicast address */
    SpiritPktBasicSetMulticastAddress(cAddress);
    SpiritPktBasicFilterOnMulticastAddress(xEnDis);
    break;
  case 3:
    /* set the broadcast address */
    SpiritPktBasicSetBroadcastAddress(cAddress);
    SpiritPktBasicFilterOnBroadcastAddress(xEnDis);
    break;
  }

  /* payload length config */
  SpiritPktBasicSetPayloadLength(20);

  /* print message */
  printf("0x%x\n\r", cAddress);
  
}


/**
 * @brief  Function called to handle the control mask and reference value update
 *         request by the user.
 * @param  None
 * @retval None
 */
void updateControlRx(void)
{
  uint32_t lControl, lControlMask;

  /* print message */
  printf("Insert the control mask (in hex): ");
  
  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer in hex format */
  sscanf((char*)vectcVcomData,"%x",(long*) &lControlMask);

  /* if control mask is equal to 0x00000000 the disable the control field filtering */
  if(!lControlMask)
  {
    SpiritPktBasicFilterOnControlField(S_DISABLE);
    SpiritPktBasicSetControlLength(PKT_CONTROL_LENGTH_0BYTES);
  }
  /* else set to 4 its length */
  else
  {
    SpiritPktBasicFilterOnControlField(S_ENABLE);
    SpiritPktBasicSetControlLength(PKT_CONTROL_LENGTH_4BYTES);
  }

  /* update the control mask */
  SpiritPktBasicSetCtrlMask(lControlMask);

  /* print a message */
  printf("0x%x\n\r", lControlMask);
  
  /* print a message */
  printf("Insert the control field to be used as a reference (in hex): ");

  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer in hex format */
  sscanf((char*)vectcVcomData,"%x",(long*)&lControl);

  /* update the control reference value */
  SpiritPktBasicSetCtrlReference(lControl);

  /* payload length config */
  SpiritPktBasicSetPayloadLength(20);

  /* print message */
  printf("0x%x\n\r", lControl);

}


/**
 * @brief  Function called to handle the control value to be transmitted update
 *         request by the user.
 * @param  None
 * @retval None
 */
void updateControlTx(void)
{
  uint32_t lControl;

  /* print message */
  printf("Insert the control field to be transmitted (in hex): ");

  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer in hex format */
  sscanf((char*)vectcVcomData,"%x",(long*) &lControl);

  /* if control TX value is equal to 0x00000000 the disable the control field filtering */
  if(!lControl)
  {
    SpiritPktBasicSetControlLength(PKT_CONTROL_LENGTH_0BYTES);
  }
  /* else set to 4 its length */
  else
  {
    SpiritPktBasicSetControlLength(PKT_CONTROL_LENGTH_4BYTES);
  }

  /* update the control TX value */
  SpiritPktBasicSetTransmittedCtrlField(lControl);

  /* payload length config */
  SpiritPktBasicSetPayloadLength(20);

  /* print message */
  printf("0x%x\n\r", lControl);

}


/**
 * @brief  Function called to handle the destination addresses update request by the user.
 * @param  None
 * @retval None
 */
void updateDestinationAddress(void)
{
  uint8_t cAddress;

  /* print message: ask for a new address */
  printf("Insert the new destination address (in hex):");

  /* receive the data string */
  pktMonReceiveFromVcom(vectcVcomData);

  /* convert it to an integer in hex format */
  sscanf((char*)vectcVcomData,"%x",(int*) &cAddress);

  /* write the destination address */
  SpiritPktStackSetDestinationAddress(cAddress);

  /* print message */
  printf("0x%x\n\r", cAddress);

}


/**
 * @brief  Menu choose management.
 * @param  None
 * @retval None
 */
void pktMonMenuManagement(void){
  uint8_t cChoose;

  /* wait for something from VCOM */
  pktMonReceiveFromVcom(vectcVcomData);

  /* if the message is different from 'p' go on, else do nothing and re-print the menu */
  if(vectcVcomData[0] != 'p')
  {
      /* convert to an integer (index of the functions vector) */
      cChoose = atoi((char*)vectcVcomData)-1;

      /* check the index to be lesser than the vector size */
      if(cChoose < MENU_SIZE){
          /* call the desired fucntion */
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

