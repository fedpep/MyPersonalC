/**
 * @file    SDK_EVAL_DFU_Prop.h
 * @author  MSH RF/ART Team IMS-Systems Lab
 * @version V1.0.0
 * @date    03/07/2011
 * @brief   All processings related to DFU.
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
#ifndef __SDK_EVAL_DFU_PROP_H
#define __SDK_EVAL_DFU_PROP_H


/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SDK_EVAL_DFU            SDK EVAL DFU Project
 * @{
 */


/** @defgroup SDK_EVAL_DFU_Processing   SDK EVAL DFU Processing
 * @{
 */


/** @defgroup SDK_EVAL_DFU_Processing_Exported_Types      SDK EVAL DFU Processing Exported Types
 * @{
 */
#define DFU_GetConfiguration          NOP_Process
//#define DFU_SetConfiguration          NOP_Process
#define DFU_GetInterface              NOP_Process
#define DFU_SetInterface              NOP_Process
#define DFU_GetStatus                 NOP_Process
#define DFU_ClearFeature              NOP_Process
#define DFU_SetEndPointFeature        NOP_Process
#define DFU_SetDeviceFeature          NOP_Process
//#define DFU_SetDeviceAddress          NOP_Process

/**
 * @}
 */


/** @defgroup SDK_EVAL_DFU_Processing_Exported_Constants    SDK EVAL DFU Processing Exported Constants
 * @{
 */

/** @defgroup DFU_Requests                                  DFU Requests
 * @{
 */

/**
 * @brief   DFU Requests
 */  
typedef enum _DFU_REQUESTS {
  DFU_DNLOAD = 1,
  DFU_UPLOAD,
  DFU_GETSTATUS,
  DFU_CLRSTATUS,
  DFU_GETSTATE,
  DFU_ABORT
} DFU_REQUESTS;
  
/**
 * @}
 */


/** @defgroup DFU_States                                    DFU States
 * @{
 */

/**
 * @brief   DFU States
 */  
#define STATE_appIDLE                 0
#define STATE_appDETACH               1
#define STATE_dfuIDLE                 2
#define STATE_dfuDNLOAD_SYNC          3
#define STATE_dfuDNBUSY               4
#define STATE_dfuDNLOAD_IDLE          5
#define STATE_dfuMANIFEST_SYNC        6
#define STATE_dfuMANIFEST             7
#define STATE_dfuMANIFEST_WAIT_RESET  8
#define STATE_dfuUPLOAD_IDLE          9
#define STATE_dfuERROR                10

/**
 * @}
 */


/** @defgroup DFU_Status                                    DFU Status
 * @{
 */

/**
 * @brief   DFU Status
 */
#define STATUS_OK                   0x00
#define STATUS_ERRTARGET            0x01
#define STATUS_ERRFILE              0x02
#define STATUS_ERRWRITE             0x03
#define STATUS_ERRERASE             0x04
#define STATUS_ERRCHECK_ERASED      0x05
#define STATUS_ERRPROG              0x06
#define STATUS_ERRVERIFY            0x07
#define STATUS_ERRADDRESS           0x08
#define STATUS_ERRNOTDONE           0x09
#define STATUS_ERRFIRMWARE          0x0A
#define STATUS_ERRVENDOR            0x0B
#define STATUS_ERRUSBR              0x0C
#define STATUS_ERRPOR               0x0D
#define STATUS_ERRUNKNOWN           0x0E
#define STATUS_ERRSTALLEDPKT        0x0F

/**
 * @}
 */


/** @defgroup DFU_States_Manifestation_State                DFU States Manifestation State
 * @{
 */

/**
 * @brief   DFU States Manifestation State
 */
#define Manifest_complete           0x00
#define Manifest_In_Progress        0x01

/**
 * @}
 */


/** @defgroup DFU_Special_Commands_With_Download_Request    DFU Special Commands With Download Request
 * @{
 */

/**
 * @brief   Special Commands With Download Request
 */
#define CMD_GETCOMMANDS              0x00
#define CMD_SETADDRESSPOINTER        0x21
#define CMD_ERASE                    0x41

/**
 * @}
 */

/**
 * @}
 */


/** @defgroup SDK_EVAL_DFU_Processing_Exported_Macros       SDK EVAL DFU Processing Exported Macros
 * @{
 */

/**
 * @}
 */


/** @defgroup SDK_EVAL_DFU_Processing_Exported_Functions     SDK EVAL DFU Processing Exported Functions
 * @{
 */
void DFU_init(void);
void DFU_Reset(void);
void DFU_SetConfiguration(void);
void DFU_SetDeviceAddress (void);
void DFU_Status_In (void);
void DFU_Status_Out (void);
RESULT DFU_Data_Setup(uint8_t);
RESULT DFU_NoData_Setup(uint8_t);
RESULT DFU_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *DFU_GetDeviceDescriptor(uint16_t );
uint8_t *DFU_GetConfigDescriptor(uint16_t);
uint8_t *DFU_GetStringDescriptor(uint16_t);
uint8_t *UPLOAD(uint16_t Length);
uint8_t *DNLOAD(uint16_t Length);
uint8_t *GETSTATE(uint16_t Length);
uint8_t *GETSTATUS(uint16_t Length);
void DFU_write_crc (void);

/**
 * @}
 */


/**
 * @}
 */


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif /* __SDK_EVAL_DFU_PROP_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
