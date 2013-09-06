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

#include "USBHIDCommunication.hxx"

#define WRITE_MODE true
#define READ_MODE false

#define DEVICE_STRING_BUFFER_SIZE 1000
#define VID_STRING 0x1325   // austriamicrosystems VID
#define PID_STRING 0xC400   // "Standard Linear WL - Other" PID mask

#define IN_REPORT_ID 0x01
#define OUT_REPORT_ID 0x02

#define C_READ_REGISTER 0x02
#define C_WRITE_REGISTER 0x01



USBHIDCommunication::USBHIDCommunication(unsigned char devAddr)
{
	//this->hinstLib = LoadLibraryA("./libraries/SLABHIDDevice.dll");
	this->devAddr = devAddr;

	// set some default values
	this->pid = PID_STRING | devAddr;
	this->vid = VID_STRING;
	this->inReportID = IN_REPORT_ID;
	this->outReportID = OUT_REPORT_ID;
	this->readRegisterCommand = C_READ_REGISTER;
	this->writeRegisterCommand = C_WRITE_REGISTER;

	this->hidDevice = NULL;
}

USBHIDCommunication::~USBHIDCommunication()
{
    //FreeLibrary(hinstLib);
}

AMSCommunication::Error USBHIDCommunication::hwConnect()
{
	Error usbHidError = NoError;

	int hidDeviceCount, hidStringByte;
	GUID hidGuid;
	LPSTR deviceString;
	char deviceStringBuffer[DEVICE_STRING_BUFFER_SIZE];
	QString hidVidString, hidPidString;

	deviceString = deviceStringBuffer;

	hidDeviceCount = GetNumHidDevices(0, 0);   // number of HID devices connected to the computer
	hidDeviceCount = GetNumHidDevices(this->vid, this->pid);   // number of desired boards connected to the computer
	if(hidDeviceCount > 0 && hidDevice == NULL)   // at least one board found, and we are not already connected to this board
	{
		this->hidDevice = new CHIDDevice();

		// get some additional information from the device descriptor
		hidStringByte = GetHidString(0, this->vid, this->pid, HID_VID_STRING, deviceString, DEVICE_STRING_BUFFER_SIZE);
		hidVidString = deviceString;
		hidStringByte = GetHidString(0, this->vid, this->pid, HID_PID_STRING, deviceString, DEVICE_STRING_BUFFER_SIZE);
		hidPidString = deviceString;
		hidStringByte = GetHidString(0, this->vid, this->pid, HID_PATH_STRING, deviceString, DEVICE_STRING_BUFFER_SIZE);
		hidStringByte = GetHidString(0, this->vid, this->pid, HID_SERIAL_STRING, deviceString, DEVICE_STRING_BUFFER_SIZE);
		GetHidGuid(&hidGuid);

		if(hidDevice->Open(0, this->vid, this->pid) == HID_DEVICE_SUCCESS)   // try to open the device
		{
			if(hidDevice->IsOpened())
			{
				connected = true;

				// get all kind of information from device descriptor
				hidDevice->GetString(HID_SERIAL_STRING, deviceString, DEVICE_STRING_BUFFER_SIZE);
				this->inputReportBufferLength = hidDevice->GetInputReportBufferLength();   // 0x11
				this->outputReportBufferLength = hidDevice->GetOutputReportBufferLength();   // 0x13
				this->featureReportBufferLength = hidDevice->GetFeatureReportBufferLength();   // 0x00
				this->maxReportRequest = hidDevice->GetMaxReportRequest();   // 0x20
				unsigned int readReportTimeout, writeReportTimeout;
				hidDevice->GetTimeouts(&readReportTimeout, &writeReportTimeout);

				hidDevice->SetTimeouts(10000, 10000);
			}
			else
			{
				usbHidError = ConnectionError;
			}
		}
		else
		{
			delete this->hidDevice;
			this->hidDevice = NULL;
			usbHidError = ConnectionError;
		}
	}
	else
	{
		// either we are already connected and everything is ok,
		// or
		if(hidDeviceCount == 0)
			usbHidError = ConnectionError;
	}

	return usbHidError;
}

void USBHIDCommunication::hwDisconnect()
{
	UsbHidError usbHidError = ERR_USBHID_NO_ERR;

	if(hidDevice)
	{
		if(hidDevice->IsOpened())
		{
			if(hidDevice->Close() == HID_DEVICE_SUCCESS)
			{
				if (!hidDevice->IsOpened())
				{
					connected = false;
					delete this->hidDevice;
					this->hidDevice = NULL;
				}
			}
		}
	}
}

bool USBHIDCommunication::isConnected()
{
	if(GetNumHidDevices(this->vid, this->pid))
	{
		// at least one board with correct PID is connected
		connected = true;
	}
	else
	{
		connected = false;
	}

	return connected;
}

AMSCommunication::Error USBHIDCommunication::hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
{
	unsigned char buffer[DEVICE_STRING_BUFFER_SIZE];
	BYTE result;

	UsbHidError usbHidError = ERR_USBHID_NO_ERR;
	AMSCommunication::Error err = NoError;
#if 1
	if (connected)
	{
		// first tell the device which address to read from
		buffer[0] = this->outReportID;
		buffer[1] = this->readRegisterCommand;   // cReadRegister
		buffer[2] = registerAddress;
		result = this->hidDevice->SetOutputReport_Control((BYTE*)buffer, DEVICE_STRING_BUFFER_SIZE);
		if(result != HID_DEVICE_SUCCESS)
		{
			usbHidError = ERR_USBHID_GENERIC;
		}

		// now ask the device for the value
		buffer[0] = this->inReportID;
		result = this->hidDevice->GetInputReport_Control((BYTE*)buffer, DEVICE_STRING_BUFFER_SIZE);
		if(result != HID_DEVICE_SUCCESS)
		{
			usbHidError = ERR_USBHID_GENERIC;
		}
		else
		{
#if 0
			// for the AS3900 firmware the following has to be true
			if(buffer[1] != registerAddress)
			{
				usbHidError = ERR_USBHID_CMD_FAILED;
			}
#endif
			*registerValue = buffer[0];
		}
	}
	else
	{
		usbHidError = ERR_USBHID_NOT_CONNECTED;
	}

	if (usbHidError  == ERR_USBHID_NOT_CONNECTED)
		err = ConnectionError;
	else if (usbHidError == ERR_USBHID_NO_ERR)
		err = NoError;
	else
		err = ReadError;
#endif
	return err;
}


  /** USBCommunication::writeByte
   * This function sends one Byte to the specified address
   * In case of success ERR_NO_ERR is returned
   */
AMSCommunication::Error USBHIDCommunication::hwWriteRegister(unsigned char registerAddress, unsigned char registerValue)
{
	unsigned char buffer[DEVICE_STRING_BUFFER_SIZE];
	BYTE result;

	UsbHidError usbHidError = ERR_USBHID_NO_ERR;
	AMSCommunication::Error err = NoError;
#if 1
	if (connected)
	{
		buffer[0] = this->outReportID;
		buffer[1] = this->writeRegisterCommand;   // cWriteRegister
		buffer[2] = registerAddress;
		buffer[3] = registerValue;
		result = this->hidDevice->SetOutputReport_Control((BYTE*)buffer, DEVICE_STRING_BUFFER_SIZE);
		if(result != HID_DEVICE_SUCCESS)
		{
			usbHidError = ERR_USBHID_GENERIC;
		}
	}
	else
	{
		usbHidError = ERR_USBHID_NOT_CONNECTED;
	}

	if (usbHidError  == ERR_USBHID_NOT_CONNECTED)
		err = ConnectionError;
	else if (usbHidError == ERR_USBHID_NO_ERR)
		err = NoError;
	else
		err = WriteError;
#endif
	return err;
}

  /** USBHIDCommunication::hwSendCommand
   * This function can be used to send special commands to the device
   */
AMSCommunication::Error USBHIDCommunication::hwSendCommand(QString command, QString * answer)
{
	unsigned char buffer[DEVICE_STRING_BUFFER_SIZE];
	BYTE result;
	int openIdx, closeIdx, i;
	bool ok;
	DWORD bytesReturned;

	UsbHidError usbHidError = ERR_USBHID_NO_ERR;
	AMSCommunication::Error err = NoError;

	if (connected)
	{
		memset(buffer, 0x00, DEVICE_STRING_BUFFER_SIZE);

		if(command.contains("GetInputReport_Control"))
		{
			openIdx = command.indexOf('(');
			closeIdx = command.indexOf(')', openIdx);
			command = command.mid(openIdx + 1, closeIdx - openIdx - 1);
			buffer[0] = command.left(2).toInt(&ok, 16);
			result = this->hidDevice->GetInputReport_Control((BYTE*)buffer, DEVICE_STRING_BUFFER_SIZE);
			if(result != HID_DEVICE_SUCCESS)
			{
				usbHidError = ERR_USBHID_GENERIC;
			}
			else
			{
				answer->clear();
				for(int i = 0; i < DEVICE_STRING_BUFFER_SIZE; i++)
				{
					*answer += QString::QString("%1").arg(QString::number(buffer[i], 16), 2, '0');
				}
			}
		}
		else if(command.contains("SetOutputReport_Control"))
		{
			openIdx = command.indexOf('(');
			closeIdx = command.indexOf(')', openIdx);
			command = command.mid(openIdx + 1, closeIdx - openIdx - 1);
			i = 0;
			while((i*2 < command.size()) && (i < DEVICE_STRING_BUFFER_SIZE))
			{
				buffer[i++] = command.mid(i*2, 2).toInt(&ok, 16);
			}
			result = this->hidDevice->SetOutputReport_Control((BYTE*)buffer, DEVICE_STRING_BUFFER_SIZE);
			if(result != HID_DEVICE_SUCCESS)
			{
				usbHidError = ERR_USBHID_GENERIC;
			}
		}
		else if (command.contains("GetInputReport_Interrupt"))
		{
			openIdx = command.indexOf('(');
			closeIdx = command.indexOf(')', openIdx);
			command = command.mid(openIdx + 1, closeIdx - openIdx - 1);
			buffer[0] = command.left(2).toInt(&ok, 16);
			result = this->hidDevice->GetInputReport_Interrupt((BYTE*)buffer
								, this->hidDevice->GetInputReportBufferLength(), 1, &bytesReturned);
			if(result != HID_DEVICE_SUCCESS)
			{
				usbHidError = ERR_USBHID_GENERIC;
			}
			else
			{
				answer->clear();
				for(int i = 0; i < DEVICE_STRING_BUFFER_SIZE; i++)
				{
					*answer += QString::QString("%1").arg(QString::number(buffer[i], 16), 2, '0');
				}
			}
		}
		else if(command.contains("SetOutputReport_Interrupt"))
		{
			openIdx = command.indexOf('(');
			closeIdx = command.indexOf(')', openIdx);
			command = command.mid(openIdx + 1, closeIdx - openIdx - 1);
			i = 0;
			while((i*2 < command.size()) && (i < DEVICE_STRING_BUFFER_SIZE))
			{
				buffer[i++] = command.mid(i*2, 2).toInt(&ok, 16);
			}
			result = this->hidDevice->SetOutputReport_Interrupt((BYTE*)buffer, this->hidDevice->GetOutputReportBufferLength());
			if(result != HID_DEVICE_SUCCESS)
			{
				usbHidError = ERR_USBHID_GENERIC;
			}
		}
		else
		{
			//unknown command
		}
	}
	else
	{
		usbHidError = ERR_USBHID_NOT_CONNECTED;
	}

	if (usbHidError  == ERR_USBHID_NOT_CONNECTED)
		err = ConnectionError;
	else if (usbHidError == ERR_USBHID_NO_ERR)
		err = NoError;
	else
		err = WriteError;

	return err;
}

UsbHidError USBHIDCommunication::setI2CDeviceAddress(unsigned char registerAddress)
{
	UsbHidError usbHidError = ERR_USBHID_NO_ERR;

	this->devAddr = devAddr;
	this->pid = PID_STRING | devAddr;

	return usbHidError;
}

// this function has to be called right after constructing the class to overwrite the default parameters for your device
// the register map already depends in the readOnce function at startup on valid IDs to be able to read out the registers
void USBHIDCommunication::setConnectionProperties(void* properties)
{
	// holds the PID of the desired HID device
	this->pid = ((usbhidConfigStructure*)properties)->pid;

	// holds the VID of the desired HID device
	this->vid = ((usbhidConfigStructure*)properties)->vid;

	this->inReportID = ((usbhidConfigStructure*)properties)->inReportID;

	this->outReportID = ((usbhidConfigStructure*)properties)->outReportID;

	// holds the ID of the command which is implemented on the device to store the address where we want to read from and which returns a register value
	// the command with this ID will be used to send the address we want to read from to the device and then the device returns the value of the register at the specific address
	// the device is responsible to store this address until the actual read out is done
	// this command ID is used in hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
	this->readRegisterCommand = ((usbhidConfigStructure*)properties)->readRegisterCommand;

	// holds the ID of the command which writes a byte at a given address into the register of the device
	// this command ID is used in hwWriteRegister(unsigned char registerAddress, unsigned char registerValue)
	this->writeRegisterCommand = ((usbhidConfigStructure*)properties)->writeRegisterCommand;
}

void USBHIDCommunication::getConnectionProperties(void* properties)
{
	((usbhidConfigStructure*)properties)->pid = this->pid;
	((usbhidConfigStructure*)properties)->vid = this->vid;
	((usbhidConfigStructure*)properties)->inReportID = this->inReportID;
	((usbhidConfigStructure*)properties)->outReportID = this->outReportID;
	((usbhidConfigStructure*)properties)->readRegisterCommand = this->readRegisterCommand;
	((usbhidConfigStructure*)properties)->writeRegisterCommand = this->writeRegisterCommand;
}
