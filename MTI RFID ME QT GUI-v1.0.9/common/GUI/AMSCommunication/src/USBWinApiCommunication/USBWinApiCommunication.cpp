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

#include "USBWinApiCommunication.hxx"

#define WRITE_MODE true
#define READ_MODE false

#define MAX_PACKET_LENGTH 1000

#define DEFAULT_OUTPACKET_LENGTH 64
#define DEFAULT_INPACKET_LENGTH 64

#define VID_STRING 0x1325   // austriamicrosystems VID
#define PID_STRING 0xC400   // "Standard Linear WL - Other" PID mask

#define OUT_READREGISTER_ID 4
#define IN_READREGISTER_ID 5
#define OUT_WRITEREGISTER_ID 6



#define MAX_DEVPATH_LENGTH 1000
#define ID_SIZE 1000



#include <initguid.h>		// Required for GUID definition
DEFINE_GUID(GUID_INTERFACE, 
//0x96E73B6E, 0x7A5A, 0x11D4, 0x9F, 0x24, 0x00, 0x80, 0xC8, 0x27, 0x27, 0xF4);   // fourth device found, "USB\VID_1325&PID_06EC&MI_01\6&2AD539BE&0&0001"
//0x745A17A0, 0x74D3, 0x11D0, 0xB6, 0xFE, 0x00, 0xA0, 0xC9, 0x0F, 0x57, 0xDA);   // third device found, "USB\VID_1325&PID_06EC&MI_00\6&2AD539BE&0&0000"
//0x36FC9E60, 0xC465, 0x11CF, 0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);   // second device found, "USB\VID_1325&PID_06EC\5&1E55E395&0&2"
//0x4D36E96F, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);   // first device found, "HID\VID_1325&PID_06EC&MI_00\7&1F428242&0&0000"
0xdda31245, 0x1bfc, 0x4225, 0xb2, 0xb8, 0xea, 0xaa, 0xb2, 0xe3, 0x90, 0xb6);   // used in original SW

// the following GUIDs are received using the silabs dll trying to connect to the AS5011 board
//0x96e73b6e, 0x7a5a, 0x11d4, 0x9f, 0x24, 0x00, 0x80, 0xc8, 0x27, 0x27, 0xf4);   // used in Mouse_Int.inf
//deviceString = 0x0012b128 "\\?\hid#vid_1325&pid_06ec&mi_00#7&1f428242&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"
//deviceString = 0x0012b128 "\\?\hid#vid_1325&pid_06ec&mi_00#7&1f428242&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"
//                                                                   hidGuid = {4D1E55B2-F16F-11CF-88CB-001111000030}   <-- ausprobieren!!!



USBWinApiCommunication::USBWinApiCommunication(unsigned char devAddr)
{
	//this->hinstLib = LoadLibraryA("./libraries/SLABHIDDevice.dll");
	this->devAddr = devAddr;

	// set some default values
	this->pid = PID_STRING | devAddr;
	this->vid = VID_STRING;
	this->outPacketLength = DEFAULT_OUTPACKET_LENGTH;
	this->inPacketLength = DEFAULT_INPACKET_LENGTH;
	this->outReadRegisterPacketId = OUT_READREGISTER_ID;
	this->inReadRegisterPacketId = IN_READREGISTER_ID;
	this->outWriteRegisterPacketId = OUT_WRITEREGISTER_ID;

	/* FIXME: we still need that, becaue otherwise the device would not be found */
	m_GUID = GUID_INTERFACE;
	this->hUsbRead = INVALID_HANDLE_VALUE;
	this->hUsbWrite = INVALID_HANDLE_VALUE;
}

USBWinApiCommunication::~USBWinApiCommunication()
{
}

bool USBWinApiCommunication::openUsbDevice()
{
	char szBufferID[ID_SIZE];
	BOOL bRet = FALSE;
	DWORD error;

	// Retrieve device list for GUID that has been specified.
	HDEVINFO hDevInfoList = SetupDiGetClassDevs (&m_GUID, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
	//HDEVINFO hDevInfoList = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if (hDevInfoList == INVALID_HANDLE_VALUE)
		error = GetLastError();

	if (hDevInfoList != NULL)
	{
		SP_DEVICE_INTERFACE_DATA deviceInfoData;
		SP_DEVINFO_DATA devInfoData;
		int MemberIndex;
		QString vidString, pidString;

		vidString = QString::QString("%1").arg(QString::number(this->vid, 16), 4, '0');
		pidString = QString::QString("%1").arg(QString::number(this->pid, 16), 4, '0');
		MemberIndex = 0;

		do
		{
			// Clear data structure
			ZeroMemory(&deviceInfoData, sizeof(deviceInfoData));
			deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

			// get device info (VID, PID, etc.)
			devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
			if (SetupDiEnumDeviceInfo (hDevInfoList, MemberIndex, &devInfoData))
			{
				if (SetupDiGetDeviceInstanceId(hDevInfoList, &devInfoData, szBufferID, ID_SIZE, NULL))
				{
					QString checkBuffer = szBufferID;
					if (checkBuffer.contains(vidString, Qt::CaseInsensitive) && checkBuffer.contains(pidString, Qt::CaseInsensitive))
					{
						//m_GUID = devInfoData.ClassGuid;
						this->connected = true;
					}
				}
			}

			// retrieves a context structure for a device interface of a device information set.
			if (SetupDiEnumDeviceInterfaces (hDevInfoList, 0, &m_GUID, MemberIndex, &deviceInfoData)) 
			{
				// Must get the detailed information in two steps
				// First get the length of the detailed information and allocate the buffer
				// retrieves detailed information about a specified device interface.
				PSP_DEVICE_INTERFACE_DETAIL_DATA     functionClassDeviceData = NULL;
				ULONG  predictedLength, requiredLength;
				predictedLength = requiredLength = 0;
				SetupDiGetDeviceInterfaceDetail (
						hDevInfoList,
						&deviceInfoData,
						NULL,			// Not yet allocated
						0,				// Set output buffer length to zero 
						&requiredLength,// Find out memory requirement
						NULL);

				predictedLength = requiredLength;
				functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc (predictedLength);
				functionClassDeviceData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);

				// Second, get the detailed information
				if ( SetupDiGetDeviceInterfaceDetail (
							hDevInfoList,
							&deviceInfoData,
							functionClassDeviceData,
							predictedLength,
							&requiredLength,
							NULL)) 
				{
					// Save the device name for subsequent pipe open calls
					m_DeviceName = functionClassDeviceData->DevicePath;
					free( functionClassDeviceData );
					bRet = TRUE;
					break;
				}

				free( functionClassDeviceData );
			}

			MemberIndex++;
		}while (MemberIndex < 256);//(GetLastError() != ERROR_NO_MORE_ITEMS);
	}

	// SetupDiDestroyDeviceInfoList() destroys a device information set
	// and frees all associated memory.
	SetupDiDestroyDeviceInfoList (hDevInfoList);

	return bRet;
}

HANDLE USBWinApiCommunication::openFile(QString filename)
{
	HANDLE hFile;

	if(!filename.isEmpty())
	{
		m_DeviceName.append("\\");                      
		m_DeviceName.append(filename);                                      
	}

	hFile = CreateFile(	m_DeviceName.toAscii(),
					GENERIC_WRITE | GENERIC_READ,
					FILE_SHARE_WRITE | FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					0,
					NULL);

	return hFile;
}

AMSCommunication::Error USBWinApiCommunication::hwConnect()
{
	Error winUsbError = NoError;

	if(m_DeviceName.isEmpty())
	{
		if (openUsbDevice())
		{
			hUsbRead = openFile("PIPE00");
			if (hUsbRead == INVALID_HANDLE_VALUE)
			{
				winUsbError = ConnectionError;
			}
			hUsbWrite = openFile("PIPE01");
			if (hUsbWrite == INVALID_HANDLE_VALUE)
			{
				winUsbError = ConnectionError;
			}
		}
		else
		{
			winUsbError = ConnectionError;
		}
	}

	return winUsbError;
}

void USBWinApiCommunication::hwDisconnect()
{
	WinUsbError winUsbError = ERR_WINUSB_NO_ERR;

	if (hUsbRead != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hUsbRead);
		hUsbRead = INVALID_HANDLE_VALUE;
	}
	if (hUsbWrite != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hUsbWrite);
		hUsbWrite = INVALID_HANDLE_VALUE;
	}

	m_DeviceName.clear();
	this->connected = false;
}

bool USBWinApiCommunication::isConnected()
{
	/* if device should be still connected, search for PID, VID if this is still the case */
	if (connected)
	{
		return openUsbDevice();
	}
	/* otherwise, it does not matter, we have to connect to the device first */
	else
	{
		return false;
	}
}

AMSCommunication::Error USBWinApiCommunication::hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
{
	unsigned char buffer[MAX_PACKET_LENGTH];
	bool result;
	DWORD NrOfBytesWritten;
	DWORD NrOfBytesRead;

	WinUsbError winUsbError = ERR_WINUSB_NO_ERR;
	AMSCommunication::Error err = NoError;

	if (connected)
	{
		// first tell the device which address to read from
		buffer[0] = this->outReadRegisterPacketId;
		buffer[1] = registerAddress;

		/* write register address */
		if (hUsbWrite != INVALID_HANDLE_VALUE)
		{
			result = WriteFile(hUsbWrite, &buffer, outPacketLength, &NrOfBytesWritten, NULL);
			if(result == false)
			{
				winUsbError = ERR_WINUSB_GENERIC;
			}
		}
		else
		{
			winUsbError = ERR_WINUSB_GENERIC;
		}

		/* give firmware time to respond */
		Sleep(50);

		/* read register value */
		if (hUsbRead != INVALID_HANDLE_VALUE)
		{
			result = ReadFile(hUsbRead, &buffer, inPacketLength, &NrOfBytesRead, NULL);
			if(result == false)
			{
				winUsbError = ERR_WINUSB_CMD_FAILED;
			}
			else
			{
				*registerValue = buffer[0];
			}
		}
		else
		{
			winUsbError = ERR_WINUSB_CMD_FAILED;
		}
	}
	else
	{
		winUsbError = ERR_WINUSB_NOT_CONNECTED;
	}

	if (winUsbError  == ERR_WINUSB_NOT_CONNECTED)
		err = ConnectionError;
	else if (winUsbError == ERR_WINUSB_NO_ERR)
		err = NoError;
	else
		err = ReadError;

	return err;
}


  /**
   * This function sends one Byte to the specified address
   * In case of success ERR_NO_ERR is returned
   */
AMSCommunication::Error USBWinApiCommunication::hwWriteRegister(unsigned char registerAddress, unsigned char registerValue)
{
	unsigned char buffer[MAX_PACKET_LENGTH];
	bool result;

	WinUsbError winUsbError = ERR_WINUSB_NO_ERR;
	AMSCommunication::Error err = NoError;

	if (connected)
	{
		DWORD NrOfBytesWritten;

		buffer[0] = this->outWriteRegisterPacketId;
		buffer[1] = registerAddress;
		buffer[2] = registerValue;
		if (hUsbWrite != INVALID_HANDLE_VALUE)
		{
			result = WriteFile(hUsbWrite, &buffer, outPacketLength, &NrOfBytesWritten, NULL);
			if(result == false)
			{
				winUsbError = ERR_WINUSB_GENERIC;
			}
		}
		else
		{
			winUsbError = ERR_WINUSB_GENERIC;
		}
	}
	else
	{
		winUsbError = ERR_WINUSB_NOT_CONNECTED;
	}

	if (winUsbError  == ERR_WINUSB_NOT_CONNECTED)
		err = ConnectionError;
	else if (winUsbError == ERR_WINUSB_NO_ERR)
		err = NoError;
	else
		err = WriteError;

	return err;
}

  /**
   * This function can be used to send special commands to the device
   */
AMSCommunication::Error USBWinApiCommunication::hwSendCommand(QString command, QString * answer)
{
	unsigned char buffer[MAX_PACKET_LENGTH];
	bool result;
	int openIdx, closeIdx, i;
	bool ok;

	WinUsbError winUsbError = ERR_WINUSB_NO_ERR;
	AMSCommunication::Error err = NoError;

	if (connected)
	{
		if (command.contains("SendData"))
		{
			DWORD NrOfBytesWritten;

			openIdx = command.indexOf('(');
			closeIdx = command.indexOf(')', openIdx);
			command = command.mid(openIdx + 1, closeIdx - openIdx - 1);
			i = 0;
			while((i*2 < command.size()) && (i < outPacketLength))
			{
				buffer[i++] = command.mid(i*2, 2).toInt(&ok, 16);
			}
			if (hUsbWrite != INVALID_HANDLE_VALUE)
			{
				result = WriteFile(hUsbWrite, &buffer, outPacketLength, &NrOfBytesWritten, NULL);
				if (result == false)
				{
					winUsbError = ERR_WINUSB_CMD_FAILED;
				}
			}
			else
			{
				winUsbError = ERR_WINUSB_CMD_FAILED;
			}
		}
		else if(command.contains("ReceiveData"))
		{
			DWORD NrOfBytesRead;
			int NrOfBytesToRead;

			openIdx = command.indexOf('(');
			closeIdx = command.indexOf(')', openIdx);
			command = command.mid(openIdx + 1, closeIdx - openIdx - 1);
			NrOfBytesToRead = command.left(2).toInt(&ok, 16);
			if (hUsbRead != INVALID_HANDLE_VALUE)
			{
				result = ReadFile(hUsbRead, &buffer, inPacketLength, &NrOfBytesRead, NULL);
				if(result == false)
				{
					winUsbError = ERR_WINUSB_CMD_FAILED;
				}
				else
				{
					answer->clear();
					for(int i = 0; i < NrOfBytesToRead; i++)
					{
						*answer += QString::QString("%1").arg(QString::number(buffer[i], 16), 2, '0');
					}
				}
			}
			else
			{
				winUsbError = ERR_WINUSB_CMD_FAILED;
			}
		}
		else
		{
			// unknown command
		}
	}
	else
	{
		winUsbError = ERR_WINUSB_NOT_CONNECTED;
	}
	if (winUsbError  == ERR_WINUSB_NOT_CONNECTED)
		err = ConnectionError;
	else if (winUsbError == ERR_WINUSB_NO_ERR)
		err = NoError;
	else
		err = WriteError;

	return err;
}

WinUsbError USBWinApiCommunication::setI2CDeviceAddress(unsigned char registerAddress)
{
	WinUsbError winUsbError = ERR_WINUSB_NO_ERR;

	this->devAddr = devAddr;
	this->pid = PID_STRING | devAddr;

	return winUsbError;
}

// this function has to be called right after constructing the class to overwrite the default parameters for your device
// the register map already depends in the readOnce function at startup on valid IDs to be able to read out the registers
void USBWinApiCommunication::setConnectionProperties(void* properties)
{
	// holds the PID of the desired HID device
	this->pid = ((configStructure*)properties)->pid;

	// holds the VID of the desired HID device
	this->vid = ((configStructure*)properties)->vid;

	// holds the packet length of IN packets
	// this option MUST be set to the correct value which corresponds with the device firmware
	// otherwise windows might produce a bluescreen
	this->outPacketLength = ((configStructure*)properties)->outPacketLength;

	// holds the packet length of OUT packets
	// this option MUST be set to the correct value which corresponds with the device firmware
	// otherwise windows might produce a bluescreen
	this->inPacketLength = ((configStructure*)properties)->inPacketLength;

	// holds the ID of the Packte which is implemented on the device to store the address where we want to read from
	// the packet with this ID will be used to send the address we want to read from to the device
	// the device is responsible to store this address until the actual read out is done
	// this packet ID is used in hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
	this->outReadRegisterPacketId = ((configStructure*)properties)->outReadRegisterPacketId;

	// holds the ID of the packet which returns a register value
	// in this packet the device writes the value of the register which address was set with the packet before
	// this packet ID is used in hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
	this->inReadRegisterPacketId = ((configStructure*)properties)->inReadRegisterPacketId;

	// holds the ID of the packet which writes a byte at a given address into the register of the device
	// this packet ID is used in hwWriteRegister(unsigned char registerAddress, unsigned char registerValue)
	this->outWriteRegisterPacketId = ((configStructure*)properties)->outWriteRegisterPacketId;
}
