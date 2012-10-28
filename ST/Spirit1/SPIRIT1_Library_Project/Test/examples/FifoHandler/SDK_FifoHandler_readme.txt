/**
  @page Fifo_Handler_example Fifo Handler example readme

  @verbatim
  ******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
  * @file    examples/FifoHandler/SDK_FifoHandler_readme.txt
  * @author  High End Analog & RF BU - AMS / ART Team IMS-Systems Lab
  * @version V2.0.2
  * @date    Febrary 7, 2012
  * @brief   BASIC packets with payload longer than 96 bytes transmission/reception example.
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

This example explains how to configure one node as a transmitter and the other as a receiver
in order to do a simple basic packet transmission.
There are two devices: the device A is configured as a transmitter and the device B as a
receiver.
Every time a communication is done the packet length is incremented. In this way, the devices
have to transmit and receive packets that can have a payload longer than 96 bytes (the maximum
FIFO size). In this case, in both the transmitter and the receiver, the FIFO must be loaded and
read during the communication in order to have loss of data.
On the transmitter side, the MCU should monitor the state of the Tx FIFO, setting the almost
empty threshold and managing the AE FIFO IRQ.
When this IRQ is raised, the MCU fills the FIFO and waits for the next IRQ (or for the end of Tx)
when the data have been transmitted.
On receiver, the MCU manages the FIFO using the almost empty threshold and the associated IRQ.
When the IRQ is raised, the MCU reads the FIFO and waits for the next IRQ (or for the end of Rx)
when data have been received.
The user can change the threshold values initialized in the code when the cThreholdTxFifoAE and
cThreholdRxFifoAF are declared.

Every time A performs a transmission it toggles the LED1 and LED2 every time the almost empty
threshold IRQ has been raised.
B communicates his state to the user by toggling its leds:
- LED3 to say that a reception has been completed
- LED2 to say that the almost full threshold IRQ has been raised
- LED1 to say that the RX timeout has expired
- LED5 to say that the received sequence is the expected one (integer numbers 0 to the actual payload size.
This parameter increases at every communication cycle).
Every action of this kind is managed through a managed IRQ.
So, if all works correctly the user should see:
- LEDs 3 and 5 of B toggling simultaneously if A is transmitting (eventually LED2 toggling if
the reception is done in more than one step).
- LEDs 1 toggling if A doesn't transmits for more than Rx timeout
(i.e. if it's off).

The COLERIDGE preprocessor variable can be used to change the payload to send. If it is defined the a famous rhime
paragraph will be sent, else the payload will be fit with the first nPayloadLength integer numbers.

@par Directory contents

  - SDK_FifoHandler_A.c			 		 Transmitter code
  
  - SDK_FifoHandler_B.c      		 		 Receiver code


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

