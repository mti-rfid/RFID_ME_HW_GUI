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
 *  \brief  Communication class for communication via winusb.dll
 *
 */

#ifndef WINUSB_DLL_COMMS_H
#define WINUSB_DLL_COMMS_H

#include <windows.h>
#include <Setupapi.h>
#include <stdio.h>
#include "AMSCommunication.hxx"
#include "winusb.h"

typedef enum {
  ERR_WINUSBDLL_NO_ERR, /**< No error occurred. */
  ERR_WINUSBDLL_CMD_FAILED, /**< The uC encountered an error. */
  ERR_WINUSBDLL_CANNOT_CONNECT, /**< The demoboard is probably not plugged in. */
  ERR_WINUSBDLL_NOT_CONNECTED, /**< Transfer function was called on a closed connection */
  ERR_WINUSBDLL_TRANSMIT, /**< An error occurred during the transmission of a command. */
  ERR_WINUSBDLL_RECEIVE, /**< An error occurred during reception. */
  ERR_WINUSBDLL_ILLEGAL_ARGUMENTS, /**< The arguments are out of bounds. */
  ERR_WINUSBDLL_GENERIC /**< An unspecified error occurred. */
} WinUsbDllError;

struct winUsbDllConfigStructure
{
	int pid;
	int vid;
	int outPacketLength;
	int inPacketLength;
	unsigned char outReadRegisterPacketId;
	unsigned char inReadRegisterPacketId;
	unsigned char outWriteRegisterPacketId;
};

class WINUSBDLLCommunication : public AMSCommunication
{

public:
	WINUSBDLLCommunication(unsigned char devAddr);
	~WINUSBDLLCommunication();

	QString getDeviceName();
	int getUSBRequest(WINUSB_SETUP_PACKET &requestCntlPacket, int wUsbTimeoutmSec, unsigned char *pbdata);
	int writeUSBVendorRequest(UCHAR Request, USHORT Length, unsigned char *pbdata, USHORT Index, USHORT Value, int wUsbTimeoutmSec);
	int readUSBVendorRequest(UCHAR Request, USHORT Length, unsigned char *pbdata, USHORT Index, USHORT Value, int wUsbTimeoutmSec);
	void closeDevice(void);
	bool openDeviceByName(QString newdeviceName);
	HANDLE openFile(QString filename);

	int getFromDev(unsigned char pipeToPc, int maxbytesToDo, unsigned char *pBytes, HANDLE hStopEvent, DWORD timeOut);

	AMSCommunication::Error hwConnect();
	void hwDisconnect();
	bool isConnected();
	AMSCommunication::Error hwReadRegister(unsigned char reg, unsigned char *val);
	AMSCommunication::Error hwWriteRegister(unsigned char reg, unsigned char val);
	AMSCommunication::Error hwSendCommand(QString command, QString * answer);

	void disconnect();

public slots:
	void deviceNodeChanged();
	
protected:
	void setConnectionProperties(void *);

private:
	WinUsbDllError setI2CDeviceAddress(unsigned char registerAddress);
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

	GUID m_GUID;

	HANDLE hDevice;
	WINUSB_INTERFACE_HANDLE devUsbHnd;

	HANDLE hToPcStreamOvrlp;	/* overlapped to device */
	OVERLAPPED ovrLapToPcStream ;

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
	int outPacketLength;
	int inPacketLength;
	unsigned char outReadRegisterPacketId;
	unsigned char inReadRegisterPacketId;
	unsigned char outWriteRegisterPacketId;
};

#endif // WINUSBDLLCOMMS_H
