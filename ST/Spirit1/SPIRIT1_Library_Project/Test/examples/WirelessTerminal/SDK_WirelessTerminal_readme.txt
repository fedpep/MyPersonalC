/**
  @page COM_wireless_terminal_example COM wireless terminal example readme

  @verbatim
  ******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
  * @file    examples/WirelessTerminal/SDK_WirelessTerminal_readme.txt
  * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
  * @version V2.0.2
  * @date    Febrary 7, 2012
  * @brief   String transmission/reception example.
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

This example is made up of two programs, a transmitter and a receiver.
The transmitter (A) has the only role of receive some bytes from the Virtual Com
channel and send them in form of Spirit Basic packets. It bufferizes all
the received bytes searching for the windows enter char ('\r') or that
the number of received bytes is equal to 90 and then it builds a packet
to send.
The receiver (B), on its side, receives these packets and send them through
Virtual Com allowing a COM terminal to display the received string.
So an user who plugs the two devices can see the strings sent to the transmitter
COM (using a COM terminal) in the COM terminal connected to the receiver.
The transmitter toggles the LED1 every time a transmission is done, while
the receiver toggles its leds in the following way:
- LED3 to say that a reception has been done
- LED4 to say that a sync word has been detected
- LED2 to say that a packet has been discarded
- LED1 to say that the RX timeout (set to 1 sec) has expired
- LED5 to say that the received sequence is the expected one (integer numbers 1 to 20)


@par Directory contents

  - SDK_WirelessTerminal_A.c			 Transmitter code
  
  - SDK_WirelessTerminal_B.c      		 Receiver code


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

