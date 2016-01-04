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


#include "libmaple_types.h"
#include "HardwareSPI.h"
#include "io.h"
#include "delay.h"
#include "wirish.h"


/* Commands */
#define	POWER_UP				0x02
#define NOP						0x00
#define PART_INFO				0x01
#define FUNC_INFO				0x10
#define SET_PROPERTY			0x11
#define GET_PROPERTY			0x12
#define GPIO_PIN_CFG			0x13
#define FIFO_INFO				0x15
#define GET_INT_STATUS			0x20
#define REQUEST_DEVICE_STATE	0x33
#define CHANGE_STATE			0x34
#define READ_CMD_BUFF			0x44
#define FRR_A_READ				0x50
#define FRR_B_READ				0x51
#define FRR_C_READ				0x53
#define FRR_D_READ				0x57
#define IRCAL					0x17
#define IRCAL_MANUAL 			0x1a
#define	START_TX				0x31
#define	WRITE_TX_FIFO 			0x66
#define PACKET_INFO				0x16
#define GET_MODEM_STATUS		0x22
#define START_RX				0x32
#define RX_HOP					0x36
#define READ_RX_FIFO 			0x77
#define GET_ADC_READING			0x14
#define GET_PH_STATUS			0x21
#define GET_CHIP_STATUS 		0x23


/* Properties */

class Si4463 {	
	public:
		/* Variables */
		typedef	struct {
			HardwareSPI* 	spi;
			uint8			sdn;			
			uint8			nsel;
			uint8			nirq;
			uint8			gpio0;
			uint8			gpio1;
			uint8			gpio2;
			uint8			gpio3;
		} Port;	
		uint16 timeout;		
		
		/* Function declarations */ 
		Si4463(HardwareSPI*, uint8, uint8);
		void powerUp();
		uint8 nop();
		uint8 partInfo();
		uint8 funcInfo();
		void setProperty(uint8 group, uint8 num_props, uint8 start_prop, byte* buffer);
		uint8 getProperty(uint8 group, uint8 num_props, uint8 start_prop);
		uint8 readCmdBuffer(byte*, uint8, uint8);		
		uint8 readCmdBuffer(byte*, uint8);
		void startTx(uint8 channel, uint8 condition, uint8 tx_len);
		void startTx();
		void writeTxFifo(byte*, uint8);
		void readRxFifo(byte*, uint8);

		
	private:
		Port _port;
		uint8 _cmdBufferLength;
		void execCmd(uint8, byte*, uint8);
		
};


#endif