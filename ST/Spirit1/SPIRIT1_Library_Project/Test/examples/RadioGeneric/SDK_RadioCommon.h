/**
 * @file    SDK_RadioCommon.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDK_RADIO_COMMON_H
#define __SDK_RADIO_COMMON_H

/* Includes ------------------------------------------------------------------*/
#include "SDK_EVAL_Config.h"
#include "SPIRIT_Config.h"
#include "stm32l1xx.h"
#include <stdlib.h>
#include <string.h>
#include "SDK_EVAL_VC_General.h"

#ifdef __cplusplus
 extern "C" {
#endif

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
 * @{
 */


/**
 * @defgroup Radio_Common_Exported_Types                Radio Common Exported Types
 * @{
 */


/**
 *@}
 */


/**
 * @defgroup Radio_Common_Exported_Constants            Radio Common Exported Constants
 * @{
 */



/**
 *@}
 */


/**
 * @defgroup Radio_Common_Exported_Macros               Radio Common Exported Macros
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup Radio_Common_Exported_Functions            Radio Common Exported Functions
 * @{
 */

void radioReceiveFromVcom(uint8_t *vectcFromVcom);
void radioMenuManagement(void);
void radioPrintMenu(void);
void updateBaseFrequency(void);
void updateChannelSpace(void);
void updateChannel(void);
void updateModulation(void);
void updateDatarate(void);
void updateFrequencyDev(void);
void updateFilterBandwidth(void);


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

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
