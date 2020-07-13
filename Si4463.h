/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2014 Nasir Ahmad.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 *  @brief SI4463 RF module library for LeafLabs Maple
 */

#ifndef _SI4463_H_
#define _SI4463_H_

#include "Arduino.h"
#include "SPI.h"
#include "radio_config_Si4463.h"

/* Commands */
#define CMD_POWER_UP 0x02
#define CMD_NOP 0x00
#define CMD_PART_INFO 0x01
#define CMD_FUNC_INFO 0x10
#define CMD_SET_PROPERTY 0x11
#define CMD_GET_PROPERTY 0x12
#define CMD_GPIO_PIN_CFG 0x13
#define CMD_FIFO_INFO 0x15
#define CMD_GET_INT_STATUS 0x20
#define CMD_REQUEST_DEVICE_STATE 0x33
#define CMD_CHANGE_STATE 0x34
#define CMD_READ_CMD_BUFF 0x44
#define CMD_FRR_A_READ 0x50
#define CMD_FRR_B_READ 0x51
#define CMD_FRR_C_READ 0x53
#define CMD_FRR_D_READ 0x57
#define CMD_IRCAL 0x17
#define CMD_IRCAL_MANUAL 0x1a
#define CMD_START_TX 0x31
#define CMD_WRITE_TX_FIFO 0x66
#define CMD_PACKET_INFO 0x16
#define CMD_GET_MODEM_STATUS 0x22
#define CMD_START_RX 0x32
#define CMD_RX_HOP 0x36
#define CMD_READ_RX_FIFO 0x77
#define CMD_GET_ADC_READING 0x14
#define CMD_GET_PH_STATUS 0x21
#define CMD_GET_CHIP_STATUS 0x23

typedef struct si4463deviceState_t
{
	uint8 test;
} si4463deviceState;

class Si4463
{
public:
	/* Variables */
	typedef struct
	{
		SPIClass *spi;
		uint8 sdn;
		uint8 nsel;
		uint8 nirq;
		uint8 cts;
		uint8 gpio0;
		uint8 gpio1;
		uint8 gpio2;
		uint8 gpio3;
	} Port;

	/* Function declarations */
	Si4463(SPIClass *spiPort, uint8 nselPin, uint8 sdnPin);
	void begin();
	void powerUp();
	void por();
	void partInfo(uint8 *data, uint8 length);
	void requestDeviceState(uint8 *data, uint8 length);
	void getIntStatus(uint8 phClrPend, uint8 modemClrPend, uint8 chipClrPend, uint8 *data, uint8 length);
	void changeState(uint8 state);
	void getPacketHandlerStatus(uint8 phClrPend, uint8 *response, uint8 length);
	void fifoInfo(uint8 reset, uint8 *response, uint8 length);
	void setProperty(uint8 group, uint8 length, uint8 start, uint8 *data);
	void getProperty(uint8 group, uint8 length, uint8 start, uint8 *response);
	void startTx(uint8 channel, uint8 txCompleteState, bool retransmit, uint8 start, uint16 txLen);
	void writeTxFifo(uint8 *data, uint8 length);
	void startRx(uint8 channel, uint8 start, uint16 rxLen, uint8 state1, uint8 state2, uint8 state3);
	void readRxFifo(uint8 *data, uint8 length);
private:
	Port _port;
	volatile uint8 _cts;
	void waitCts();
	void execCommand(uint8 *data, uint8 length); //Execute raw command
	void execCommand(uint8 cmd);
	void execCommand(uint8 cmd, uint8 *data, uint8 length);
	void readCmdBuffer(uint8 *responseBuffer, uint8 length);
};

#endif