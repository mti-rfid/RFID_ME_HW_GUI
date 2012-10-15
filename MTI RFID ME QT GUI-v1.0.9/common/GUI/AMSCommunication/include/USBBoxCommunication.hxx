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

/*
 *      PROJECT:   AMSCommunication
 *      $Revision: 1.1 $
 *      LANGUAGE: QT C++
 */

/*! \file
 *
 *  \author C. Eisendle
 *
 *  \brief  Communication class for BU comms USB Box
 *
 *  This controls the communication to the USB box used in the
 *  COMMS BU. The USB Box is based on a Cypress AN2131 (8051 compatible)
 *  controller. Actual USB access is handled via usbcon.dll.
 */

#ifndef USBBOXCOMMUNICATION_H
#define USBBOXCOMMUNICATION_H

#include <windows.h>
#include <stdio.h>
#include "AMSCommunication.hxx"
#include "usb.h"

#define BLOCK_WRITE_MAX_BYTES	0x3A	/*<! maximal number of bytes transferred with one block write command */

typedef enum {
  ERR_NO_ERR, /**< No error occurred. */
  ERR_CMD_FAILED, /**< The uC encountered an error. */
  ERR_CANNOT_CONNECT, /**< The demoboard is probably not plugged in. */
  ERR_NOT_CONNECTED, /**< Transfer function was called on a closed connection */
  ERR_TRANSMIT, /**< An error occurred during the transmission of a command. */
  ERR_RECEIVE, /**< An error occurred during reception. */
  ERR_ILLEGAL_ARGUMENTS, /**< The arguments are out of bounds. */
  ERR_GENERIC /**< An unspecified error occurred. */
} UsbError;

struct usbBoxConfigStructure
{
	unsigned char deviceI2CAddress;
};

/** Interface definition **/
typedef enum { I2C, SPI, SWI , PPTRIM , I2C_HardClock , Standard_SPI , SSPI } Interface;

class USBBoxCommunication : public AMSCommunication
{
	Q_OBJECT

public:
	USBBoxCommunication(unsigned char devAddr);
	~USBBoxCommunication();

	void setDevAddr(unsigned char registerAddress);
	AMSCommunication::Error hwConnect();
	void hwDisconnect();
	bool isConnected();
	AMSCommunication::Error hwReadRegister(unsigned char reg, unsigned char *val);
	AMSCommunication::Error hwWriteRegister(unsigned char reg, unsigned char val);
	AMSCommunication::Error hwSendCommand(QString command, QString * answer);
	UsbError sendReceivePacket(QByteArray *in, const QByteArray *out);
	UsbError sendReceiveI2CPacket(char startRegister, QByteArray * in, const QByteArray * out);
	UsbError setVsyncFrequency ( DWORD frequencyInHertz );

	typedef UsbError (*AMSusbConnect)(void);
	typedef UsbError (*AMSusbDisconnect)(void);
	typedef UsbError (*AMSusbReadByte) (unsigned char registerAddress, unsigned char* registerValue);
	typedef UsbError (*AMSusbWriteByte) (unsigned char registerAddress, unsigned char registerValue);
	typedef UsbError (*AMSusbSetI2CDevAddr) (unsigned char registerAddress);
	typedef UsbError (*AMSsetPortAsInput) (int ioPort, unsigned char mask);
	typedef UsbError (*AMSsetPortAsOutput) (int ioPort, unsigned char mask);
	typedef UsbError (*AMSwritePort) (unsigned char data, int ioPort, unsigned char mask); 
	typedef UsbError (*AMSreadPort) (unsigned char *data, int ioPort, unsigned char mask);
	typedef bool     (*AMSusbIsConnected) ();
	typedef UsbError (*AMSusbFirmwareID) (unsigned char * str);
	typedef UsbError (*AMSusbDebugMsg) (unsigned char* str);
	typedef UsbError (*AMSsetInterface) (Interface iface);
	typedef UsbError (*AMSwriteByte) (unsigned char register_address,unsigned char value);
	typedef UsbError (*AMSblkWrite) (unsigned char start_addr,unsigned char* values,unsigned char num_bytes);
	typedef UsbError (*AMSblkRead) (unsigned char start_addr,unsigned char* values,unsigned char num_bytes);
	typedef UsbError (*AMSreadADC) (unsigned char channel,unsigned* value);
	typedef UsbError (*AMSgetPortDirection) (Port io_port,unsigned char* direction);
	typedef UsbError (*AMSsendPacket) (char *in,int in_length, const char *out, int out_length);

	AMSsetPortAsInput setPortAsInput;
	AMSsetPortAsOutput setPortAsOutput;
	AMSwritePort writePort;
	AMSreadPort readPort;
    AMSusbReadByte readByteFunction;
    AMSusbWriteByte writeByteFunction;
	AMSusbFirmwareID readFirmwareId;
	AMSusbIsConnected         usbIsConnected;
	AMSusbFirmwareID          usbFirmwareID;
	AMSusbDebugMsg            usbDebugMsg;
	AMSsetInterface           setInterface;
	AMSwriteByte              writeByte;
	AMSblkWrite               blkWrite;
	AMSblkRead                blkRead;
	AMSreadADC                readADC;
	AMSgetPortDirection       getPortDirection;
	AMSsendPacket             sendPacketDLL;
	
protected:
	void setConnectionProperties(void *);
	void getConnectionProperties(void *);

private:
	UsbError setI2CDeviceAddress(unsigned char registerAddress);
	HINSTANCE hinstLib;

    AMSusbSetI2CDevAddr setI2CDeviceAddressFunction;
	unsigned char devAddr;
};

#endif // USBBOXCOMMUNICATION_H
