/* MCP4725 Driver Library
 * Copyright (c) 2014 Neil Thiessen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MCP4725.h"

MCP4725::MCP4725(PinName sda, PinName scl, Address addr, int hz) : m_I2C(sda, scl), m_ADDR((int)addr)
{
    //Set the I2C bus frequency
    m_I2C.frequency(hz);
}

bool MCP4725::open()
{
    //Probe for the MCP4725 using a Zero Length Transfer
    if (!m_I2C.write(m_ADDR, NULL, 0)) {
        //Return success
        return true;
    } else {
        //Return failure
        return false;
    }
}

void MCP4725::reset()
{
    //The General Call Reset command
    char data = 0x06;

    //Issue the command to the General Call address
    m_I2C.write(0x00, &data, 1);
}

void MCP4725::wakeup()
{
    //The General Call Wake-up command
    char data = 0x09;

    //Issue the command to the General Call address
    m_I2C.write(0x00, &data, 1);
}

MCP4725::PowerMode MCP4725::powerMode()
{
    //Read the current settings from the DAC
    readDac();

    //Return the current power mode
    return m_PowerMode;
}

void MCP4725::powerMode(PowerMode mode)
{
    //Update the power mode
    m_PowerMode = mode;

    //Update the DAC
    writeDac();
}

float MCP4725::read()
{
    //Read the current settings from the DAC
    readDac();

    //Return the current DAC value as a percentage
    return m_DacValue / 4095.0;
}

void MCP4725::write(float value)
{
    //Range limit value
    if (value < 0.0)
        value = 0.0;
    else if (value > 1.0)
        value = 1.0;

    //Update the DAC value
    m_DacValue = (unsigned short)(value * 4095);

    //Update the DAC
    writeDac();
}

void MCP4725::write_u12(unsigned short value)
{
    //Update the DAC value
    //m_DacValue = value >> 4;
    m_DacValue = value;
    //Update the DAC
    writeDac();
}

void MCP4725::readEeprom(PowerMode* mode, unsigned short* value)
{
    //Create a temporary buffer
    char buff[5];

    //Keep reading until the EEPROM is ready
    do {
        m_I2C.read(m_ADDR, buff, 5);
    } while ((buff[0] & 0x80) == 0);

    //Extract the EEPROM power mode, and 12-bit DAC value
    *mode = (PowerMode)((buff[3] >> 5) & 0x03);
    *value = ((buff[3] << 8) & 0xFFF) | buff[4];
}

void MCP4725::writeEeprom(PowerMode mode, unsigned short value)
{
    //Create a temporary buffer
    char buff[5];

    //Block until the EEPROM is ready
    do {
        m_I2C.read(m_ADDR, buff, 5);
    } while ((buff[0] & 0x80) == 0);

    //Load the command, power mode, and 12-bit DAC value
    buff[0] = 0x60 | ((int)mode << 1);
    buff[1] = value >> 4;
    buff[2] = value << 4;

    //Write the data
    m_I2C.write(m_ADDR, buff, 3);
}

#ifdef MBED_OPERATORS
MCP4725::operator float()
{
    //Return the current output voltage
    return read();
}

MCP4725& MCP4725::operator=(float value)
{
    //Set the output voltage
    write(value);
    return *this;
}
#endif

void MCP4725::readDac()
{
    //Create a temporary buffer
    char buff[5];

    //Read the current DAC settings
    m_I2C.read(m_ADDR, buff, 5);

    //Extract the current power mode, and 12-bit DAC value
    m_PowerMode = (PowerMode)((buff[0] >> 1) & 0x03);
    m_DacValue = (buff[1] << 4) | (buff[2] >> 4);
}

void MCP4725::writeDac()
{
    //Create a temporary buffer
    char buff[2];

    //Load the power mode, and 12-bit DAC value
    buff[0] = ((int)m_PowerMode << 4) | (m_DacValue >> 8);
    buff[1] = m_DacValue;

    //Write the data
    m_I2C.write(m_ADDR, buff, 2);
}
