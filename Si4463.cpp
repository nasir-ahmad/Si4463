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
 *  @brief Si4463 EZPro RF module library for LeafLabs Maple
 */

#include "Si4463.h"

Si4463::Si4463(HardwareSPI *spi_dev, uint8 nsel_pin, uint8 sdn_pin){
	
	timeout = 5000;
	
	_port.nsel = nsel_pin;
	_port.sdn = sdn_pin;
	_port.spi = spi_dev;
	
	pinMode(_port.nsel, OUTPUT);
	pinMode(_port.sdn, OUTPUT);
	
	_port.spi->begin(SPI_9MHZ, MSBFIRST, 0);	
	digitalWrite(_port.nsel, HIGH);
	digitalWrite(_port.sdn, HIGH);	
	
}

void Si4463::powerUp(){
	uint8 data[] = { 0x02, 0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80 };
	execCmd(POWER_UP, data, sizeof(data));
}

uint8 Si4463::nop(){
	uint8 cts;	
	digitalWrite(_port.nsel, LOW);	
	_port.spi->write(NOP);
	cts = _port.spi->transfer(0x00);
	digitalWrite(_port.nsel, HIGH);	
	
	return cts;
}

uint8 Si4463::partInfo(){
	execCmd(PART_INFO, 0, 0);
	
	/* Return number of reply bytes */
	return 8;
}

uint8 Si4463::funcInfo(){
	execCmd(FUNC_INFO, 0, 0);
	return 6;
}

void Si4463::setProperty(uint8 group, uint8 num_props, uint8 start_prop, byte* buffer){
	uint8 length = num_props + 3;
	uint8 data[length];
	
	data[0] = group;
	data[1] = num_props;
	data[2] = start_prop;
	
	for(uint8 i = 0; i < num_props; i++){
		data[i+3] = buffer[i];		
	}
	
	execCmd(SET_PROPERTY, buffer, length);
	
	/*
	digitalWrite(_port.nsel, LOW);	
	
	_port.spi->write(SET_PROPERTY);	
	_port.spi->write(group);
	_port.spi->write(num_props);
	_port.spi->write(start_prop);
	
	while(num_props--){
		_port.spi->write(*buffer++);
	}
	
	digitalWrite(_port.nsel, HIGH);	
	*/
}

uint8 Si4463::getProperty(uint8 group, uint8 num_props, uint8 start_prop){
	uint8 data[] = { group, num_props, start_prop };
	execCmd(GET_PROPERTY, data, sizeof(data));
	
	return num_props;
}

void Si4463::startTx(uint8 channel, uint8 condition, uint8 tx_len){
	uint8 data[] = { channel, condition, tx_len };
	execCmd(START_TX, data, 3);
}


void Si4463::startTx(){
	execCmd(START_TX, 0, 0);	
}


void Si4463::writeTxFifo(byte *buffer, uint8 length){
	execCmd(WRITE_TX_FIFO, buffer, length);	
}

void Si4463::readRxFifo(byte *buffer, uint8 length){
	execCmd(READ_RX_FIFO, 0, 0);
	
	//This command does not cause CTS to go low, and can be sent while CTS is low. 
	readCmdBuffer(buffer, length, true);
}


uint8 Si4463::readCmdBuffer(byte *buffer, uint8 length, uint8 ignoreCts){
	volatile uint8 cts = 0x00;
	
	//while(!cts){
		digitalWrite(_port.nsel, LOW);	
		_port.spi->write(READ_CMD_BUFF);	//Send read_command_buffer command
		cts = ignoreCts || _port.spi->transfer(0x00);	//Send dummy byte to read CTS
				
		if (cts && buffer){
			while(length--){
				*buffer++ = _port.spi->transfer(0x00);	//Sending dummy byte to generate clock
			}
		}
		digitalWrite(_port.nsel, HIGH);
		
		//if(!timeout)
		//	return cts;		
	//}
	
	return cts;
}

uint8 Si4463::readCmdBuffer(byte *buffer, uint8 length){
	return readCmdBuffer(buffer, length, 0);
}

/*
void Si4463::packetInfo(uint8 fieldNumber, uint16 len, uint16 lenDiff){
		
}
*/


void Si4463::execCmd(uint8 cmd, byte* buffer, uint8 length){
	uint8 bbyte;
	
	digitalWrite(_port.nsel, LOW);	
	_port.spi->write(cmd);
	
	while(length--){
		bbyte = *buffer++;
		if (bbyte == NULL) break;
		_port.spi->write(bbyte);
	}
	
	digitalWrite(_port.nsel, HIGH);	 
}