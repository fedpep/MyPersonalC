/**
*
* @file    usb_conf.h
* @author  MCD Application Team
* @version V3.3.0RC1
* @date    07/14/2010
* @brief   Device Firmware Upgrade (DFU) configuration file
*
* @details
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
* <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
*
*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF_H
#define __USB_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* These timings will be returned to the host when it checks the device
   status during a write or erase operation to know how much time the host
   should wait before issuing the next get status request. 
   These defines are set in usb_conf.h file.
   The values of this table should be extracted from relative memories 
   datasheet (Typical or Maximum timming value for Sector Erase and for 
   1024 bytes Write). All timings are expressed in millisecond unit (ms).
   Note that "Sector" refers here to the memory unit used for Erase/Write 
   operations. It could be a sector, a page, a block, a word ...
   If the erase operation is not supported, it is advised to set the erase
   timing to 1 (which means 1ms: one USB frame). */
   
 #define INTERN_FLASH_SECTOR_ERASE_TIME 100
 #define INTERN_FLASH_SECTOR_WRITE_TIME 104   

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/


/** @addtogroup SDK_EVAL_DFU_Program           SDK DFU EVAL Program
  * @{
  */


/** @defgroup SDK_EVAL_DFU_Configuration_File       SDK EVAL DFU Configuration File
  * @{
  */


/**
  *  \brief defines how many endpoints are used by the device
  *  \code
  *  EP_NUM 
  *  \endcode
  */
#define EP_NUM                          (1)

#ifndef STM32F10X_CL

/**
*  \brief Buffer Description Table
*  \code
*  BTABLE_ADDRESS 
*  \endcode
*/
#define BTABLE_ADDRESS      (0x00)

/**
*  \brief rx buffer base address
*  \code
*  ENDP0_RXADDR
*  \endcode
*/
#define ENDP0_RXADDR        (0x10)
/**
*  \brief rx buffer base address
*  \code
*  ENDP0_RXADDR
*  \endcode
*/
#define ENDP0_TXADDR        (0x50)


/**
*  \brief mask defining which events has to be handled by the device application software
*  \code
*  IMR_MSK 
*  \endcode
*/

#define IMR_MSK (CNTR_CTRM  | \
                 CNTR_WKUPM | \
                 CNTR_SUSPM | \
                 CNTR_ERRM  | \
                 CNTR_SOFM  | \
                 CNTR_ESOFM | \
                 CNTR_RESETM  \
                )
#endif /* STM32F10X_CL */


#ifdef STM32F10X_CL

/**
*  \brief FIFO Size Configuration
*  \code
*                             FIFO Size Configuration
*  
*  (i) Dedicated data FIFO SPRAM of 1.25 Kbytes = 1280 bytes = 320 32-bits words
*      available for the endpoints IN and OUT.
*      Device mode features:
*      -1 bidirectional CTRL EP 0
*      -3 IN EPs to support any kind of Bulk, Interrupt or Isochronous transfer
*      -3 OUT EPs to support any kind of Bulk, Interrupt or Isochronous transfer
*
*  ii) Receive data FIFO size = RAM for setup packets + 
*                   OUT endpoint control information +
*                   data OUT packets + miscellaneous
*      Space = ONE 32-bits words
*     --> RAM for setup packets = 4 * n + 6 space
*        (n is the nbr of CTRL EPs the device core supports) 
*     --> OUT EP CTRL info      = 1 space
*        (one space for status information written to the FIFO along with each 
*        received packet)
*     --> data OUT packets      = (Largest Packet Size / 4) + 1 spaces 
*        (MINIMUM to receive packets)
*     --> OR data OUT packets  = at least 2*(Largest Packet Size / 4) + 1 spaces 
*        (if high-bandwidth EP is enabled or multiple isochronous EPs)
*     --> miscellaneous = 1 space per OUT EP
*        (one space for transfer complete status information also pushed to the 
*        FIFO with each endpoint's last packet)
*
*  (iii)MINIMUM RAM space required for each IN EP Tx FIFO = MAX packet size for 
*       that particular IN EP. More space allocated in the IN EP Tx FIFO results
*       in a better performance on the USB and can hide latencies on the AHB.
*
*  (iv) TXn min size = 16 words. (n  : Transmit FIFO index)
*   (v) When a TxFIFO is not used, the Configuration should be as follows: 
*       case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
*       --> Txm can use the space allocated for Txn.
*       case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
*       --> Txn should be configured with the minimum space of 16 words
*  (vi) The FIFO is used optimally when used TxFIFOs are allocated in the top 
*       of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
*  \endcode
*/



#define RX_FIFO_SIZE                          128
#define TX0_FIFO_SIZE                          64
#define TX1_FIFO_SIZE                          64
#define TX2_FIFO_SIZE                          16
#define TX3_FIFO_SIZE                          16


/* OTGD-FS-DEVICE IP interrupts Enable definitions */
/* Uncomment the define to enable the selected interrupt */
//#define INTR_MODEMISMATCH
#define INTR_SOFINTR
#define INTR_RXSTSQLVL           /* Mandatory */
//#define INTR_NPTXFEMPTY
//#define INTR_GINNAKEFF
//#define INTR_GOUTNAKEFF
//#define INTR_ERLYSUSPEND
#define INTR_USBSUSPEND          /* Mandatory */
#define INTR_USBRESET            /* Mandatory */
#define INTR_ENUMDONE            /* Mandatory */
//#define INTR_ISOOUTDROP
//#define INTR_EOPFRAME
//#define INTR_EPMISMATCH
#define INTR_INEPINTR            /* Mandatory */
#define INTR_OUTEPINTR           /* Mandatory */
//#define INTR_INCOMPLISOIN
//#define INTR_INCOMPLISOOUT
#define INTR_WKUPINTR            /* Mandatory */

/* OTGD-FS-DEVICE IP interrupts subroutines */
/* Comment the define to enable the selected interrupt subroutine and replace it
   by user code */
#define  INTR_MODEMISMATCH_Callback      NOP_Process
#define  INTR_SOFINTR_Callback           NOP_Process
#define  INTR_RXSTSQLVL_Callback         NOP_Process
#define  INTR_NPTXFEMPTY_Callback        NOP_Process
#define  INTR_NPTXFEMPTY_Callback        NOP_Process
#define  INTR_GINNAKEFF_Callback         NOP_Process
#define  INTR_GOUTNAKEFF_Callback        NOP_Process
#define  INTR_ERLYSUSPEND_Callback       NOP_Process
#define  INTR_USBSUSPEND_Callback        NOP_Process
#define  INTR_USBRESET_Callback          NOP_Process
#define  INTR_ENUMDONE_Callback          NOP_Process
#define  INTR_ISOOUTDROP_Callback        NOP_Process
#define  INTR_EOPFRAME_Callback          NOP_Process
#define  INTR_EPMISMATCH_Callback        NOP_Process
#define  INTR_INEPINTR_Callback          NOP_Process
#define  INTR_OUTEPINTR_Callback         NOP_Process
#define  INTR_INCOMPLISOIN_Callback      NOP_Process
#define  INTR_INCOMPLISOOUT_Callback     NOP_Process
#define  INTR_WKUPINTR_Callback          NOP_Process

/* Isochronous data update */
#define  INTR_RXSTSQLVL_ISODU_Callback   NOP_Process  

/* Isochronous transfer parameters */
/* Size of a single Isochronous buffer (size of a single transfer) */
#define ISOC_BUFFER_SZE                  1
/* Number of sub-buffers (number of single buffers/transfers), should be even */
#define NUM_SUB_BUFFERS                  2

#endif /* STM32F10X_CL */

/* CTR service routines */
/* associated to defined endpoints */
#define  EP1_IN_Callback   NOP_Process
#define  EP2_IN_Callback   NOP_Process
#define  EP3_IN_Callback   NOP_Process
#define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process


#define  EP1_OUT_Callback   NOP_Process
#define  EP2_OUT_Callback   NOP_Process
#define  EP3_OUT_Callback   NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process



/**
  * @}
  */ 

/**
  * @}
  */ 



#endif /*__USB_CONF_H*/

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/




