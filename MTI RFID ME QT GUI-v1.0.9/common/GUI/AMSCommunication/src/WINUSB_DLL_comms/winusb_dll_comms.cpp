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

#include "winusb_dll_comms.h"

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

#define READ_REGISTER_RECEIVE_BYTES 2



#define MAX_DEVPATH_LENGTH 1000
#define ID_SIZE 1000



#define USBAS3940_VENDORREQ_AS3940_DEVID              0xa8 /* 1 byte error code + 3 bytes data (device -> host) */
#define USBAS3940_VENDORREQ_FW_VERSION                0xa0 /* 1 byte error code + 8 bytes data (device -> host) */
#define USBAS3940_VENDORREQ_STATUS                    0xa1 /* 1 byte error code + 4 bytes data (device -> host) (destructive read)*/
#define USBAS3940_VENDORREQ_ACTIVE_CLIENTS            0xac /* requests all available clients on the datapipe USBAS3940_REPORT_TAG_CLIENT1..9_INFO  */
#define USBAS3940_VENDORREQ_GET_MASTER_BATTERY_LEVEL  0xad /* 1 byte error code +  current master battery level 1 byte bit7=1=low 0=high  */

#define USBAS3940_SPI_CLIENTINFO_TABLE_BASE	   0x30 /* base device type table */
#define USBAS3940_SPI_CLIENTINFO_TABLE_ELMLEN  10 /* base device type elemnt length in bytes */
#define USBAS3940_VENDORREQ_SPI_READ                  0xa9 /* 1 byte error code + n bytes data (device -> host) ENGINEERING */
#define USBAS3940_VENDORREQ_SPI_WRITE                 0xaa /* n bytes data (host -> device) ENGINEERING  */

#define USBAS3940_VENDORREQ_CONFIG_UCONTROLLER_READ   0xa2 /* 1 byte error code + 512 bytes data (device -> host) */
#define USBAS3940_VENDORREQ_CONFIG_UCONTROLLER_WRITE  0xa3 /* 512 bytes data (host -> device) */
#define USBAS3940_VENDORREQ_CONFIG_MASTER_READ        0xa4 /* 1 byte error code + 256 bytes data (device -> host) */
#define USBAS3940_VENDORREQ_CONFIG_MASTER_WRITE       0xa5 /* 256 bytes data (host -> device) */
#define USBAS3940_VENDORREQ_CONFIG_CLIENT_READ        0xa6 /* 1 byte error code + 256 bytes data (device -> host) */
#define USBAS3940_VENDORREQ_CONFIG_CLIENT_WRITE       0xa7 /* 256 bytes data (host -> device) */

#define CUSBDEVICE_VENDOR_REQUEST	0x40

#define CPROPPAG_STREAM_PIPE_TO_PC  0x83
#define COMOPROPSHEET_MSG_THREAD_DATA_TO_PC (WM_USER +82)



#define DEBUG_SWITCH_SHOW_RECEIVED_BYTES 0



#include <initguid.h>		// Required for GUID definition
DEFINE_GUID(GUID_INTERFACE,
            0x06DFB896, 0xBC62, 0x46e6, 0x87, 0x85, 0x68, 0x44, 0x4E, 0x7D, 0xD9, 0xB9);   // WinUSB Ams 3940

// for more information on the winusb.dll API see
// http://msdn.microsoft.com/en-us/library/aa476437.aspx

WINUSBDLLCommunication::WINUSBDLLCommunication(unsigned char devAddr)
{
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

    this->hDevice = NULL;
    this->devUsbHnd = NULL;
    this->hToPcStreamOvrlp = NULL;
}

WINUSBDLLCommunication::~WINUSBDLLCommunication()
{
}

QString WINUSBDLLCommunication::getDeviceName()
{
    char szBufferID[ID_SIZE];
    BOOL bRet = FALSE;
    DWORD error;
    QString m_DeviceName;

    // Retrieve device list for GUID that has been specified.
    HDEVINFO hDevInfoList = SetupDiGetClassDevs (&m_GUID, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
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
			// until here, and no further...
			// seems as if SetupDiEnumDeviceInfo never returns true in Release Mode, it is working in Debug Mode
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
			else   // !SetupDiEnumDeviceInfo
			{
				//QMessageBox::information(0, "SetupDiEnumDeviceInfo GetLastError", QString::QString("%1").arg(QString::number(GetLastError())));
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
			else   // !SetupDiEnumDeviceInterfaces
			{
				//QMessageBox::information(0, "SetupDiEnumDeviceInterfaces GetLastError", QString::QString("%1").arg(QString::number(GetLastError())));
            }

            MemberIndex++;
        }
        while (MemberIndex < 256);//(GetLastError() != ERROR_NO_MORE_ITEMS);
    }

    // SetupDiDestroyDeviceInfoList() destroys a device information set
    // and frees all associated memory.
    SetupDiDestroyDeviceInfoList (hDevInfoList);

    return m_DeviceName;
}

// read/write transfer for ep0 data make it sycnhronous returns zero when nothing could be done, requested length in setup packet
int WINUSBDLLCommunication::getUSBRequest(WINUSB_SETUP_PACKET &requestCntlPacket, int wUsbTimeoutmSec, unsigned char *pbdata)
{
    int uiRet = 0;

    WINUSB_INTERFACE_HANDLE devUsbHndTmp = devUsbHnd;
    if ( hDevice != INVALID_HANDLE_VALUE )
    {
        ULONG xfrBytes = 0;
        DWORD errInfo  = 0;
        OVERLAPPED ovrLapCntl;
        memset(&ovrLapCntl,0,sizeof(ovrLapCntl));
        ovrLapCntl.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		//qDebug() << ovrLapCntl.hEvent;
        if ( ovrLapCntl.hEvent != NULL)
        {
            if (( WinUsb_ControlTransfer (  devUsbHndTmp, requestCntlPacket,  (PUCHAR) pbdata, requestCntlPacket.Length,
                                            &xfrBytes, &ovrLapCntl)   ) ||
                    ( GetLastError() ==  ERROR_IO_PENDING ))
            {
# if (DEBUG_SWITCH_SHOW_RECEIVED_BYTES == 1)
				if(xfrBytes != 0)
					qDebug() << "--- 1 --- " << xfrBytes;
#endif
                DWORD dWaited =  WaitForSingleObject(ovrLapCntl.hEvent , wUsbTimeoutmSec);
                xfrBytes = 0;
                if (WinUsb_GetOverlappedResult(devUsbHnd, &ovrLapCntl, &xfrBytes, FALSE) == 0)
                {
# if (DEBUG_SWITCH_SHOW_RECEIVED_BYTES == 1)
					qDebug() << xfrBytes;
#endif
                    errInfo = GetLastError();
                    CancelIo(devUsbHnd);
                    WaitForSingleObject(ovrLapCntl.hEvent , 100);
                }
# if (DEBUG_SWITCH_SHOW_RECEIVED_BYTES == 1)
				qDebug() << xfrBytes;
#endif
            }
            else
            {
                errInfo = GetLastError();
            }
            uiRet = (int) xfrBytes;
			//qDebug() << ovrLapCntl.hEvent;
            CloseHandle( ovrLapCntl.hEvent );
        }
    }

    return uiRet;
}

int WINUSBDLLCommunication::writeUSBVendorRequest(UCHAR Request, USHORT Length, unsigned char *pbdata, USHORT Index, USHORT Value, int wUsbTimeoutmSec)
{
    WINUSB_SETUP_PACKET requestCntlPacket;
    requestCntlPacket.Index = Index;
    requestCntlPacket.Length = Length;
    requestCntlPacket.Request = Request;
    requestCntlPacket.RequestType = CUSBDEVICE_VENDOR_REQUEST & 0x7F; /*write*/
    requestCntlPacket.Value = Value;
    return ( getUSBRequest(  requestCntlPacket, wUsbTimeoutmSec, pbdata));
}

int WINUSBDLLCommunication::readUSBVendorRequest(UCHAR Request, USHORT Length, unsigned char *pbdata, USHORT Index, USHORT Value, int wUsbTimeoutmSec)
{
    WINUSB_SETUP_PACKET requestCntlPacket;
    requestCntlPacket.Index = Index;
    requestCntlPacket.Length = Length;
    requestCntlPacket.Request = Request;
    requestCntlPacket.RequestType = CUSBDEVICE_VENDOR_REQUEST | 0x80; /*read*/
    requestCntlPacket.Value = Value;
    return ( getUSBRequest(  requestCntlPacket, wUsbTimeoutmSec, pbdata));
}

void WINUSBDLLCommunication::closeDevice(void)
{
    if ((hDevice != NULL) && (hDevice != INVALID_HANDLE_VALUE))
    {
        if ((devUsbHnd != NULL) && (devUsbHnd != INVALID_HANDLE_VALUE))
        {
            WinUsb_Free(devUsbHnd);
            devUsbHnd = INVALID_HANDLE_VALUE ;
        }
        // CancelIo(hDevice);	// be sure all do end
        CloseHandle(hDevice);
        hDevice = INVALID_HANDLE_VALUE;
    }
}

bool WINUSBDLLCommunication::openDeviceByName(QString newdeviceName)
{
    closeDevice();

    if (newdeviceName.isEmpty())
    {
        newdeviceName = getDeviceName();
    }
	
    hDevice = CreateFile(newdeviceName.toAscii(),
                         GENERIC_WRITE | GENERIC_READ,FILE_SHARE_WRITE | FILE_SHARE_READ,
                         NULL,  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

    if (hDevice != INVALID_HANDLE_VALUE)
    {
        if (!WinUsb_Initialize(hDevice, &devUsbHnd))
        {
            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
        }
        else   // get the data
        {
            ULONG bytesXfrd;
            QString deviceName = newdeviceName;
            QString deviceSerNr, deviceProduct, deviceManufacturer, languageList;
            USB_DEVICE_DESCRIPTOR  devDescr;	/* retrieved data */

			this->connected = true;
			
			if ((   WinUsb_GetDescriptor (devUsbHnd, USB_DEVICE_DESCRIPTOR_TYPE, 0, 0,
                                          (PUCHAR)(& devDescr), sizeof(devDescr), &bytesXfrd )   ) &&  (bytesXfrd == sizeof(devDescr)))
            {
                wchar_t USBString[256]; /* Absolute maximum size for string descriptors */
                _USB_STRING_DESCRIPTOR	*pcurStrDescr = (_USB_STRING_DESCRIPTOR*)(&USBString[0]); /* create full one */

                memset(USBString, 0, sizeof(USBString));
                if ( ( WinUsb_GetDescriptor (devUsbHnd, USB_STRING_DESCRIPTOR_TYPE, 0/* language */, 0,
                                             (PUCHAR)(pcurStrDescr),   sizeof(USBString ) , &bytesXfrd )   ) && (bytesXfrd > 2))
                {
                    languageList.fromUtf16((ushort*)&USBString[1]);    /* skip len and type */
                }



                memset(USBString, 0, sizeof(USBString));
                if ( ( WinUsb_GetDescriptor (devUsbHnd, USB_STRING_DESCRIPTOR_TYPE, devDescr.iSerialNumber, 0,
                                             (PUCHAR)(pcurStrDescr),    sizeof(USBString ), &bytesXfrd )   ) && (bytesXfrd > 2))
                {
                    deviceSerNr.fromUtf16((ushort*)&USBString[1]);    /* skip len and type */
                }

                memset(USBString, 0, sizeof(USBString));
                if ( ( WinUsb_GetDescriptor (devUsbHnd, USB_STRING_DESCRIPTOR_TYPE, devDescr.iProduct, 0,
                                             (PUCHAR)(pcurStrDescr),    sizeof(USBString ), &bytesXfrd )   ) && (bytesXfrd > 2))
                {
                    deviceProduct.fromUtf16((ushort*)&USBString[1]);    /* skip len and type */
                }

                memset(USBString, 0, sizeof(USBString));
                if ( ( WinUsb_GetDescriptor (devUsbHnd, USB_STRING_DESCRIPTOR_TYPE, devDescr.iManufacturer, 0,
                                             (PUCHAR)(pcurStrDescr),    sizeof(USBString ), &bytesXfrd )   ) && (bytesXfrd > 2))
                {
                    deviceManufacturer.fromUtf16((ushort*)&USBString[1]);    /* skip len and type */
                }
            }
        }
    }
    return ( hDevice != INVALID_HANDLE_VALUE);
}

AMSCommunication::Error WINUSBDLLCommunication::hwConnect()
{
    Error winUsbDllError = NoError;

    if ((hDevice == NULL) || (hDevice == INVALID_HANDLE_VALUE))
    {
#ifdef _DEBUG
		if (openDeviceByName(""))
#else
        if (openDeviceByName("\\\\?\\usb#vid_1325&pid_c428#2009#{06dfb896-bc62-46e6-8785-68444e7dd9b9}"))
#endif
        {
        }
        else
        {
            winUsbDllError = ConnectionError;
        }
    }

    return winUsbDllError;
}

void WINUSBDLLCommunication::hwDisconnect()
{
    WinUsbDllError winUsbDllError = ERR_WINUSBDLL_NO_ERR;

    closeDevice();

    this->connected = false;
}

bool WINUSBDLLCommunication::isConnected()
{
#ifdef _DEBUG
	return !getDeviceName().isEmpty();
#else
	return this->connected;
#endif
}

AMSCommunication::Error WINUSBDLLCommunication::hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
{
    int NrOfBytesRead;
    unsigned char receiveBuffer[READ_REGISTER_RECEIVE_BYTES];

    WinUsbDllError winUsbDllError = ERR_WINUSBDLL_NO_ERR;
    AMSCommunication::Error err = NoError;

    if (this->connected)
    {
        // readRegister
        NrOfBytesRead = readUSBVendorRequest(USBAS3940_VENDORREQ_SPI_READ, READ_REGISTER_RECEIVE_BYTES, receiveBuffer, registerAddress, 0, 100);
        if (NrOfBytesRead == READ_REGISTER_RECEIVE_BYTES)
        {
            if (receiveBuffer[0] == 0)  // no error
            {
                *registerValue = receiveBuffer[1];
            }
            else
            {
                winUsbDllError = ERR_WINUSBDLL_RECEIVE;
            }
        }
        else
        {
            winUsbDllError = ERR_WINUSBDLL_GENERIC;
        }
    }
    else
    {
        winUsbDllError = ERR_WINUSBDLL_NOT_CONNECTED;
    }

    if (winUsbDllError  == ERR_WINUSBDLL_NOT_CONNECTED)
        err = ConnectionError;
    else if (winUsbDllError == ERR_WINUSBDLL_NO_ERR)
        err = NoError;
    else
        err = ReadError;

    return err;
}


/**
 * This function sends one Byte to the specified address
 * In case of success ERR_NO_ERR is returned
 */
AMSCommunication::Error WINUSBDLLCommunication::hwWriteRegister(unsigned char registerAddress, unsigned char registerValue)
{
    int NrOfBytesWritten;

    WinUsbDllError winUsbDllError = ERR_WINUSBDLL_NO_ERR;
    AMSCommunication::Error err = NoError;

    if (this->connected)
    {
        // writeRegister
        NrOfBytesWritten = writeUSBVendorRequest(USBAS3940_VENDORREQ_SPI_WRITE, 1, &registerValue, registerAddress, 0, 100);
        if (NrOfBytesWritten != 1)
        {
            winUsbDllError = ERR_WINUSBDLL_GENERIC;
        }
    }
    else
    {
        winUsbDllError = ERR_WINUSBDLL_NOT_CONNECTED;
    }

    if (winUsbDllError  == ERR_WINUSBDLL_NOT_CONNECTED)
        err = ConnectionError;
    else if (winUsbDllError == ERR_WINUSBDLL_NO_ERR)
        err = NoError;
    else
        err = WriteError;

    return err;
}

/* input to pipe, returns nr of bytes received  */
int WINUSBDLLCommunication::getFromDev(unsigned char pipeToPc, int maxbytesToDo, unsigned char *pBytes, HANDLE hStopEvent, DWORD timeOut)
{
    int uiDone = 0;

    if ( hToPcStreamOvrlp == NULL)
    {
        hToPcStreamOvrlp = CreateEvent(NULL,TRUE,FALSE,NULL);
    }
    if ( hToPcStreamOvrlp != NULL)
    {
        //if (waitForALock(900,lockToPcStream))		/* one only */
        {
            WINUSB_INTERFACE_HANDLE devUsbHndLoc = devUsbHnd; /* valid HANDLE when open */
            HANDLE hDeviceLoc = hDevice;
            if (( devUsbHndLoc != NULL) && (devUsbHndLoc != INVALID_HANDLE_VALUE)
                    && (hDeviceLoc != INVALID_HANDLE_VALUE)) /* asynchronous problem */
            {

                bool bIsInData = false;
                DWORD dataXfrd = 0;

#if 0   // this is done in the MsgThread. needed?
                /* request the device list */
                pThis->writeUSBVendorRequest(USBAS3940_VENDORREQ_ACTIVE_CLIENTS, 0, (uint8_t*) "A", 0, 0, 100);
#endif

                ResetEvent(hToPcStreamOvrlp);
                memset(&ovrLapToPcStream, 0, sizeof(OVERLAPPED));
                ovrLapToPcStream.hEvent = hToPcStreamOvrlp;
                /* NEED TO MAKE THIS BETTER */
                if (WinUsb_ReadPipe(devUsbHndLoc, 0x81, pBytes, maxbytesToDo, &dataXfrd, &ovrLapToPcStream))
                {
                    bIsInData = true;
                }
                else
                {
                    bIsInData = false;
                }
                if (!bIsInData) /* pending */
                {
                    DWORD derr = GetLastError();
                    if ((derr == ERROR_IO_PENDING) || (derr == ERROR_IO_INCOMPLETE))
                    {
                        bIsInData = true;
                    }
                    else
                    {
                        WinUsb_ResetPipe(devUsbHndLoc, 0x81);
                    }
                }
                if (bIsInData)
                {
					DWORD endWaitReason;
                    HANDLE hWait[2];
                    int nrHandles = 1;
                    hWait[0] = hToPcStreamOvrlp;
                    hWait[1] = hStopEvent;
                    if ( hStopEvent != 0)
                    {
                        nrHandles++;
                    }
                    dataXfrd = 0;
                    endWaitReason = WaitForMultipleObjects(nrHandles,hWait,FALSE,timeOut); /* timeout or rcvd */
                    /* wait for data or the end */
					if(endWaitReason != 0 && endWaitReason != 0x102)   // timeout
						qDebug() << "WaitForMultipleObjects returned " << endWaitReason;

                    if ( WinUsb_GetOverlappedResult( devUsbHndLoc, &ovrLapToPcStream, &dataXfrd, false) == 0)
                    {
                        DWORD derr = GetLastError();
                        CancelIo(hDeviceLoc);
                        bIsInData = false;
                    }
                    else if ( dataXfrd == 0)
                    {
                        bIsInData = false;
                    }
                    else
                    {
                        uiDone = (int) dataXfrd;
                    }
                }

                //InterlockedDecrement(&lockToPcStream); /* release lock*/
            }
        }
    }
# if (DEBUG_SWITCH_SHOW_RECEIVED_BYTES == 1)
	qDebug() << uiDone;
#endif
    return uiDone;
}

/**
 * This function can be used to send special commands to the device
 */
AMSCommunication::Error WINUSBDLLCommunication::hwSendCommand(QString command, QString * answer)
{
    //unsigned char buffer[MAX_PACKET_LENGTH];
    //bool result;
    int openIdx, closeIdx, i;
    bool ok;

    WinUsbDllError winUsbDllError = ERR_WINUSBDLL_NO_ERR;
    AMSCommunication::Error err = NoError;

    if (this->connected)
    {
        if (command.contains("SendClientData"))
        {
            //DWORD NrOfBytesWritten;

            openIdx = command.indexOf('(');
            closeIdx = command.indexOf(')', openIdx);
            command = command.mid(openIdx + 1, closeIdx - openIdx - 1);
            i = 0;
#if 0
            while ((i*2 < command.size()) && (i < outPacketLength))
            {
                buffer[i++] = command.mid(i*2, 2).toInt(&ok, 16);
            }
            if (hUsbWrite != INVALID_HANDLE_VALUE)
            {
                result = WriteFile(hUsbWrite, &buffer, outPacketLength, &NrOfBytesWritten, NULL);
                if (result == false)
                {
                    winUsbDllError = ERR_WINUSBDLL_CMD_FAILED;
                }
            }
            else
            {
                winUsbDllError = ERR_WINUSBDLL_CMD_FAILED;
            }
#endif
        }
        else if (command.contains("ReceiveClientData"))
        {
            unsigned char curDevClient[1 + USBAS3940_SPI_CLIENTINFO_TABLE_ELMLEN]; /* client data */
            int m_Client_sel_1_7;
            int NrOfBytesRead;

            openIdx = command.indexOf('(');
            closeIdx = command.indexOf(')', openIdx);
            command = command.mid(openIdx + 1, closeIdx - openIdx - 1);
            m_Client_sel_1_7 = command.left(2).toInt(&ok, 16);

            memset(curDevClient, 0, sizeof(curDevClient));
			if((this->hDevice == NULL || this->hDevice == INVALID_HANDLE_VALUE) || (this->devUsbHnd == NULL || this->devUsbHnd == INVALID_HANDLE_VALUE))
			{
				answer->clear();
				return ReadError;
			}
            NrOfBytesRead = readUSBVendorRequest(USBAS3940_VENDORREQ_SPI_READ, 1+(USBAS3940_SPI_CLIENTINFO_TABLE_ELMLEN), (unsigned char*) &curDevClient[0], (USBAS3940_SPI_CLIENTINFO_TABLE_BASE) + ((USBAS3940_SPI_CLIENTINFO_TABLE_ELMLEN) * m_Client_sel_1_7), 0, 100);
            if (NrOfBytesRead  >= (1 + (USBAS3940_SPI_CLIENTINFO_TABLE_ELMLEN)))
            {
                if (curDevClient[0] == 0)  // no error
                {
                    // AS3940_ClientData struct drüber legen um an daten zu kommen, see decodeClientData in ams_como_if.cpp
                    answer->clear();
                    for (int i = 1; i < NrOfBytesRead; i++)
                    {
                        *answer += QString::QString("%1").arg(QString::number(curDevClient[i], 16), 2, '0');
                    }
                }
                else
                {
                    winUsbDllError = ERR_WINUSBDLL_RECEIVE;
                }
            }
            else
            {
                winUsbDllError = ERR_WINUSBDLL_GENERIC;
            }
        }
        else if (command.contains("getMasterBatteryLevel"))
        {
            unsigned char szTmp[10];

            if (readUSBVendorRequest(USBAS3940_VENDORREQ_GET_MASTER_BATTERY_LEVEL, 2, (unsigned char*) &szTmp[0], 0, 0, 100) >= 2)
            { /* error code + value bit 7 */
                answer->clear();
                *answer += QString::QString("%1").arg(QString::number(szTmp[1], 16), 2, '0');
            }
            else
            {
                winUsbDllError = ERR_WINUSBDLL_RECEIVE;
            }
        }
        else if (command.contains("getActiveClients"))
        {
            unsigned char szTmp[10];

            if (readUSBVendorRequest(USBAS3940_VENDORREQ_ACTIVE_CLIENTS, 3, (unsigned char*) &szTmp[0], 0, 0, 100) >= 3)
            { /* error code + value bit 7 */
                answer->clear();
                *answer += QString::QString("%1").arg(QString::number(szTmp[1], 16), 2, '0');
            }
            else
            {
                winUsbDllError = ERR_WINUSBDLL_RECEIVE;
            }
        }
        else if (command.contains("getDeviceID"))
        {
            unsigned char tmpBytes[4];
            bool AS3940Type;
            int AS3940_devId;

            answer->clear();
            /* check on as3940 mtype and get additional data */
            if ((readUSBVendorRequest(USBAS3940_VENDORREQ_AS3940_DEVID, 4, tmpBytes, 0, 0, 200) == 4) && (tmpBytes[0] == 0))  // OK
            {
                AS3940Type = true;
                AS3940_devId = tmpBytes[1];
                AS3940_devId = (AS3940_devId << 8) | tmpBytes[2];
                AS3940_devId = (AS3940_devId << 8) | tmpBytes[3];

                *answer = QString::QString("Device ID: 0x%1").arg(QString::number(AS3940_devId, 16), 6, '0');
            }
            else
            {
                winUsbDllError = ERR_WINUSBDLL_RECEIVE;
            }
        }
        else if (command.contains("getFwVersion"))
        {
            unsigned char tmpBytes[9];
            int AS3940_majorSwVersion, AS3940_minorSwVersion, AS3940_majorIFVersion, AS3940_minorIFVersion;

            answer->clear();
            if ((readUSBVendorRequest(USBAS3940_VENDORREQ_FW_VERSION, 9, tmpBytes, 0, 0, 200) == 9) && (tmpBytes[0] == 0))  // OK
            {
                AS3940_majorSwVersion = (((int)(*(tmpBytes+2))) << 8) | (*(tmpBytes+1));
                AS3940_minorSwVersion = (((int)(*(tmpBytes+4))) << 8) | (*(tmpBytes+3));
                AS3940_majorIFVersion = (((int)(*(tmpBytes+6))) << 8) | (*(tmpBytes+5));
                AS3940_minorIFVersion = (((int)(*(tmpBytes+8))) << 8) | (*(tmpBytes+7));

                *answer = QString::QString("FW Version: %1.%2, IF Version: %3.%4").arg(AS3940_majorSwVersion).arg(AS3940_minorSwVersion).arg(AS3940_majorIFVersion).arg(AS3940_minorIFVersion);
            }
            else
            {
                winUsbDllError = ERR_WINUSBDLL_RECEIVE;
            }
        }
        else if (command.contains("getStatus"))
        {
            unsigned char tmpBytes[5];
            int AS3940_LastError, AS3940_Mode;

            answer->clear();
            if ((readUSBVendorRequest(USBAS3940_VENDORREQ_STATUS, 5, tmpBytes, 0, 0, 200) == 5) && (tmpBytes[0] == 0))  // OK
            {
                AS3940_LastError = (((int)(*(tmpBytes+2))) << 8) | (*(tmpBytes+1));
                AS3940_Mode = (((int)(*(tmpBytes+4))) << 8) | (*(tmpBytes+3));

                if (AS3940_Mode == 1)
                {
                    *answer = "AS3940 Master";
                }
                else if (AS3940_Mode == 2)
                {
                    *answer = "AS3940 Client";
                }
            }
            else
            {
                winUsbDllError = ERR_WINUSBDLL_RECEIVE;
            }
        }
        else if (command.contains("readUSBVendorRequest"))
        {
            unsigned char request, length, index, value;
            unsigned char szTmp[1024 + 1];

            openIdx = command.indexOf('(');
            closeIdx = command.indexOf(')', openIdx);
            command = command.mid(openIdx + 1, closeIdx - openIdx - 1);

            request = command.mid(0, 2).toInt(&ok, 16);
            length = command.mid(2, 2).toInt(&ok, 16);
            index = command.mid(4, 2).toInt(&ok, 16);
            value = command.mid(6, 2).toInt(&ok, 16);

            if (readUSBVendorRequest(request, length + 1, (unsigned char*) &szTmp[0], index, value, 3000) == length + 1)
            {
                if (szTmp[0] == 0)  // no error
                {
                    answer->clear();
                    for (int i = 1; i < length + 1; i++)
                    {
                        *answer += QString::QString("%1").arg(QString::number(szTmp[i], 16), 2, '0');
                    }
                }
                else
                {
                    winUsbDllError = ERR_WINUSBDLL_RECEIVE;
                }
            }
            else
            {
                winUsbDllError = ERR_WINUSBDLL_GENERIC;
            }
        }
        else if (command.contains("writeUSBVendorRequest"))
        {
        }
        else if (command.contains("getFromDev"))
        {
            unsigned char buffer[1024];
            int receivedBytes;

            receivedBytes = getFromDev(/*COMOPROPSHEET_MSG_THREAD_DATA_TO_PC*/ 0x81, 1024, buffer, NULL, 1000);
            answer->clear();
            for (int i = 0; i < receivedBytes; i++)
            {
                *answer += QString::QString("%1").arg(QString::number(buffer[i], 16), 2, '0');
            }
        }
		else if (command.contains("readConfig"))
		{
			int received, expectedReception;
			unsigned char readRegister[0x201];

			if(command.contains("Master"))
			{
				expectedReception = 0x101;
				received = readUSBVendorRequest(USBAS3940_VENDORREQ_CONFIG_MASTER_READ, expectedReception, readRegister, 0, 0, 3000);
			}
			else if(command.contains("Client"))
			{
				expectedReception = 0x101;
				received = readUSBVendorRequest(USBAS3940_VENDORREQ_CONFIG_CLIENT_READ, expectedReception, readRegister, 0, 0, 3000);
			}
			else if(command.contains("Controller"))
			{
				expectedReception = 0x201;
				received = readUSBVendorRequest(USBAS3940_VENDORREQ_CONFIG_UCONTROLLER_READ, expectedReception, readRegister, 0, 0, 3000);
			}
			else
			{
				expectedReception = -1;
				received = 0x00;
			}

			if(received == expectedReception && readRegister[0] == 0x00)
			{
			}
			else
			{
				received = 0x00;
				winUsbDllError = ERR_WINUSBDLL_RECEIVE;
			}

			answer->clear();
			for (int i = 1; i < received; i++)
			{
				*answer += QString::QString("%1").arg(QString::number(readRegister[i], 16), 2, '0');
			}
		}
		else if (command.contains("writeConfig"))
		{
			QString data;
			int written, expectedWrite, openIdx, closeIdx, length;
			unsigned char writeRegister[0x200];
			bool ok;

			openIdx = command.indexOf('(');
            closeIdx = command.indexOf(')', openIdx);
            data = command.mid(openIdx + 1, closeIdx - openIdx - 1);

			if(data.length()/2 > 0x200)
				length = 0x200;
			else
				length = data.length()/2;

			for (int i = 0; i < length; i++)
			{
				writeRegister[i] = data.mid(i*2, 2).toInt(&ok, 16);
			}

			if(command.contains("Master"))
			{
				expectedWrite = length;
				written = writeUSBVendorRequest(USBAS3940_VENDORREQ_CONFIG_MASTER_WRITE, expectedWrite, writeRegister, 0, 0, 3000);
			}
			else if(command.contains("Client"))
			{
				expectedWrite = length;
				written = writeUSBVendorRequest(USBAS3940_VENDORREQ_CONFIG_CLIENT_WRITE, expectedWrite, writeRegister, 0, 0, 3000);
			}
			else if(command.contains("Controller"))
			{
				expectedWrite = length;
				written = writeUSBVendorRequest(USBAS3940_VENDORREQ_CONFIG_UCONTROLLER_WRITE, expectedWrite, writeRegister, 0, 0, 3000);
			}
			else
			{
				expectedWrite = -1;
				written = 0x00;
			}

			if(written != expectedWrite)
			{
				winUsbDllError = ERR_WINUSBDLL_RECEIVE;
			}
		}
        else
        {
            // unknown command
        }
    }
    else
    {
        winUsbDllError = ERR_WINUSBDLL_NOT_CONNECTED;
    }

    if (winUsbDllError  == ERR_WINUSBDLL_NOT_CONNECTED)
        err = ConnectionError;
    else if (winUsbDllError == ERR_WINUSBDLL_NO_ERR)
        err = NoError;
    else
        err = WriteError;

    return err;
}

WinUsbDllError WINUSBDLLCommunication::setI2CDeviceAddress(unsigned char registerAddress)
{
    WinUsbDllError winUsbDllError = ERR_WINUSBDLL_NO_ERR;

    this->devAddr = devAddr;
    this->pid = PID_STRING | devAddr;

    return winUsbDllError;
}

// this function has to be called right after constructing the class to overwrite the default parameters for your device
// the register map already depends in the readOnce function at startup on valid IDs to be able to read out the registers
void WINUSBDLLCommunication::setConnectionProperties(void* properties)
{
    // holds the PID of the desired HID device
    this->pid = ((winUsbDllConfigStructure*)properties)->pid;

    // holds the VID of the desired HID device
    this->vid = ((winUsbDllConfigStructure*)properties)->vid;

    // holds the packet length of IN packets
    // this option MUST be set to the correct value which corresponds with the device firmware
    // otherwise windows might produce a bluescreen
    this->outPacketLength = ((winUsbDllConfigStructure*)properties)->outPacketLength;

    // holds the packet length of OUT packets
    // this option MUST be set to the correct value which corresponds with the device firmware
    // otherwise windows might produce a bluescreen
    this->inPacketLength = ((winUsbDllConfigStructure*)properties)->inPacketLength;

    // holds the ID of the Packte which is implemented on the device to store the address where we want to read from
    // the packet with this ID will be used to send the address we want to read from to the device
    // the device is responsible to store this address until the actual read out is done
    // this packet ID is used in hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
    this->outReadRegisterPacketId = ((winUsbDllConfigStructure*)properties)->outReadRegisterPacketId;

    // holds the ID of the packet which returns a register value
    // in this packet the device writes the value of the register which address was set with the packet before
    // this packet ID is used in hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
    this->inReadRegisterPacketId = ((winUsbDllConfigStructure*)properties)->inReadRegisterPacketId;

    // holds the ID of the packet which writes a byte at a given address into the register of the device
    // this packet ID is used in hwWriteRegister(unsigned char registerAddress, unsigned char registerValue)
    this->outWriteRegisterPacketId = ((winUsbDllConfigStructure*)properties)->outWriteRegisterPacketId;
}

void WINUSBDLLCommunication::deviceNodeChanged()
{
	// have a look if a board with the correct PID is connected
#if _DEBUG
	if(isConnected())
#else
	if(!isConnected())
#endif
	{
		// try to connect to this board, if we are already connected, nothing will happen
		connect();
	}
	else
	{
		// there is no board with correct PID connected
		// we try to disconnect
		//disconnect();
		// we don't disconnect but send the signal to the main thread to first stop the comThread and then disconnect
		emit connectionChanged(false);
	}
}

void WINUSBDLLCommunication::disconnect()
{
	this->hwDisconnect();
	//emit connectionChanged(false);
    connected = false;
}
