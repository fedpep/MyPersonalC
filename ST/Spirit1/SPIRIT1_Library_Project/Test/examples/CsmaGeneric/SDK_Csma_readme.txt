/**
  @page CSMA_generic_example CSMA generic example readme

  @verbatim
  ******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
  * @file    examples/CsmaGeneric/SDK_Csma_readme.txt
  * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
  * @version V2.0.2
  * @date    Febrary 7, 2012
  * @brief   STack packets transmission using the CSMA protocol example.
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

This example explains how to configure the CSMA protocol in order to obtain a collision
free transmission.
In this example there are three programs, two of them (SDK_Csma_A and SDK_Csma_B) have the
same transmission logic with CSMA turned on: they transmit a packet and wait some fractions
of sec before beginning a new transmission.
The last one (SDK_Csma_TxCW) is a simple CW transmitter without CSMA, i.e. a node that
transmits a bit stream continously without any control.
This node, which radio is configured in the same way as A and B, has the role to create a
career in order to create a collision with A or B packets if no multiple access policies
are used.
Now let's consider the case of A and TxCW switched on and B off.
Since A toggles the LED1 to notify a transmission done, the user will see this LED off all
the time that TxCW is on.
If TxCW is turned off, A transmits and its LED1 toggles.
If A and B are turned on and TxCW is off the user will see the LED 1 of the devices toggle
alternately notifying the transmissions have been done correctly because each device
has waited for the career free before beginning transmission.
In the program a threshold of RSSI of 95 is configured in order to ensure that the channel
is considered not free (CS flag equals to 1) if the actual measure of RSSI is over it.
In the program CSMA is configured in persistent mode, but the user can easily change this
mode by editing the CSMA configuration parameters defined at the beginning of the c files.
If the user configures the CSMA in not persistent mode, when the max number of backoff is
reached, the device will be forced to the READY state. An external interrupt to the push
button is used to send the Tx command to make the device exits from the READY state making
possible another transmission cycle.

@par Directory contents

  - SDK_Csma_A.c			  A CSMA transmitter of Basic packets code
  
  - SDK_Csma_B.c      		 	  B CSMA transmitter of Basic packets code
  
  - SDK_Csma_TxCW.c			  Continous Wave transmitter code
 

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

