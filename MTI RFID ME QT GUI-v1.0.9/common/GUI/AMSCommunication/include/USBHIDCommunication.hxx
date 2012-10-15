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
 *  \author S. Puri
 *
 *  \brief  Communication class for communication via USB HID
 *
 *  This controls the communication to the SiLabs C8051F34x.
 *  Actual USB access is handled via SLABHIDDevice.dll.
 *  Human Interface Device (HID) Tutorials (AN249).
 */

#ifndef USBHIDCOMMUNICATION_HXX
#define USBHIDCOMMUNICATION_HXX

#include <windows.h>
#include <stdio.h>
#include "AMSCommunication.hxx"
#include "HIDDevice.h"

typedef enum {
  ERR_USBHID_NO_ERR, /**< No error occurred. */
  ERR_USBHID_CMD_FAILED, /**< The uC encountered an error. */
  ERR_USBHID_CANNOT_CONNECT, /**< The demoboard is probably not plugged in. */
  ERR_USBHID_NOT_CONNECTED, /**< Transfer function was called on a closed connection */
  ERR_USBHID_TRANSMIT, /**< An error occurred during the transmission of a command. */
  ERR_USBHID_RECEIVE, /**< An error occurred during reception. */
  ERR_USBHID_ILLEGAL_ARGUMENTS, /**< The arguments are out of bounds. */
  ERR_USBHID_GENERIC /**< An unspecified error occurred. */
} UsbHidError;

struct usbhidConfigStructure
{
	int pid;
	int vid;
	unsigned char inReportID;
	unsigned char outReportID;
	unsigned char readRegisterCommand;
	unsigned char writeRegisterCommand;
};

class USBHIDCommunication : public AMSCommunication
{
	Q_OBJECT

public:
	USBHIDCommunication(unsigned char devAddr);
	~USBHIDCommunication();

	AMSCommunication::Error hwConnect();
	void hwDisconnect();
	bool isConnected();
	AMSCommunication::Error hwReadRegister(unsigned char reg, unsigned char *val);
	AMSCommunication::Error hwWriteRegister(unsigned char reg, unsigned char val);
	AMSCommunication::Error hwSendCommand(QString command, QString * answer);
	
protected:
	void setConnectionProperties(void *);
	void getConnectionProperties(void *);

protected:
	UsbHidError setI2CDeviceAddress(unsigned char registerAddress);
	typedef DWORD (*AMSgetNumHidDevices) (WORD vid, WORD pid);
	typedef BYTE (*AMSgetHidString) (DWORD deviceIndex, WORD vid, WORD pid, BYTE hidStringType, LPSTR deviceString, DWORD deviceStringLength);
	typedef VOID (*AMSgetHidGuid) (LPGUID hidGuid);
#if 0
	typedef UsbError (*AMSusbConnect)(void);
	typedef UsbError (*AMSusbDisconnect)(void);
	typedef UsbError (*AMSusbReadByte) (unsigned char registerAddress, unsigned char* registerValue);
	typedef UsbError (*AMSusbWriteByte) (unsigned char registerAddress, unsigned char registerValue);
	typedef UsbError (*AMSusbSetI2CDevAddr) (unsigned char registerAddress);
	typedef UsbError (*AMSsetPortAsOutput) (int ioPort, unsigned char mask);
	typedef UsbError (*AMSwritePort) (unsigned char data, int ioPort, unsigned char mask); 
#endif
	HINSTANCE hinstLib;

	AMSgetNumHidDevices getNumHidDevicesFunction;
	AMSgetHidString getHidStringFunction;
	AMSgetHidGuid getHidGuidFunction;
	//CHIDDevice CHIDDeviceClass;
	CHIDDevice *hidDevice;
#if 0
	AMSsetPortAsOutput setPortAsOutput;
	AMSwritePort writePort;
    AMSusbReadByte readByteFunction;
    AMSusbWriteByte writeByteFunction;
    AMSusbSetI2CDevAddr setI2CDeviceAddressFunction;
#endif
	unsigned char devAddr;
	int pid;
	int vid;
	unsigned char inReportID;
	unsigned char outReportID;
	unsigned char readRegisterCommand;
	unsigned char writeRegisterCommand;

	int inputReportBufferLength;
	int outputReportBufferLength;
	int featureReportBufferLength;
	int maxReportRequest;

};

#endif // USBHIDCOMMUNICATION_HXX
