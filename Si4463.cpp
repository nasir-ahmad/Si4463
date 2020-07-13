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

static const uint8_t configs[] PROGMEM = RADIO_CONFIGURATION_DATA_ARRAY;

Si4463::Si4463(SPIClass *spiPort, uint8 nselPin, uint8 sdnPin)
{
    this->_port.spi = spiPort;
    this->_port.nsel = nselPin;
    this->_port.sdn = sdnPin;
}

void Si4463::begin()
{
    //Configure spi interface
    SPI.begin();                         //Initialize the port.
    SPI.setBitOrder(MSBFIRST);           // Set the SPI bit order
    SPI.setDataMode(SPI_MODE0);          //Set the  SPI data mode 0
    SPI.setClockDivider(SPI_CLOCK_DIV8); // Slow speed (72 / 16 = 4.5 MHz SPI_1 speed)

    //Setup pins
    pinMode(_port.nsel, OUTPUT);
    pinMode(_port.sdn, OUTPUT);

    //Initialize pins with disable states
    digitalWrite(_port.nsel, HIGH);
    digitalWrite(_port.sdn, HIGH);
}

/**
 * Execute raw command
 * data[0] = CMD
 * data[1..n] = arguments
 * 
 */
void Si4463::execCommand(uint8 *data, uint8 length)
{
    digitalWrite(_port.nsel, LOW);
    SPI.write(data, length);
    digitalWrite(_port.nsel, HIGH);
}

/**
 * Execute command 
 */
void Si4463::execCommand(uint8 cmd)
{
    digitalWrite(_port.nsel, LOW);
    SPI.write(cmd);
    digitalWrite(_port.nsel, HIGH);
}

/**
 * Execute command with parameters
 */
void Si4463::execCommand(uint8 cmd, uint8 *data, uint8 length)
{
    digitalWrite(_port.nsel, LOW);
    SPI.write(cmd);
    if ((data != NULL) && (length > 0))
        SPI.write(data, length);
    digitalWrite(_port.nsel, HIGH);
}

/**
 * Read response from cmd buffer of the executed command.
 * Wait for cts before reading.
 */
void Si4463::readCmdBuffer(uint8 *responseStream, uint8 length)
{
    while (true)
    {
        digitalWrite(_port.nsel, LOW);
        SPI.write(CMD_READ_CMD_BUFF);
        _cts = SPI.transfer(0x00);
        if (_cts == 0xFF)
        {
            while (length--)
                *responseStream++ = SPI.transfer(0x00);
            digitalWrite(_port.nsel, HIGH);
            break;
        }

        digitalWrite(_port.nsel, HIGH);
    }
}

/**
 * Wait for clear to send
 */
void Si4463::waitCts()
{
    while (true)
    {
        digitalWrite(_port.nsel, LOW);
        SPI.write(CMD_READ_CMD_BUFF);
        this->_cts = SPI.transfer(0x00);
        digitalWrite(_port.nsel, HIGH);

        if (this->_cts == 0xFF)
            break;
    }
}

/**
 * POR (Power On Reset) radio 
 * and wait for complete
 * 
 */
void Si4463::por()
{
    digitalWrite(_port.sdn, HIGH);
    delayMicroseconds(15);
    digitalWrite(_port.sdn, LOW);

    //Wait for max. delay of POR (<6ms)
    delay(10);
}

/**
 * Power up radio
 */
void Si4463::powerUp()
{
    //Trigger POR (power on reset) and wait complete
    this->por();
    delay(4000);
    uint8 buffer[20];

    //Apply radio configs
    for (uint16 i = 0; i < sizeof(configs); i++)
    {
        memcpy_P(buffer, &configs[i], sizeof(buffer));
        i += buffer[0];
        this->execCommand(&buffer[1], buffer[0]);
        this->waitCts();

        /*        
        for (int j = 0; j <= buffer[0]; j++)
        {
            Serial.print(buffer[j], 16);
            Serial.print('-');
        }
        Serial.println("");
        delay(500);
        */
    }
}

/**
 * Get device part info
 */
void Si4463::partInfo(uint8 *data, uint8 length)
{
    //ROM ID (3 = revB1B, 6 = revC2A)
    this->execCommand(CMD_PART_INFO);
    this->readCmdBuffer(data, length);
}

/**
 * Reqeust device state and return response 
 */
void Si4463::requestDeviceState(uint8 *data, uint8 length)
{
    this->execCommand(CMD_REQUEST_DEVICE_STATE);
    this->readCmdBuffer(data, length);
}

/**
 *  Change current device state to target state
 */
void Si4463::changeState(uint8 state)
{
    uint8 data[] = {state};
    this->execCommand(CMD_CHANGE_STATE, data, 1);
    this->waitCts();
}

/**
 * Get packet handler status
 * and clear interrupt flags
 */
void Si4463::getPacketHandlerStatus(uint8 phClrPend, uint8 *response, uint8 length)
{
    uint8 data[] = {phClrPend};
    this->execCommand(CMD_GET_PH_STATUS, data, 1);
    this->readCmdBuffer(response, length);
}

/**
 * Get fifo info
 */
void Si4463::fifoInfo(uint8 reset, uint8 *response, uint8 length)
{
    uint8 data[] = {reset};
    this->execCommand(CMD_FIFO_INFO, data, 1);
    this->readCmdBuffer(response, length);
}

/**
 * Set property
 */
void Si4463::setProperty(uint8 group, uint8 length, uint8 start, uint8 *data)
{
    digitalWrite(_port.nsel, LOW);
    SPI.write(CMD_SET_PROPERTY);
    SPI.write(group);
    SPI.write(length);
    SPI.write(start);
    SPI.write(data, length);
    digitalWrite(_port.nsel, HIGH);

    this->waitCts();
}

/**
 * Get property
 */
void Si4463::getProperty(uint8 group, uint8 length, uint8 start, uint8 *data)
{
    digitalWrite(_port.nsel, LOW);
    SPI.write(CMD_SET_PROPERTY);
    SPI.write(group);
    SPI.write(length);
    SPI.write(start);
    digitalWrite(_port.nsel, HIGH);

    this->readCmdBuffer(data, length);
}

/**
 * Start tx
 */
void Si4463::startTx(uint8 channel, uint8 txCompleteState, bool retransmit, uint8 start, uint16 txLen)
{
    uint8 data[] = {
        channel,
        (txCompleteState << 4) | start,
        txLen >> 8,
        txLen};

    this->execCommand(CMD_START_TX, data, sizeof(data));
    this->waitCts();
}

/**
 * Write data to TX FIFO
 */
void Si4463::writeTxFifo(uint8 *buffer, uint8 length)
{
    this->execCommand(CMD_WRITE_TX_FIFO, buffer, length);
}

/**
 * Start RX
 */
void Si4463::startRx(uint8 channel, uint8 start, uint16 rxLen, uint8 state1, uint8 state2, uint8 state3)
{
    uint8 data[] = {
        channel,
        start,
        rxLen >> 8,
        rxLen,
        state1,
        state2,
        state3,
    };

    this->execCommand(CMD_START_RX, data, sizeof(data));
    this->waitCts();
}

/**
 * Read data from RX FIFO
 */
void Si4463::readRxFifo(uint8 *buffer, uint8 length)
{
    digitalWrite(_port.nsel, LOW);
    SPI.write(CMD_READ_RX_FIFO);
    while (length--)
    {
        *buffer++ = SPI.transfer(0x00);
    }
    digitalWrite(_port.nsel, HIGH);
}

void Si4463::getIntStatus(uint8 phClrPend, uint8 modemClrPend, uint8 chipClrPend, uint8 *data, uint8 length)
{
    uint8 params[] = {phClrPend, modemClrPend, chipClrPend};
    this->execCommand(CMD_GET_INT_STATUS, params, sizeof(params));
    this->readCmdBuffer(data, length);
}