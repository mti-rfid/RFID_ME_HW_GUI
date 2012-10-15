/*
 *****************************************************************************
 * Copyright @ 2009 by austriamicrosystems AG                                *
 * All rights are reserved.                                                  *
 *                                                                           *
 * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
 * THE SOFTWARE.                                                             *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       * 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
 *****************************************************************************
 */

/**
 * @file usbcon.h
 * @author Christoph Wurzinger
 *
 * Header file for usbcon.dll
 */

#ifndef USBCON_H__
#define USBCON_H__

#include "usb.h"

/* make it c++ compatible... */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef DLL_BUILD__
#define __EXPORT_TYPE __export
#else
#define __EXPORT_TYPE
#endif // DLL_BUILD__

/** Interface definition */
typedef enum { I2C, SPI } Interface;
typedef unsigned char BYTE;
//typedef enum { TRUE, FALSE } bool;
/** Error-codes used as return values for the usb-interfacing. */
typedef enum {
  ERR_NO_ERR, /**< No error occured. */
  ERR_CMD_FAILED, /**< The uC encoutered an error. */
  ERR_CANNOT_CONNECT, /**< The demoboard is probably not plugged in. */
  ERR_NOT_CONNECTED, /**< Transfer function was called on a closed connection */
  ERR_TRANSMIT, /**< An error occured during the transmission of a command. */
  ERR_RECEIVE, /**< An error occured during receiption. */
  ERR_ILLEGAL_ARGUMENTS, /**< The arguments are out of bounds. */
  ERR_GENERIC /**< An unspecified error occured. */
} UsbError;


UsbError __EXPORT_TYPE AMSusbConnect();
UsbError __EXPORT_TYPE AMSusbDisconnect();
//bool __EXPORT_TYPE AMSusbIsConnected();

/**
 * note: the array str >= 64 !!!
 */
UsbError __EXPORT_TYPE AMSusbFirmwareID(BYTE* str);

/**
 * note: the array str >= 64 !!!
 */
UsbError __EXPORT_TYPE AMSusbDebugMsg(BYTE* str);

/**
 * Set I2C device address for subsequent read/write access
 *
 * @return UsbError
 * @param address 7bit device address
 */
UsbError __EXPORT_TYPE AMSsetI2CDevAddr(BYTE address);


/**
 * Select interface for subsequent read/write access
 *
 * @return UsbError
 * @param iface interface type (I2C/SPI)
 */
UsbError __EXPORT_TYPE AMSsetInterface(Interface iface);


/**
 * Write a single byte over the previosly selected inteface (I2C by default)
 *
 * @return UsbError
 * @param register_address 8bit register address
 * @param value 8bit register value
 */
UsbError __EXPORT_TYPE AMSwriteByte(BYTE register_address,BYTE value);


/**
 * Read a single byte over the previosly selected inteface (I2C by default)
 *
 * @return UsbError
 * @param register_address 8bit register address
 * @param value pointer to the resulting byte
 */
UsbError __EXPORT_TYPE AMSreadByte(BYTE register_address,BYTE* value);


/**
 * Write a block of bytes over the previosly selected inteface (I2C by default)
 *
 * @return UsbError
 * @param register_address 8bit start register address
 * @param value array of 8bit register values
 * @param num_bytes how many bytes to write
 */
UsbError __EXPORT_TYPE AMSblkWrite(BYTE start_addr,BYTE* values,BYTE num_bytes);


/**
 * Read a block of bytes over the previosly selected inteface (I2C by default)
 *
 * @return UsbError
 * @param register_address 8bit start register address
 * @param value array of 8bit register values
 * @param num_bytes how many bytes to read
 */
UsbError __EXPORT_TYPE AMSblkRead(BYTE start_addr,BYTE* values,BYTE num_bytes);


UsbError __EXPORT_TYPE AMSreadADC(BYTE channel,unsigned* value);


/**
 * \brief Reads one byte from a specified io-port.
 *
 * Note: The current state of the pins is returned (see EZ-USB Technical
 * Reference Manual section 4.2, PORTCFG = 0 for details).
 * \param data pointer to a BYTE that is filled with the read data.
 * \param io_port which IO-port to use (PORTA, PORTB or PORTC).
 * \param mask which pin to read.
 * \return UsbError code.
 */
UsbError __EXPORT_TYPE AMSreadPort(BYTE* data,Port io_port ,BYTE mask);

/**
 * \brief Writes one byte to a specified io-port.
 *
 * \param data pointer to a BYTE that is written.
 * \param io_port which IO-port to use (PORTA, PORTB or PORTC).
 * \param mask *data is ANDed with mask before the write access.
 * \return UsbError code.
 */
UsbError __EXPORT_TYPE AMSwritePort(BYTE data,Port io_port,BYTE mask);

/**
 * \brief Set a specific pin as input
 *
 * \param io_port which IO-port to influence
 * \param mask which IO-pin to influence
 * \return UsbError code.
 */
UsbError __EXPORT_TYPE AMSsetPortAsInput(Port io_port,BYTE mask);

/**
 * \brief Set a specific pin as output
 *
 * \param io_port which IO-port to influence
 * \param mask which IO-pin to influence
 * \return UsbError code.
 */
UsbError __EXPORT_TYPE AMSsetPortAsOutput(Port io_port,BYTE mask);

/**
 * \brief Get the IO-configuration of a specific IO-port
 *
 * \param io_port which IO-port to query
 * \param direction the result. pins configured as output are high bits.
 * \return UsbError code.
 */
UsbError __EXPORT_TYPE AMSgetPortDirection(Port io_port,BYTE* direction);

/**
 * \brief Permanently write bits to the OTP (zapp)
 *
 * \param values the bits to be written in _little endian_ order.
 * \param num_bits how many bits to write.
 * \return UsbError code.
 */
UsbError __EXPORT_TYPE AMSotpPermWrite(BYTE* values,unsigned short int num_bits);

/**
 * \brief Write bits to the OTP for testing purposes (_not_ zapp)
 *
 * \param values the bits to be written in _little endian_ order.
 * \param num_bits how many bits to write.
 * \return UsbError code.
 */
UsbError __EXPORT_TYPE AMSotpTestWrite(BYTE* values,unsigned short int num_bits);

/**
 * \brief Read bits from the OTP (the digital way)
 *
 * \param values the bits to be read in _little endian_ order.
 * \param num_bits how many bits to read.
 * \param perform_load whether to perform the loading sequence (see Easy2zapp App. Note)
 * \return UsbError code.
 */
//UsbError __EXPORT_TYPE AMSotpDigitalRead(BYTE* values,unsigned short int num_bits,bool perform_load = true);

/**
 * \brief Read bits from the OTP (the analog way)
 *
 * \param values the ADC results in _little endian_ order, Vddmes at values[num_bits].
 *               note: the size of the array has to be num_bits + 1 (for Vddmes)
 * \param num_bits how many bits to read.
 * \return UsbError code.
 */
UsbError __EXPORT_TYPE AMSotpAnalogRead(unsigned short int* values,unsigned short int num_bits);

/**
 * \brief Read bits from the OTP (the analog way)
 *
 * \param values the ADC results converted to Volts, Vddmes at values[num_bits].
 *               note: the size of the array has to be num_bits + 1 (for Vddmes)
 * \param num_bits how many bits to read.
 * \return UsbError code.
 */
UsbError __EXPORT_TYPE AMSotpAnalogReadVolts(double* values,unsigned short int num_bits);

#ifdef __cplusplus	//c++ compiler command
}
#endif

#endif // USBCON_H__
