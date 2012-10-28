/**
  @page Basic_Packets_monitoring_example Basic Packets monitoring example readme

  @verbatim
  ******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
  * @file    examples/BasicPktMonitoring/SDK_BasicPktMonitoring_readme.txt
  * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
  * @version V2.0.2
  * @date    Febrary 7, 2012
  * @brief   Basic packet configuration example.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
   @endverbatim

@par Example Description

This example explains how to configure one node as a transmitter and
the other as a receiver in order to do a simple basic packet
transmission focusing on the radio configuration.
It is made up of four files: the transmitter code, the receiver code
and a module which consists of functions called by the two programs.
These functions deal with the menu and the VCOM management for the user
choice.
The two programs implement a transmitter (A) and a receiver (B) of Basic
packets with numbers from 1 to 20 for payload.
The user can change the basic packets parameters during runtime for both the devices.
Every time A performs a transmission it toggles the LED1.
B communicates his state to the user by toggling its leds:
- LED3 to say that a reception has been done
- LED4 to say that a sync word has been detected
- LED2 to say that a packet has been discarded
- LED1 to say that the RX timeout (set to 1 sec) has expired
- LED5 to say that the received sequence is the expected one (integer numbers 1 to 20)
So, the user can easily understand if a communication fails observing the leds status.

@par Directory contents

  - SDK_BasicPktMonitoring_A.c	 			Transmitter code
  
  - SDK_BasicPktMonitoring_B.c		 		Receiver code
  
  - SDK_BasicPktCommon.c and .h  		  	Common functions


@par Hardware and Software environment


  - This example runs on STM32L1xx Ultra Low Power Medium-Density Devices.

  - This example has been tested with STMicroelectronics SDK-EVAL evaluation
    board and can be easily tailored to any other supported device and
    development board.

  - SPIRIT Set-up
    - Connect SPIRIT to the SPI connectors in the SDK-EVAL board.

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain and import the .c files in your workspace
 - Rebuild all files and load your image into target memory
 - Run the example

@note
- Ultra Low Power Medium-density devices are STM32L151xx and STM32L152xx
  microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.

 * <h3><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h3>
 */

