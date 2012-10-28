/**
  @page Basic_packet_test_example Basic packet test example readme

  @verbatim
  ******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
  * @file    examples/BasicPktTest/SDK_BasicPktTest_readme.txt
  * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
  * @version V2.0.2
  * @date    Febrary 7, 2012
  * @brief   BASIC packet test.
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

This application performs a Basic packet test using a transmitter (device A)
and a receiver (device B).
The application is based on a master/slave architecture. The master (device A)
receives the following commands by the user from VCOM:

 - "start": to begin the test
 
 - "stop":  to stop the test

At the start command the master sends first a start packet to the slave,
and then it begins a transmission of packets.
After the master receives the start packet, it begins counting the received
packets and displays the actual RSSI value.
At the stop command on the master, it sends a stop packet to the slave
merging in it also the number of transmitted packets making possible to the
slave the computation of the packet error rate.


@par Directory contents

  - SDK_BasicPktTest_A.c		 Transmitter code
  
  - SDK_BasicPktTest_B.c      		 Receiver code
  
  - SDK_BasicPktTest_Common.h		 Common parameters definition

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

