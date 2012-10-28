/**
 * @file    SDK_BasicPktCommon.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDK_PKT_COMMON_H
#define __SDK_PKT_COMMON_H

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
 * @addtogroup SDK_Basic_packet_monitoring              SDK Basic packet monitoring
 * @{
 */

/**
 * @defgroup SDK_Basic_packet_monitoring_Common         SDK Basic packet monitoring Common
 * @{
 */

/**
 * @defgroup Basic_packet_monitoring_Common_Exported_Types              Basic packet monitoring Common Exported Types
 * @{
 */


/**
 *@}
 */


/**
 * @defgroup Basic_packet_monitoring_Common_Exported_Constants          Basic packet monitoring Common Exported Constants
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup Basic_packet_monitoring_Common_Exported_Macros             Basic packet monitoring Common Exported Macros
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup Basic_packet_monitoring_Common_Exported_Functions          Basic packet monitoring Common Exported Functions
 * @{
 */

void pktMonPrintMenu(void);
void pktMonReceiveFromVcom(uint8_t *vectcFromVcom);
void pktMonMenuManagement(void);
void updatePreambleLength(void);
void updateSyncLength(void);
void updateSyncWord(void);
void updateLengthSize(void);
void updateCrcLength(void);
void updateFecMode(void);
void updateWhiteningMode(void);
void updateAddresses(void);
void updateControlRx(void);
void updateControlTx(void);
void updateDestinationAddress(void);

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
