 /*
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

#ifndef MCP4725_H
#define MCP4725_H

#include "mbed.h"

/** MCP4725 class.
 *  Used for controlling an MCP4725 DAC connected via I2C.
 *
 * Example:
 * @code
 * #include "mbed.h"
 * #include "MCP4725.h"
 *
 * //Create an MCP4725 object at the default address (ADDRESS_0)
 * MCP4725 dac(p28, p27);
 *
 * int main()
 * {
 *     //Try to open the MCP4725
 *     if (dac.open()) {
 *         printf("Device detected!\n");
 *
 *         //Wake up the DAC
 *         //NOTE: This might wake up other I2C devices as well!
 *         dac.wakeup();
 *
 *         while (1) {
 *             //Generate a sine wave on the DAC
 *             for (float i = 0.0; i < 360.0; i += 0.1)
 *                 dac = 0.5 * (sinf(i * 3.14159265 / 180.0) + 1);
 *         }
 *     } else {
 *         error("Device not detected!\n");
 *     }
 * }
 * @endcode
 */
class MCP4725
{
public:
    /** Represents the different I2C address possibilities for the MCP4725
     */
    enum Address {
        ADDRESS_0 = (0x60 << 1),    /**< A[2:0] bits = 000 */
        ADDRESS_1 = (0x61 << 1),    /**< A[2:0] bits = 001 */
        ADDRESS_2 = (0x62 << 1),    /**< A[2:0] bits = 010 */
        ADDRESS_3 = (0x63 << 1),    /**< A[2:0] bits = 011 */
        ADDRESS_4 = (0x64 << 1),    /**< A[2:0] bits = 100 */
        ADDRESS_5 = (0x65 << 1),    /**< A[2:0] bits = 101 */
        ADDRESS_6 = (0x67 << 1),    /**< A[2:0] bits = 110 */
        ADDRESS_7 = (0x68 << 1)     /**< A[2:0] bits = 111 */
    };

    /** Represents the power mode of the MCP4725
     */
    enum PowerMode {
        POWER_NORMAL,           /**< Chip is enabled, and the output is active */
        POWER_SHUTDOWN_1K,      /**< Chip is shutdown, and the output is grounded with a 1kΩ resistor */
        POWER_SHUTDOWN_100K,    /**< Chip is shutdown, and the output is grounded with a 100kΩ resistor */
        POWER_SHUTDOWN_500K     /**< Chip is shutdown, and the output is grounded with a 500kΩ resistor */
    };

    /** Create an MCP4725 object connected to the specified I2C pins with the specified I2C slave address
     *
     * @param sda The I2C data pin.
     * @param scl The I2C clock pin.
     * @param addr The I2C slave address (defaults to ADDRESS_0).
     * @param hz The I2C bus frequency (defaults to 400kHz).
     */
    MCP4725(PinName sda, PinName scl, Address addr = ADDRESS_0, int hz = 400000);

    /** Probe for the MCP4725 and indicate if it's present on the bus
     *
     * @returns
     *   'true' if the device exists on the bus,
     *   'false' if the device doesn't exist on the bus.
     */
    bool open();

    /** Issue a General Call Reset command to reset all MCP4725 devices on the bus
     *
     * @warning This might reset other I2C devices as well
     */
    void reset();

    /** Issue a General Call Wake-up command to power-up all MCP4725 devices on the bus
     *
     * @warning This might wake up other I2C devices as well
     */
    void wakeup();

    /** Get the current power mode of the MCP4725
     *
     * @returns The current power mode as a PowerMode enum.
     */
    MCP4725::PowerMode powerMode();

    /** Set the power mode of the MCP4725
     *
     * @param mode The new power mode as a PowerMode enum.
     */
    void powerMode(PowerMode mode);

    /** Get the current output voltage of the MCP4725 as a percentage
     *
     * @returns The current output voltage as a percentage (0.0 to 1.0 * VDD).
     */
    float read();

    /** Set the output voltage of the MCP4725 from a percentage
     *
     * @param value The new output voltage as a percentage (0.0 to 1.0 * VDD).
     */
    void write(float value);

    /** Set the output voltage of the MCP4725 from a 16-bit range
     *
     * @param value The new output voltage as a 16-bit unsigned short (0x0000 to 0xFFFF).
     */
    void write_u12(unsigned short value);

    /** Get the current DAC settings in EEPROM
     *
     * @param mode Pointer to a PowerMode enum for the power mode in EEPROM.
     * @param value Pointer to an unsigned short for the 12-bit DAC value in EEPROM (0x0000 to 0x0FFF).
     */
    void readEeprom(PowerMode* mode, unsigned short* value);

    /** Set the DAC settings in EEPROM
     *
     * @param mode The new EEPROM power mode as a PowerMode enum.
     * @param value The new EEPROM DAC value as a 12-bit unsigned short (0x0000 to 0x0FFF).
     */
    void writeEeprom(PowerMode mode, unsigned short value);

#ifdef MBED_OPERATORS
    /** A shorthand for read()
     *
     * @returns The current output voltage as a percentage (0.0 to 1.0 * VDD).
     */
    operator float();

    /** A shorthand for write()
     *
     * @param value The new output voltage as a percentage (0.0 to 1.0 * VDD).
     */
    MCP4725& operator=(float value);
#endif

private:
    //Member variables
    I2C m_I2C;
    const int m_ADDR;
    MCP4725::PowerMode m_PowerMode;
    unsigned short m_DacValue;

    //Internal functions
    void readDac();
    void writeDac();
};

#endif
